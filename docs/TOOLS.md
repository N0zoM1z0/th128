# Tools and reproduction

> Current workflow: follow the **Highest priority** section in `AGENTS.md`.

Python 3.11+ is required for TOML parsing. `scripts/repo-python` selects an
interpreter that reproduces the four Capstone 5.0.6 file hashes in
`config/tools.lock.toml`, adding the ignored `.tools/capstone` package root
when needed. Public `python3 scripts/ci.py` needs no target, Ghidra, Wine,
compiler or Capstone.

| Need | Command |
| --- | --- |
| Verify original | `scripts/repo-python scripts/verify-target.py` |
| Validate ledgers | `scripts/repo-python scripts/validate-tracking.py --require-target` |
| Regenerate progress | `scripts/repo-python scripts/progress.py` |
| Attest analysis tools | `scripts/repo-python scripts/verify-analysis-tools.py` |
| Attest compiler payload | `scripts/repo-python scripts/verify-toolchain.py --check` |
| Run toolchain smoke | `scripts/repo-python scripts/verify-toolchain.py --execute` |
| Attest Ghidra project | `scripts/repo-python scripts/ghidra.py check` |
| Query Ghidra | `scripts/repo-python scripts/ghidra.py query .analysis/q.txt OPERATION ARGS...` |
| Decompile | `scripts/repo-python scripts/ghidra.py decompile .analysis/f.c 0x0047460E` |
| Compile a probe | `scripts/compile-probe.sh SOURCE build/probe/x.obj /TP /O2 /Ob1 /Oy- /Gy /MT /Isrc /GL` |
| Build exact unit | `scripts/repo-python scripts/build-match-unit.py --unit NAME` |
| Compare normal COFF unit | `scripts/repo-python scripts/compare-coff-function.py --unit NAME --json` |
| Compare linked unit | `scripts/repo-python scripts/compare-linked-function.py --unit NAME` |
| Cold-replay affected units | `scripts/repo-python scripts/replay-exact-units.py --source PATH` |
| Classify CRT/STL code | `scripts/repo-python scripts/scan-library-code.py` |
| Record a source mapping / exact claim | `scripts/repo-python scripts/record-source.py UNIT --module M --convention C --signature S --notes N --boundary B [--exact EVIDENCE]` |
| Side-by-side COFF diff (diagnostic) | `scripts/repo-python scripts/diff-coff-target.py OBJ SYMBOL ADDR SIZE` |
| Side-by-side linked-PE diff (diagnostic) | `scripts/repo-python scripts/diff-linked-target.py BUILD_DIR SYMBOL ADDR SIZE` |
| Name import slots | `scripts/repo-python scripts/import-slots.py 0x0049A254 ...` |
| List a linked owner's fields (for `linkages`) | `scripts/repo-python scripts/linked-fields.py BUILD_DIR SYMBOL SIZE` |
| Callers and strings of a function | `scripts/repo-python scripts/function-context.py ADDR...` (needs `ghidra.py architecture`) |
| Capture original title screen | `scripts/capture-title-screen.sh [OUTPUT.png]` |
| Public checks | `python3 scripts/ci.py` (`--full` adds the synthetic Oracle suites) |

Use the selected unit's full profile and support profiles when compiling;
the probe example is a game-code `/GL` profile, not a replacement for every
owner's recipe. `--source` selects exact owners by primary source; name
frontiers and other affected consumers explicitly with `--unit`. Record an
exact claim only after its strict comparator has passed.

Session recovery uses `verify-toolchain.py --check`. Use `--execute` for a
bounded toolchain smoke when setup/tool changes require execution, rather
than for ordinary notes or every checkpoint.

## Local tool selection

| Ignored path | Role |
| --- | --- |
| `.tools/ghidra` | Factory Ghidra 12.1.4 |
| `.tools/jdk` | Factory Temurin JDK 21.0.12.1+1 |
| `.tools/capstone` | Factory Capstone 5.0.6 Python package root |
| `.tools/winbind` | Factory hash-pinned Ubuntu `ntlm_auth` runtime |
| `.tools/msvc1000` | archaic-msvc/msvc1000 at the locked commit (VC10 RTM + Windows SDK 7.0A), trimmed: no Git objects, ATL/MFC, CRT sources, debug libraries or x64/IA64 SDK libraries |
| `.tools/wineprefix-msvc1000` | TH128-only 32-bit compiler Wine prefix |
| `.tools/wineprefix-runtime` | TH128-only prefix for running the original game; recreated on demand |
| `.tools/shared-runtimes.json`, `.tools/factory-runtime.py`, `.tools/factory-bin` | Factory shared-runtime selectors |

`scripts/bootstrap-local-tools.py --shared-tools DIR` links Ghidra/JDK/Capstone
and clones the compiler at its locked commit when missing; it then checks the
NTLM helper, analysis tools and compiler hashes. The compiler repository
declares no license: it is a private compatibility input, never vendored or
published.

## VC10 under Wine

Two Wine details are required for the full compile/resource/link path:

- **cvtres runtime alias.** VC10 RTM `cvtres.exe` imports
  `MSVCR100_CLR0400.dll`, normally installed by .NET 4 as a renamed
  `msvcr100.dll`. Without it every resource input fails with LNK1123.
  `verify-toolchain.py --execute` copies the prefix's own `msvcr100.dll` to
  `system32/msvcr100_clr0400.dll`. The pinned payload is never modified.
- **PDB server authentication.** `mspdbsrv.exe` uses Wine RPC NTLM
  authentication; a missing `ntlm_auth` shows up as misleading PDB errors.
  `scripts/ntlm_auth` runs the private helper from `.tools/winbind`, and
  `scripts/bootstrap-ntlm-auth.py --check` verifies it.

`scripts/run-headless-wine.sh` serializes heavy tools through
`.tools/heavy-tool.lock`, pins one CPU, runs at nice 10, clears `TMPDIR` and
disables Mono/Gecko and menu-builder prompts. Ghidra likewise runs with one
CPU, one analysis worker and a 768 MiB heap.

## Ghidra

Ghidra query addresses take a `0x` prefix, and `xrefs_to`/`xrefs_from`
take a reference limit before the addresses. `ghidra.py architecture`
writes the call-edge, global, string and metrics CSVs under
`.analysis/architecture/` that `function-context.py` reads.

`ghidra.py import` is a one-time inventory operation and refuses an existing
project. Later commands open `ghidra-project/TH128` read-only with no analysis,
after verifying the disk target, Program identity, base/entry, distributed
mapped bytes and the complete mapped `.text`. `inventory` refuses to overwrite
maintained source or exact rows. For ABI review the decompiler accepts
`u32@ADDRESS`, `u32-fast1@ADDRESS` and `cookie@ADDRESS` selectors; their
changes are rolled back before the read-only session ends.

## Running the original

`scripts/capture-title-screen.sh` runs the attested original under Xvfb. It
needs the game data next to the target: copy `th128.dat` and `thbgm.dat`
from your installation into `resources/` first (they are ignored and can be
deleted afterwards). It answers the first-start display dialog
with *window 640×480*, and crops the client area to `resources/title-screen.png`.
It uses the separate `.tools/wineprefix-runtime` prefix and a scratch
directory of links under `.analysis/runtime/`, so game configuration and
scores never land in `resources/` or the compiler prefix. The retained
title/attract capture used Wine 9.0's D3D9 and builtin
`d3dx9_42`; this is historical runtime evidence for the original, not proof
that the reconstructed native product runs.
