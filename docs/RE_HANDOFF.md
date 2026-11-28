# TH128 reconstruction handoff

Prepared 2026-11-28 for the next agent. Last executable-code checkpoint:
**`fb7f377` — `ItemWave1` with exact `ItemManager::~ItemManager` (`0x00428430`, +203B)**.
No background job remains running. Local `main` is 3 commits ahead of
`origin/main` (`aa12500`); no push was performed.

## Resume

1. Read [AGENTS.md](../AGENTS.md), [workflow](RE_WORKFLOW.md),
   [Oracles](ORACLES.md), and the relevant source/ledger rows.
   Preserve dirty/untracked work; inspect `git status --short --branch`
   and `git log -12 --oneline` before editing.
2. Before target-dependent work in a new session, run once:

   ~~~bash
   scripts/repo-python scripts/verify-target.py
   scripts/repo-python scripts/validate-tracking.py --require-target
   scripts/repo-python scripts/verify-toolchain.py --check
   ~~~

3. Analyze through `scripts/repo-python scripts/ghidra.py` (or direct PE/Capstone
   inspection of `resources/th128.exe`). Ghidra boundaries/types/decompilation
   remain provisional hypotheses, never exactness evidence.
4. Keep **VC10 RTM** (`cl 16.00.30319.01`, `link 10.00.30319.01`) and `/GL`
   ownership. Run **only one Wine/VC10 build at a time** on
   `.tools/wineprefix-msvc1000`. Documentation/handoff edits need diff review
   only (`git diff --check`), not a compiler or replay.

Only use ignored `resources/th128.exe`: original Japanese v1.00a,
752,640 bytes, SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
[Shared tools](SHARED_TOOLS.md) explains the isolated Factory runtime.

## Current state

[PROGRESS.md](PROGRESS.md) is generated from canonical ledgers (`scripts/repo-python scripts/progress.py`).
At `fb7f377`:

| Measure | Count |
| --- | ---: |
| Tracked 1.00a function candidates | 3,516 |
| Boundary reviewed / provisional | 3,452 / 64 |
| Origin review pending | 21 |
| Confirmed authored functions / bytes | 2,226 / 435,812B |
| Source-present mappings (authored / pending) | 1,459 (1,423 / 16) |
| Authored source-present exact backlog | 89 |
| **Canonical exact functions / bytes** | **1,369 / 99,212B** |
| **Canonical exact authored functions / bytes** | **1,334 / 97,817B (22.44%)** |

Remaining non-exact confirmed authored functions (`892` funcs, `337,995B`) by size bucket:

- `1–49B`: 314 funcs (`6,932B`)
- `50–99B`: 118 funcs (`8,324B`)
- `100–299B`: 193 funcs (`34,982B`)
- `300–999B`: 186 funcs (`95,741B`)
- `1000+B`: 81 funcs (`192,016B`)

`config/matches.csv` grants exact credit; `config/match-units.toml` defines
each reproducible owner context, complete extent and exhaustive address fields.
The native Windows product has not been closed or runtime-validated.

## Latest verified work

Recent subsystem-cohesive commits advanced exact reconstruction across
`AnmManager`, `ScreenFade`, `EngineWave1`, `LowFpuWave`, `CleanSmallWave`, and `ItemWave1`:

| Commit | Wave | Exact units landed | Bytes |
| --- | --- | --- | ---: |
| `fb7f377` | `ItemWave1` | `ItemManager::~ItemManager` (`0x00428430`, `/EHsc`) | +203B |
| `8c8cb69` | `AnmWave3` | `AnmManager::DrawFilledCircle` (`0x00462A00`) | +396B |
| `68fd8e1` | `AnmMgrWave2` | `AnmManager::DrawLayer21`/`DrawLayer24`/`DrawLayer32`/`DrawVmGroup` | +530B |
| `aa12500` | `ScreenFadeWave` | `SfwDrawSolidRect`, `SfwDrawGradientRect`, `OnDrawFade`, `OnCalcFlash`, `OnCalcShakeA` | +1,541B |
| `9face45` | `EngineWave1A/B` | `RotateVec2`, `PointInRotatedRect`, `CircleInsideRotatedRect`, `AdvanceAngle`, `GetU32AndSkip2`, `SumBytes` | +638B |
| `efec314` | `LowFpuWaveC` | `PauseDrawTargetA`/`B`, `AbilityMgr::AbilityMgr`, `SptResourceInf::CreateVm` | +588B |
| `ab02d7c` | `CleanSmallWaveD` | 6 lifecycle/setup functions (`0x0040EA10..0x00456540`) | +413B |

