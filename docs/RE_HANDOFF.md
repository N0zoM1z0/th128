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

## Recommended next steps for the next agent

Work in **subsystem-cohesive `/GL` waves** (`src/<Wave>.hpp`, `src/<Wave>.cpp`,
`probes/<wave>-context.cpp`, ~8–12s per cold link). Always pre-filter candidate
lists with Capstone to skip the ~22 natural-C++ `fsincos` frontiers.

### 1. Immediate near-exact waves already drafted on disk (1–2 edits from exact)

- **`AnmMgrWave2`** (`src/AnmMgrWave2.*`, committed): `DrawLayer4` (`0x00466710`,
  126/126B, 16B store-schedule diff), `RegisterWorldVmAtTail` / `RegisterUiVmAtTail`
  (`0x00466D60` / `0x00466E60`, 123/124B, -1B), `SetInterruptAndUpdate`
  (`0x004674F0`, 127/128B, -1B).
- **`AnmWave3`** (`src/AnmWave3.*`, committed): `DrawCircleOutline` (`0x00462D60`,
  368/368B, 1B SIB diff), `GetAnmVmDrawPosition` (`0x004630F0`, 108/108B, reg-swap).
- **`AnmMgrWave1`** (`src/AnmMgrWave1.*`, untracked): `CopyBackBufferToTexture`
  (`0x00466440`, 327/328B, -1B), `RenderAnmSpriteText` (`0x00466130`, 209/210B),
  `DrawTextAt` (`0x00466210`, 129/132B), `DrawCenteredText` (`0x004662A0`, 198/196B).
- **`AnmWave4`** (`src/AnmWave4.*`, untracked): `SetEffectState` (`0x0045E6C0`,
  132/133B, -1B), `DrawVmSprite` (`0x004616D0`, 144/142B), `DrawVmTriangleFan`
  (`0x00461A00`, 244/242B), `BuildViewCorners` (`0x00462070`, 100/104B).
- **`ScoreWave1`** (`src/ScoreWave1.*`, untracked): `PopupMgr::OnCalc`
  (`0x00443C00`, 161/161B, 2B `fld/fadd` operand swap), `AddScorePopup`
  (`0x00444010`, 204/210B).
- **`ReplayWave1`** (`src/ReplayWave1.*`, untracked): `ReplayManager::OnCalcPlayback`
  (`0x00441310`, 393/395B, -2B), `ReplayManager::OnDraw` (`0x004414E0`, 185/190B).
- **`ItemWave1`** (`src/ItemWave1.*`, committed): `Item::CollectPoint`
  (`0x00429120`, 223/231B).

### 2. High-yield existing frontier backlog (`94` units in `config/match-units.toml`, `50,591B`)

Many medium/large non-`fsincos` functions already have written C++ source in `src/`:
- `src/AnmTextureLoading.cpp`: `0x004645F0` (1,774B), `0x00464FD0` (511B)
- `src/CardInfActions.cpp`: `0x0040DD70` (1,496B), `0x0040E350` (398B)
- `src/HelpInf.cpp`: `0x00427CD0` (1,184B)
- `src/PopupNotifications.cpp`: `0x00424B80` (1,072B)
- `src/AnmFinalization.cpp`: `0x00465770` (1,000B)
- `src/TextTexture.cpp`: `0x004519C0` (817B), `0x00452840` (696B), `0x00452B00` (584B)
- `src/AnmVmEffects.cpp`: `0x004631A0` (764B), `0x004634A0` (561B)
- `src/EclResource.cpp`: `0x004703F0` (538B)
- `src/Lzss.cpp`: `0x00450860` (443B); `src/PbgArchive.cpp`: `0x0044F9A0` (384B)

### 3. Fast per-wave banking sequence

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
   `resources/progress.svg`, and the wave's `src/` + `probes/` files.
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
