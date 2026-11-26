# TH128 reconstruction handoff

Snapshot: 2026-11-26 (Scratch-image lifetime and font setup: five new exact owners).

## Resume safely

1. Run git status --short --branch and git log -5 --oneline. Preserve
   existing dirty/untracked work; do not reset, rebase or clean it away.
2. Work only against the original Japanese TH12.8 v1.00a target:
   resources/th128.exe, 752,640 bytes, SHA-256
   ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4.
3. Before target-dependent analysis in a new session, run:

   ~~~bash
   scripts/repo-python scripts/verify-target.py
   scripts/repo-python scripts/validate-tracking.py --require-target
   scripts/repo-python scripts/verify-toolchain.py --check
   ~~~

4. Use scripts/ghidra.py for target analysis. It verifies the disk target,
   loaded Program and complete mapped .text before each query. Decompiler
   output remains a hypothesis until source and compiler evidence settle it.

Local commits are reconstruction checkpoints, not remote publication.
Recheck the live branch and remote state before any push; no push was
performed for this checkpoint.

## Current ledger status

docs/PROGRESS.md and resources/progress.svg are generated from the
canonical ledgers. At this snapshot:

| Measure | Current value |
| --- | ---: |
| Function candidates | 3,516 (provisional denominator) |
| Boundary reviewed / provisional | 3,452 / 64 |
| Origin rows pending | 21 |
| Authored / excluded / indeterminate | 2,226 / 1,269 / 0 |
| Source-present mappings | 449 |
| Source-present with authored origin | 440 |
| Authored source-present exact backlog | 92 |
| Canonical exact functions | 357 |
| Canonical exact codegen bytes | 26,077 |
| Canonical exact authored bytes | 25,717 |

The candidate inventory is not proven complete. The 21 remaining pending origin
rows are unreferenced `d3dx9math.inl` candidates (all 16 CRT-band boundary-review
rows and all 12 LTCG-entry-adapter rows were closed in Phase 0). Exact functions,
source mappings and a clean Git tree do not prove that the native Windows product
is complete or runnable. The native product has not been closed and
runtime-validated.

## Completed Phases 0–2 summary (2026-11-25)

Four local checkpoints (`a56cce6`, `c46e1f8`, `d8e26b4`, `c8c11fd`) advanced the
canonical ledger from **102 exact / 6,620 B** to **303 exact / 17,121 B** (+201
cold-replayed exact functions, +10,501 exact bytes):

- **Phase 0 (`a56cce6`)**: Closed 16 CRT-band and 12 LTCG-entry-adapter pending
  origins; documented whole-binary VC10 RTM flags (`/O2 /Oy- /fp:precise /GS /GL /OPT:NOICF`)
  and coarse engine/gameplay map in `docs/KNOWLEDGE_BASE.md`.
- **Phase 1A (`c46e1f8`, +52 exact / +4,180 B)**: `SupervisorTime` (1/1),
  `SoundManager` (17/17), `MenuCursor` (3/3), `ThreadInf` (8/8), `Chain` (13/19),
  `FileSystem` (7/8), `GameErrorContext` (2/2), `EngineUtil` (2/5).
- **Phase 1B (`d8e26b4`, +42 exact / +2,148 B)**: `Input` (6/8), `CoreMath` (11/14),
  `CSoundManager` / `CSoundView` / `CWaveFileView` (25/29).
- **Phase 2 (`c8c11fd`, +107 exact / +4,173 B)**: `AnmManager` core + 23 `DrawLayer`
  callbacks + `DrawVmGroup` + `CleanupVm` + `FlushSpriteBatch` + `AreAllAnmsLoaded` +
  `ReleaseAnm` (30), `AnmResource` / `AnmTextureEntry` (3), `AnmHelpers` (32,
  including `ResolveAnmVmId` `0x00467780` via natural `struct AnmId` by-value ABI),
  `AnmInterp<T>` template + vector/angle/color helpers (39), and `AnmVmTransitions`
  (`SetScaleTransition`, `SetSecondaryScaleTransition`, `SetSecondaryEffectState`, 3).