See [KNOWLEDGE_BASE.md](KNOWLEDGE_BASE.md#vc10-ltcg-subsystem-wave-findings-2026-11-28-gemini-4-neon)
for the reusable VC10 LTCG codegen patterns and probe idioms from these waves.

## Roadmap: large-function-first strategy (updated 2026-11-28)

Small functions (`< 300B`, 625 funcs) account for only `50,238B` (`14.9%`) of
the remaining `337,995B` authored backlog, whereas **`>= 1,000B` (81 funcs)
account for `192,016B` (`56.8%`)** and `300–999B` (186 funcs) account for
`95,741B` (`28.3%`). Moreover, among the 81 remaining `>= 1,000B` functions,
**only 1 uses `fsincos`** (`1,171B`) and **only 2 use C++ EH** (`0x00417FF0`
and `0x004055F0`); the other 79 (`170.2 KB`) are plain non-EH C++.

**Prioritize large functions in 4 ordered phases** rather than sweeping small
functions in isolation:

### Phase 1 — Low-call high-yield large functions (`>= 1,700B`, `calls <= 41`)

Tackle one function per isolated TU (`src/<Fn>.cpp` + `probes/<fn>-context.cpp`)
so LTCG interprocedural register noise stays minimal. These 11 functions alone
represent **~29.8 KB** (`+6.8%` of total authored bytes):

| Address | Size | Calls | Notes |
| --- | ---: | ---: | --- |
| `0x004645F0` | 1,774B | 6 | `AnmHelpers::FillTransparentTextureColors` (existing frontier in `src/AnmTextureLoading.cpp`) |
| `0x0044A440` | 3,951B | 14 | Low-call large non-EH |
| `0x0041D2C0` | 2,392B | 11 | Low-call large non-EH |
| `0x00401AD0` | 2,296B | 12 | `AsciiInf` band, low-call non-EH |
| `0x004091F0` | 2,196B | 16 | `Bullet` band, low-call non-EH |
| `0x00460270` | 1,868B | 12 | `AnmDraw` band, low-call non-EH |
| `0x0042C180` | 1,773B | 17 | `Laser`/`Stage` band, low-call non-EH |
| `0x00422680` | 1,750B | 20 | `Gui` band, low-call non-EH |
| `0x0042AC10` | 1,734B | 12 | `Laser`/`Item` band, low-call non-EH |
| `0x00409A90` | 3,680B | 31 | `Bullet` band, non-EH |
| `0x004210B0` | 5,438B | 41 | `Gui` band, non-EH |

### Phase 2 — Close existing large/medium frontiers in `config/match-units.toml` (`50,591B` already written)

Close large non-`fsincos` functions that already have C++ source in `src/`
before writing new code from scratch:

- `0x0045E920` (1,532B) & `0x00461B00` (1,392B) — `src/AnmDraw.cpp`
- `0x0040DD70` (1,496B) & `0x0040E350` (398B) — `src/CardInfActions.cpp`
- `0x00427CD0` (1,184B) — `src/HelpInf.cpp`
- `0x00424B80` (1,072B) — `src/PopupNotifications.cpp`
- `0x0045F110` (1,209B) & `0x0045F610` (1,020B) — `src/AnmVmView.cpp`
- `0x00465770` (1,000B) — `src/AnmFinalization.cpp`
- `0x004519C0` (817B), `0x00452840` (696B), `0x00452B00` (584B) — `src/TextTexture.cpp`
- `0x004631A0` (764B) & `0x004634A0` (561B) — `src/AnmVmEffects.cpp`
- `0x004703F0` (538B) — `src/EclResource.cpp`; `0x00450860` (443B) — `src/Lzss.cpp`; `0x0044F9A0` (384B) — `src/PbgArchive.cpp`

### Phase 3 — The three mega-VM opcode dispatchers (`39,841B` = `9.1%` of the game)

Align opcode `switch` cases incrementally (verify jump-table shape, stack frame
size, and per-case byte counts via Capstone):

1. `0x00459D90` `AnmVm::Update` (**13,604B**, 289 calls) — closest to exact
   (already in `src/AnmVmScript.cpp`, ~84B off).
2. `0x0046E100` `EclVm::Run` (**7,453B**, 116 calls) — WIP already on disk in
   `src/EclVmRun.cpp`.
3. `0x00417FF0` Enemy ECL opcode dispatcher (**18,784B**, 535 calls, `/EHsc`) —
   tackle after the first two VM dispatchers and `Enemy` struct layout stabilize.

### Phase 4 — Subsystem-ordered sweep of remaining `>= 1,000B` and `300–999B` (largest first)

Clear remaining `1,000B+` and `300–999B` functions subsystem-by-subsystem,
pulling in small callees only when needed to settle a large caller's ABI/layout:

1. `0x428000..0x42FFFF` (Laser / Item / Stage — 13 funcs `>= 1KB`, 20.0 KB)
2. `0x444000..0x44FFFF` (Title / Score / Replay — 12 funcs `>= 1KB`, 26.0 KB)
3. `0x434000..0x43FFFF` (Player / Pause / GameManager — 11 funcs `>= 1KB`, 22.9 KB)
4. `0x404000..0x40FFFF` (Bullet / Ice / Card — 9 funcs `>= 1KB`, 20.8 KB)

### Large-function engineering rules

1. **One large function per TU:** Keep `>= 1,000B` targets in their own
   `src/<Name>.cpp` + `probes/<name>-context.cpp` rather than bundling with
   5–8 unrelated small functions.
2. **Structure-first diffing:** Match stack frame size (`sub esp`), switch jump
   tables, and callee register ABIs first; then binary-search the first diverging
   basic block / switch case with Capstone.
3. **Optional quick banking (existing near-exact WIPs on disk):** Only bank
   existing 1–2 byte near-matches when convenient (e.g., `AnmMgrWave2`
   `DrawLayer4`, `AnmWave3` `DrawCircleOutline` 1B diff, `AnmMgrWave1`
   `CopyBackBufferToTexture` -1B, `ScoreWave1` `PopupMgr::OnCalc` 2B diff,
   `ReplayWave1` `OnCalcPlayback` -2B); do not start new small-function waves.

### Per-unit exact banking sequence

1. Build & diff in one command (remember: **only one Wine/VC10 build at a time**).
2. Append `[units.<unit_id>]` to `config/match-units.toml` from `source.exe` + `source.map`.
3. Record non-exact source mapping:
   ~~~bash
   scripts/repo-python scripts/record-source.py <unit_id> \
     --module <Module> --convention <conv> --signature "<sig>" \
     --boundary "exact" --notes "<notes>"
   ~~~
4. Run targeted cold replay (never full cold replay):
   ~~~bash
   scripts/repo-python scripts/replay-exact-units.py --unit <unit_id> [...]
   ~~~
5. Re-run `record-source.py` with `--exact "build/probe-ltcg/<Wave>"`, then run:
   ~~~bash
   scripts/repo-python scripts/progress.py
   scripts/repo-python scripts/validate-tracking.py --require-target
   ~~~
6. Stage all 6 tracking files (`config/match-units.toml`,
   `config/function-boundaries.csv`, `config/functions.csv`,
   `config/implemented.csv`, `config/matches.csv`,
   `config/reccmp-functions.csv`) plus `docs/PROGRESS.md`,
   `resources/progress.svg`, and the unit's `src/` + `probes/` files.
   Finish with `git diff --cached --check` and commit.

## Preserved dirty / untracked WIP & cleaned storage

- **Storage cleaned:** Temporary build artifacts in `build/` (~105 MB) and 185
  unreferenced scratch dumps in `.analysis/` (~5.7 MB) were removed.
  `resources/th128.exe`, `.tools/wineprefix-msvc1000`, `ghidra-project/`,
  `.analysis/architecture/`, `.analysis/factory-runtime/`, and all files
  referenced by `docs/*.md` are intact.
- **Preserved WIP on disk** (do not run `git clean -fdx`):
  - Tracked modified WIP (pre-existing): `src/EclVmRun.hpp`, `src/EclVmRun.cpp`,
    `probes/ecl-vm-run-context.cpp`.
  - Untracked WIP waves: `AnmMgrWave1`, `AnmWave4`, `ReplayWave1`, `ScoreWave1`,
    `CleanMedWave1/2`, `EngineWave1C/D`, `TextTextureWave`, `SupervisorFrameWave`,
    `AnmDrawLeafWave1`, `EhBatchD/E/G/H`, plus earlier scratch waves.
  - Active pre-dumped disassemblies kept in `.analysis/`:
    `anm-mgr-wave1-disasm.txt`, `anm-wave4-disasm.txt`,
    `replay-wave1-disasm.txt`, `score-wave1-disasm.txt`,
    `laser-wave1-disasm.txt`.

## References

- Historical checkpoint archive: [RE_CHECKPOINT_NOTES.md](RE_CHECKPOINT_NOTES.md)
- Compiler/target findings and falsified hypotheses: [KNOWLEDGE_BASE.md](KNOWLEDGE_BASE.md)
- Linked Oracle contract: [LINKED_ORACLE.md](LINKED_ORACLE.md)
