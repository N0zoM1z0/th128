#!/usr/bin/env bash
# Run the attested original under Xvfb and capture its 640x480 title screen.
#
# Needs the private resources/th128.exe, th128.dat and thbgm.dat. The game
# runs in its own .tools/wineprefix-runtime (never the compiler prefix) from a
# scratch directory of links, so its configuration and score files stay out of
# resources/.
set -euo pipefail

repo_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
output=${1:-$repo_root/resources/title-screen.png}
title_delay=${TH128_TITLE_DELAY:-10}

if [[ -z "${TH128_CAPTURE_INNER:-}" ]]; then
  python3 "$repo_root/scripts/verify-target.py" >/dev/null
  for data in th128.dat thbgm.dat; do
    [[ -f "$repo_root/resources/$data" ]] || { echo "missing resources/$data" >&2; exit 1; }
  done
  for tool in xvfb-run xdotool import convert; do
    command -v "$tool" >/dev/null || { echo "missing $tool" >&2; exit 1; }
  done
  exec env TH128_CAPTURE_INNER=1 xvfb-run -a -s "-screen 0 800x600x24" \
    "${BASH_SOURCE[0]}" "$(realpath -m "$output")"
fi

scratch="$repo_root/.analysis/runtime"
game="$scratch/game"
mkdir -p "$game"
for file in th128.exe th128.dat thbgm.dat; do
  ln -sfn "$repo_root/resources/$file" "$game/$file"
done

runtime=("$repo_root/scripts/shared_runtime.py" --prefix .tools/wineprefix-runtime)
cleanup() {
  "${runtime[@]}" wineserver -- -k >/dev/null 2>&1 || true
}
trap cleanup EXIT

cd "$game"
WINEDEBUG=-all WINEDLLOVERRIDES='winemenubuilder.exe=d;mscoree,mshtml=d' \
  "${runtime[@]}" wine -- th128.exe >/dev/null 2>&1 &

window_with_size() {
  local id size
  for id in $(xdotool search --onlyvisible --name . 2>/dev/null || true); do
    size=$(xdotool getwindowgeometry "$id" 2>/dev/null | awk '/Geometry/ {print $2}' || true)
    if [[ $size == "$1" ]]; then
      echo "$id"
      return
    fi
  done
}

# The startup dialog offers fullscreen/window sizes; choose window 640x480.
game_window=
for _ in $(seq 60); do
  sleep 1
  dialog=$(window_with_size 179x154)
  if [[ -n $dialog ]]; then
    xdotool mousemove --window "$dialog" 17 49 click 1
    sleep 0.5
    xdotool mousemove --window "$dialog" 87 133 click 1
  fi
  game_window=$(window_with_size 640x480)
  [[ -n $game_window ]] && break
done
[[ -n $game_window ]] || { echo "game window did not appear" >&2; exit 1; }

sleep "$title_delay"
read -r x y < <(xdotool getwindowgeometry "$game_window" | awk -F'[ ,]+' '/Position/ {print $3, $4}')
import -window root "$scratch/root.png"
convert "$scratch/root.png" -crop "640x480+$x+$y" +repage -strip "$output"
echo "wrote $output"