Key compiler discoveries recorded in `docs/KNOWLEDGE_BASE.md`: `struct AnmId`
by-value naturally reproduces `GetVmById` (`0x00467420`) and 30+ callers without
any unused-`ECX` shim; `/Ob1` is required on ANM `/GL` units so out-of-line helpers
are not over-inlined; address-taken probe pointers prevent interprocedural
`and esp, -8` hoisting; struct copies unlock `fldz` hoisting in transition setters.

## Latest advance (2026-11-26)

Five complete owners in TextImageLifetime.cpp are newly exact, adding
**1,139B / 80 address fields**. The final source was cold-built in one
three-source context with real Rng.cpp support, then all five primary PDB
owners were compared with zero differences:

| Owner / target | Whole owner | Address fields |
| --- | ---: | ---: |
| DestroyTextImage / 0x004516B0 | 105B | 14 |
| InitializeDefaultTextImage / 0x00451720 | 22B | 2 |
| InitializeTextImage / 0x00451740 | 413B | 22 |
| InitializeTextFonts / 0x004525D0 | 516B | 32 |
| DestroyTextFonts / 0x004527E0 | 83B | 10 |

Primary flags remain pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`. Only the RNG support
in this new context uses `/O2 /Oy- /GL` without `/Ob1`, which permits the
observed GetU16 inlining in the 516B font owner. The first candidate kept an
out-of-line GetU16 and emitted only 405B. Existing Rng.cpp source and its
canonical primary context are unchanged. This matches a reproducible context;
it does not settle the original TU split or compiler settings.
[Finding and failed approaches](KNOWLEDGE_BASE.md#scratch-image-lifetime-and-font-setup-2026-11-26),
[complete results](../.analysis/text-image-lifetime-final-results.json),
[scope](../.analysis/text-image-lifetime-final-scope.json),
[cold build](../.analysis/text-image-lifetime-final-cold-build.log).

Creation uses a 108B BITMAPV4HEADER, width 1024, physical height **-129**,
logical stored height 128 and byte count pitch * 128. Preserve that extra
physical row. The mutable format table has six 24B records and a -1 sentinel:
formats 22/21/24/23/25/26/-1, with bpp and alpha/red/green/blue masks. Lookup
returns null only for requested -1; other misses return the sentinel address.
Do not add a new rejection. The font setup continues even if both format
attempts fail. CP932 full-width-MS Gothic/Mincho names and all eight font
heights/weights are maintained; cleanup deletes default/2/4/6/1/3/5/7 without
clearing font globals. The two unreferenced wrappers keep medium boundary
confidence even with exact codegen.

The typed-texture hypothesis for DrawTextToTexture was falsified in a private
cached link: the same complete 696B / 42-field owner retains ordinary
**+533/+580** differences. No maintained TextTexture/AnmText source, shared
header or existing canonical context changed, and no old caller replay was
needed for that rejected hypothesis. Do not repeat it.
[Typed result](../.analysis/texture-text-typed-observations.json).

Existing frontiers remain: TextTexture fill 823/817B, alpha inversion 166/166B
with the wrong three-field layout, ordinary drawing 696/696B with two byte
differences, alternate drawing 592/584B. Card Start/Finish are 1504/1496B and
374/398B; Popup Show 1020/1072B; sprite text 214/210B; text wrappers
142/132B, 215/196B, 201/193B. These are complete owner sizes. AddScore's
54B/three-field exact result is unchanged.
[Previous context results](../.analysis/texture-text-final-results.json).
The older CardInf::Update and Shutdown frontiers, and rejected CreateVm
scoped-lock experiment, are unchanged as well.

Next useful work is the adjacent four unreferenced color-conversion owners
0x004518E0 (66B), 0x00451930 (55B), 0x00451970 (30B), 0x00451990 (42B).
Their complete instructions and delimiters can be inspected in one bounded
query; their source types may also explain the alternate text fill frontier:

~~~bash
scripts/repo-python scripts/ghidra.py query .analysis/text-color-converters-span.txt review_span 0x004518E0 0x004519B9
~~~

Do not widen replay to old ANM/frame/Card families: their source/header/profile
inputs are unchanged. Tracking validation and final diff review passed for
this checkpoint. No push was performed.

## Active reconstruction: AnmVm

Recent local work has focused on AnmVm rendering, world matrices, fog,
primitives, transitions, per-VM update behavior and the operand resolver
family. Exact counts below come only from the canonical ledger and replay
receipts, not from commit count or source presence.

### Main boss: AnmVm::Update

- Target entry: 0x00459D90
- Target extent: 13,604 bytes, through 0x0045D2B3
- Source: src/AnmVmScript.cpp
- Ledger: source mapping exists; no accepted exact match in
  config/matches.csv
- State: partial C++ reconstruction, not exact

Current source covers script gating/termination, game-speed save/restore,
continuous angle/scale/texture motion, view-rectangle refresh, render-state
synchronization, transition updates, custom geometry, frame callback and timer
advancement. The latest selected VC10 `/GL` PDB contribution is 13,764 bytes
versus the 13,604-byte target (160 bytes larger), with 284 direct calls versus
286 in the target; it is still not an exact replay. Effect-10 geometry keeps
the signed count and mode-specific ordering: mode 9 loads its raw angle before
the divisor conversion; modes 13/14 normalize first, then compute the angle/V
steps; mode 14 normalizes again after local-center composition and before the
parent-center contribution. The ring closes through its advanced vertex
cursor, including the no-loop case. The Update-specific `SetPolarForUpdate`
leaf at 0x0045E010 remains a natural-C++ frontier: target FSINCOS versus source
cos/sin calls; do not use inline assembly. The view-corner helpers now preserve
the target's anchor-3 no-write path in modes 0 and 2/3, per-corner parent
translation order, and transient parent-Z additions before the final child-Z
store. Their LTCG frontiers are 834/1209 bytes (mode 0), 799/1020 (mode 2/3),
and 537/623 (mode 1); the target's private register ABI is not reproduced by
the natural C++ probe. The separate `InterpolateColorVector`
frontier is 719 bytes versus its 721-byte target;
its mode-8 path now uses target-style float time and 12 individually converted
Hermite terms in target basis order, but its private register ABI and x87
scheduling still differ. `EaseTransition` at 0x0046B190 now has a separate
linked-PE replay: 728-byte owner, 35 mapped relocation fields, and zero
normalized byte differences. Its 16 jump-table entries, handler order and
natural C++ easing expressions now reproduce the target. `ZunTimer::Increment`
is now an exact 88-byte out-of-line leaf, preserving the timer call edge inside
transition interpolation. `ResetTransitionTimer(void *)` now exactly replays
the 52-byte `FUN_00406F80` owner-relative reset helper: it receives a transition
owner and resets the embedded timer at `+0x30`, including lazy initialization
and the `g_TimeScale` relocation. Update opcodes 56, 100 and 59 now share this
natural C++ helper (three target and candidate call edges). Texture-offset
opcodes 107 and 108 now use the separate exact 52-byte `ResetScalarTransitionTimer`
helper at `0x0040CEA0`, resetting the timer at owner `+0x10`; both original
call edges are retained. Opcode 113 uses a third byte-identical 52-byte
`ResetViewTransitionTimer` copy at `0x0045DAD0`, also owner `+0x10`, and keeps
its separate target call edge. The four Update timer-frame writes now call
the exact 61-byte `SetUpdateTimerCurrent` copy at `0x00403280`, distinct from
`ZunTimer::SetCurrent` at `0x00403240`. `AnmManager::GetVmById` (`0x00467420`, 89 bytes) and `ResolveAnmVmId`
(`0x00467780`, 21 bytes) are both now exact via natural `struct AnmId` by-value
(`AnmManager::GetVmById(AnmId)` in `src/AnmManager.cpp` and
`AnmHelpers::ResolveVmIdPtr` in `src/AnmHelpers.cpp`). The old unused-ECX
shim unit has been removed; `anm-manager-get-vm-by-id` is the canonical lookup.
Opcode 45 is confirmed as `acos` by the target `__CIacos` call.
An earlier retained `AnmVm::Update` measurement, before the child member-return changes, was
13,628 bytes vs 13,604 target (3,721 vs 3,769 instructions; 286 vs 289 calls,
283 vs 286 direct). The opcode-40 world/UI branches now preserve both target
`Rng::GetU32` calls, and opcode 102 preserves both target `BindAnmFile` calls.
The remaining three direct-call deficit is `SpawnChildVm`: target has five
callsites while LTCG folds the four list-mode handlers (88/91/90/92) into one
candidate call, leaving two total. `SpawnChildVmAtPosition` remains 2/2. The
float/integer operand-address helper counts are 11/14 candidate vs 12/13
target. The random-float call is the exact 52-byte `Rng::GetFloat(float scale)`
leaf at `0x0040D130` (target inventory label `Rng::GetFloatScaled`);
`Rng::GetU32` is an exact 149-byte leaf with eight mapped relocations. The
`SpawnChildVm` member is now 293 bytes vs the complete 266-byte target
in the script context; its registration return ABI and control flow remain frontiers. Opcodes 40 and 41
select the world/UI RNG in separate branches; opcode 41 uses the exact
`Rng::GetFloat(float scale)` leaf at 0x0040D130. Opcodes 17, 27 and 42..46
share the target float-result store, opcode 47 uses `AddAngles(angle, 0)`, and
opcode 113 plus mode-13/14 geometry preserve the target angle-normalization
call paths. `NormalizeAngle` (144 bytes) and `AddAngles` (153 bytes) now both
replay exact as natural C++ normal-COFF leaves, including the float-rounded
negative-pi bound and x87-preserved initial sum in `AddAngles`. The integer/float
source and destination resolver call counts now match the target (82/93 and
13/12). Opcode 98 retains four calls to the exact
coordinate helper; the per-frame view-mode refresh uses inline coordinate
divisions and lower-bound clamps, as in the target. The view-mode interpolation
now retains the target's two world/UI call sites plus the position-transition
call (three `InterpolateVector` calls). Two timer-reset call sites remain
coalesced by the candidate compiler, leaving 284 direct calls versus 286 in
the target. The separately tracked `InterpolateColorVector` mode-8 helper now
uses a float-rounded normalized time and materializes its twelve converted
Hermite terms in target basis order; its current candidate is 650 bytes versus
the 721-byte target and remains a frontier. `ZunTimer::Increment` has been
moved out of line and replayed exact as an 88-byte linked-PE leaf. Continue
comparing the full control flow, branch ordering and side effects; do not mark
the boss exact from source presence or code size.

Primary evidence:
- .analysis/anm-vm-update-target.txt
- .analysis/anm-vm-update-full.asm
- .analysis/anm-vm-update-boss.asm
- .analysis/anm-vm-update-helpers.txt
- .analysis/anm-vm-update-leaves.txt

### Related ANM source

| Source | Scope | Status |
| --- | --- | --- |
| src/AnmVmScript.cpp | wait commands, opcode dispatcher, ready-instruction loop, view corners, easing and timer reset helpers | 8 owners exact (`EaseTransition`, 3 timer resets, `SetUpdateTimerCurrent`, positioned child creation, both resource binders); `AnmVm::Update` frontier |
| src/AnmVmTransitions.cpp | transition setup (`SetScaleTransition`, `SetSecondaryScaleTransition`, `SetSecondaryEffectState`, color/effect setup) | 5 exact (`0x0045E080`, `0x0045E190`, `0x0045E240`, `0x0045E2C0`, `0x0045E4D0`); primary-effect frontier |
| src/AnmVmEffects.cpp | render-state sync, world matrices, sprite/fog/primitive draw paths, effect 10 | 5 exact normal-COFF owners; renderer paths frontier |
| src/AnmVmOperands.cpp | four operand resolver owners (`0x00459900..0x00459CE0`) | 4/4 exact (linked PE) |
| src/AnmVmResource.cpp | `NormalizeAnmCoordinates` (`0x00459D50`) | exact (linked PE) |
| src/AnmManager.cpp | VM lookup, allocation, list cleanup, draw grouping, tick callbacks, `FlushSpriteBatch`, preload, 23 `DrawLayer*` callbacks | 35 exact (linked PE); DrawVmGroup, world update and interrupt frontiers remain |
| src/AnmLoading.cpp | first-stage ANM load, external texture caching, inline resource constructor contribution | constructor exact (linked PE); loader/cache frontiers |
| src/AnmFinalization.cpp | resolved sprite installation, one-entry finalization and pending-resource scheduler | `SetSprite` (145B) and scheduler (140B) exact; finalizer frontier |
| src/AnmTextureLoading.cpp | seven texture creation/loading/upload methods, format fallback, neighbor accumulation and transparent RGB repair | 7 owners exact (334B); 6 frontiers including the full 1,774B repair |
| src/HelpInf.cpp | help viewer lifecycle/update, UI VM creation, page/resource worker bodies and Chain callbacks | 6 owners exact (151B); Create, Initialize, Shutdown, Update, UI helper and resource worker frontiers |
| src/AnmDraw.cpp | draw dispatch, position, fog/state, sprite submission/append, rectangles/borders and buffer reset | fog Enable/Disable, ApplyState and AppendSprite exact (662B); remaining owners frontier |
| src/AnmViewport.cpp | 2D camera matrices and float tangent | compiler/header tangent exact (31B, excluded from authored totals); camera frontier |
| src/SupervisorFrame.cpp | adaptive, timed and vsync calc/draw entry points | 3 frontiers; real Present/screenshot/recovery family linked |
| src/SupervisorDevice.cpp | release/recreate render-target textures across all 38 ANM slots | 2 exact (289B) |
| src/SupervisorSurfaces.cpp | level-zero surface acquisition, three VM bindings and fallback viewport copy | exact (217B) |
| src/SupervisorPresent.cpp | adaptive/immediate/sleep presentation and device-loss recovery | immediate/sleep exact (504B); adaptive frontier (653B / two ordinary bytes) |
| src/SupervisorRenderState.cpp | initial D3D9 render/stage/sampler states and ANM cache invalidation | exact (802B) |
| src/SupervisorScreenshot.cpp | screenshot trigger, BMP pixel capture and asynchronous stream writer | capture/writer exact (872B); handler frontier |
| src/SupervisorSurfaceCopies.cpp | four queued back-buffer rectangle copies into ANM textures | frontier (369/371B) |
| src/AnmResource.cpp | `Release`, `AnmTextureEntry::Release`, `CreateVm` | 2 exact (linked PE); `CreateVm` frontier |
| src/AnmVmUpdateWrapper.cpp | manager binding update wrapper (`0x00407080`) | 20-byte target frontier; 5-byte GL body, 15-byte normal-COFF body |
| src/AnmHelpers.cpp | `ResolveAnmVmId` and 31 `AnmId`/VM/child/rotation/draw helpers (`0x00467570..0x00468100`) | 31 exact (linked PE); DrawSingleVm credit withdrawn; `ClearD3DTexture*` frontiers |
| src/AnmInterp.cpp | `AnmInterp<T>` template (`AnmColor3`/`float`/`int`/`ZunVec2`) and vector/angle/color helpers (`0x0045D330..0x0045E76F`) | 39 exact (linked PE) + frontiers |
| src/Rng.cpp | RNG primitives and scaled overloads | exact (linked PE) |

The two world-matrix paths are deliberately separate in
AnmVm::UpdateWorldMatrix:
- applyAlignment == true: anchor-adjusted path associated with target
  FUN_00460DD0 (0x00460DD0).
- applyAlignment == false: copy cached matrix then add VM translation,
  associated with target FUN_004609C0 (0x004609C0).

These are reverse-engineering hypotheses implemented in C++; neither path
has an accepted exact owner replay yet.

### Exact ANM owners

`config/matches.csv` now contains **149 exact owners with an ANM module**:
39 `AnmInterp`, 36 `AnmHelpers`, 16 `AnmVm`, 36 `AnmManager`,
9 `AnmResource`, 5 `AnmVmEffects`, 4 `AnmVmScript`, and 4 `AnmGraphics`, counted
through the maintained module in `config/functions.csv`. Do not expand this list
from source presence or successful compilation; only accepted rows in
`config/matches.csv` count as exact.

## Natural-source cleanup decision

The synthetic renderer and binding probe bodies are removed. The resulting
25 caller mismatches are retained as source frontiers rather than restoring
volatile argument-address tricks or fake returns to regain credit. Recover
their actual private ABI through the real draw leaves and link context.

The target helper ZunVec3::SetPolarXY at 0x00432E20 uses x87 FSINCOS. Its
former inline-assembly implementation has been replaced with natural C++
cos/sin calls, and its exact ledger row was withdrawn. This deliberately
reduces the exact count; do not restore inline assembly to recover that count.
The target codegen mismatch is an honest frontier. The four laser slot-0
callers remain frontiers as well.

## Next useful work (Phase 3+)

1. **Continue the real CardInf/text context**: Start/Finish, notification
   dispatch and the four text routines have complete source candidates. Recover
   the real texture-text backends at `0x00452840/0x00452B00` and font/DIB helpers
   before changing private caller interfaces. Preserve the full switch tables
   and rounding order. Keep the earlier Update floating-add and Shutdown CutImpl
   frontiers separate, with their natural interfaces and fixed profiles.
   The three calc/draw frame
   entries, 2D camera and all three Present paths now have source. Both texture
   walkers, the 217-byte surface restoration and 802-byte D3D state setup
   are exact, as are screenshot capture (520B) and its BMP writer (352B).
   The handler's FileExists inlining and the copy consumer's 369/371B
   load/local-layout difference remain open. Do not repeat the unsuccessful
   FsErrUtil-probe addition or move a mismatch into a support callee.
   Immediate/sleep Present are exact. Adaptive has complete 653B source,
   with only ordinary bytes +90/+92 remaining in its clamp; both max orders
   failed. Real Present/screenshot/copy bodies are integrated into the
   frame-entry contexts. Investigate original TU/link placement and the
   frame return/FP layout without repeating the failed expression probes.
   The reviewed mode-17 polar
   line strip at `0x00462D60` is another useful scope. Sprite/rectangle
   frontiers still include ambient rounding and FSINCOS; avoid repeating
   the documented vector/zero-lifetime probes.
   Preserve the 58-field dispatcher, two float-rounding stages and genuine
   manager/VM inputs. DrawLayer is exact again under real callees;
   DrawVmGroup and DrawSingleVm remain frontiers.
2. **Close ANM finalization dependencies**: resolve the transparent-color
   repair frontier (`0x004645F0`, 1,774B, including physical tables) and
   the external decoder (`0x00464FD0`, 511B) / embedded uploader
   (`0x004651D0`, 296B) frontiers. The five small helpers are exact; retain
   their real definitions while resolving the repair's local lifetimes and
   stack/register layout. All four direct callers now have source. The
   [historical checkpoint notes](RE_CHECKPOINT_NOTES.md) retain the upload
   path details; the real HelpInf caller at
   `0x00427CD0` is now present without closing the uploader's GS/alignment
   differences. Its two timer-call identities are now repaired through the
   shared maintained setter. The six HelpInf frontiers remain listed above;
   retain the real lifecycle bodies when investigating their linked context.
   The pixel, upload and GPU findings retain failed probes.
   Then revisit the
   1,000-byte finalizer and the loader/cache pair
   (`0x004653F0`, 515B / `0x00465680`, 231B).
   Other focused frontiers are `UpdateWorldVms` (`0x00466AF0`),
   the binding update wrapper (`0x00407080`, 20B), `CreateVm` (`0x004670B0`),
   `SetInterruptAndUpdate` (`0x004674F0`, 128B), `SetEffectState` (`0x0045E6C0`, 133B),
   `ClearD3DTexture` (`0x00467BD0`) and the
   four `AnmInterp<T>::Step` functions (`0x0045D3D0..0x0045DB20`), plus `AnmVm::Update`
   (`0x00459D90`, 13,604B).
3. **Phase 3 — D3D9/Window/Supervisor (`0x00451300..0x00458400`) & ECL/SPT Engine
   (`0x00410000..0x0041FFFF`, `0x0046E100..0x004708BC`)**: reconstruct `Supervisor`
   initialization/main loop and the ECL VM / `SptInf` resource layer next, reusing
   the `/Ob1`, `struct`-by-value, and address-escape patterns proven in Phases 1–2.
4. **Phase 4 — Gameplay modules (`0x00401000..0x0044F500`)**: `AsciiInf`, `Bullet`,
   `Ice`/`Card`, `EnemyInf`, `Gui`, `Laser*Inf`, `Player`, `Stage`, `GameManager`,
   `Replay`/`Title`/`Score`.
5. **Next useful command after the session identity checks**:

   ~~~bash
   scripts/repo-python scripts/ghidra.py query .analysis/texture-text-first-owned.txt read_bytes 696 0x00452840
   ~~~

   The new dispatcher and small sprite wrappers are reviewed in
   `.analysis/anm-render-dispatch-complete.txt` and
   `.analysis/anm-render-sprite-wrappers-review.txt`. For HelpInf Initialize,
   compare `.analysis/help-lifecycle-closure-Initialize-source.asm` with
   `.analysis/manual-local-helpers-review.txt`; its stack contract still
   differs from the candidate's EDI input.
   The HelpInf update owner's complete 1,184 bytes through `0x0042816F`, including
   alignment and five dispatch entries, are reviewed in
   `.analysis/manual-update-body-and-tail-review.txt` and
   `.analysis/manual-dispatch-table.txt`. Preserve the separate setter
   identity at `0x00403280` while narrowing the affected replay scope.
   For the repair, compare `.analysis/anm-colors-complete-callers-final-source.asm`
   with `.analysis/anm-colors-target.asm`. Its saved body review and separate
   table read cover the full target; instruction-only review must stop at
   `0x00464CA9`.

## Workflow and constraints

- Natural C/C++ only. No inline ASM, target-byte injection, inert padding,
  fake returns or compiler-profile tricks to disguise incomplete source.
- Do not run a full cold replay for routine progress. Use per-file compile
  probes during exploration; use the canonical Oracle only for the affected
  exact owner(s) and required dependencies.
- Make substantive local checkpoints with the required format:
  gpt-6.1-sol: <English summary> (current operator instruction).
- Keep source-present, exact codegen, product closure and runtime behavior as
  separate states.
- build/ is disposable generated output. Recreate it through repository
  scripts; never treat old objects/receipts as current proof.
- Never run git clean -fdx.

## Retained private state

Keep these; they are not disposable build clutter:

- resources/th128.exe and resources/title-screen.png
- ghidra-project/TH128
- .tools/msvc1000 and .tools/wineprefix-msvc1000
- .analysis/architecture/, .analysis/boundary-review/ and
  .analysis/origin-review/
- .analysis/anm-vm-* target disassemblies and decompilations

Generated compiler objects in build/, Python __pycache__, and Ghidra
runtime logs/compiled caches can be removed and recreated. Do not delete the
target, toolchain, compiler prefix, Ghidra project or reviewed ledger evidence.
