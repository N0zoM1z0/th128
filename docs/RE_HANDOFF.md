# TH128 reconstruction handoff

## Start here

Run `git status --short --branch` and `git log -5 --oneline`; dirty work is
ours to continue. Work against Japanese TH12.8 v1.00a only: private
`resources/th128.exe`, 752,640 bytes, SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
The code-generation target is **VC10 RTM build 30319**. The remote is the
public `N0zoM1z0/th128`; push after coherent checkpoints.

```bash
scripts/repo-python scripts/verify-target.py
scripts/repo-python scripts/validate-tracking.py --require-target
scripts/repo-python scripts/verify-toolchain.py --check
scripts/repo-python scripts/replay-exact-units.py --source src/PbgArchive.cpp
```

`build/` is disposable: `replay-exact-units.py` cold-rebuilds whatever it
replays. Keep the Ghidra project, the compiler Wine prefix,
`.analysis/architecture/` and the two review directories under `.analysis/`
(decision CSVs, notes and their tools). The runtime prefix and game data are
only needed for `capture-title-screen.sh` and are recreated on demand.

## Current state

Counts are generated in [PROGRESS](PROGRESS.md); the source map is in
[ARCHITECTURE](ARCHITECTURE.md), findings and tried variants in
[KNOWLEDGE_BASE](KNOWLEDGE_BASE.md). Boundary and origin review cover the
whole image (3,516 candidates; 50 origin rows open: 34 need VC10/D3DX
compiles, 16 CRT-band rows).

Two codegen lanes are in use; both are hypotheses about the original TU
split:

| Lane | Profile | Sources |
| --- | --- | --- |
| Normal COFF | `/TP /O2 /Oy- /Gy /MT` | Pbg file streams, AnmVm constructor and effect callbacks, `DebugPrint`, `WindowProc`, angle helpers, `SptInf`, `LaserDataInf`, script registry |
| Linked PE | `/TP /O2 /Ob1 /Oy- /Gy /MT /GL` (+`/EHsc` for the archive) behind `probes/` contexts | ZunTimer, AnmVm setters, `MenuCursor`, `Rng`, Pbg archive chain, laser points |

Lessons that decide most matches (details in the knowledge base): `/Oy-`;
`__stdcall` declarations or plain members for LTCG private receivers;
`#include <new>` for `delete[]`; the release logger as normal COFF; vector
struct assignment for integer copies; ordinary out-of-line definitions for
inline-assembly helpers; and letting globals and pointers escape in probes
(`&g_GameFlags`, aliasing timer speeds) so LTCG does not reorder loads and
stores the original keeps in order.

## Next useful work

1. **Close frontiers** (each lists its tried variants in the knowledge base):
   `CreateGameWindow` (frame and EBX/EDI), `MemFile::Seek` (ESI
   shrink-wrap), `ZunTimer::Add` (unscaled branch merge), the archive chain
   (`ReadHeader`, `ReadEntry`, `DecryptBlocks`, `DecompressLzss`),
   `SptResourceInf::Load` (register assignment), `AnmManager::GetVmById`
   (id passed in ECX), `LaserInfiniteInf::Slot01` and the laser slot-0
   points.
2. **Fixed-convention owners first.** Virtual methods (laser kinds,
   `EnemyInf`, `EclResourceInf`), Windows callbacks and table callbacks
   (the AnmVm effect table near `0x004B1E9C`) keep their conventions under
   LTCG. The remaining effect callbacks call `0x0045E770`, `0x00462A00`,
   `0x00462B90` and `0x00412100`, whose private conventions are needed
   first.
3. **High-fan-in helpers** unlock callers: `0x00467480` (197 call sites, VM
   value broadcast through `GetVmById`), `0x004670B0` (115), `0x0045EF60`.
4. **AsciiInf** (`0x00401010..0x00402A55`): resolve the 0x141-iteration timer
   loop before writing the constructor.
5. Product closure needs the August 2009 DirectX SDK (`d3dx9_42`); D3D9
   device calls are currently out of reach for that reason.
