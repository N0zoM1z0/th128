# TH128 reconstruction handoff

Snapshot: 2026-11-24. Read this first, then AGENTS.md.

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
Recheck the live branch and remote state before any push; the repository
shell has no network access, so this workflow does not push changes.

## Current ledger status

docs/PROGRESS.md and resources/progress.svg are generated from the
canonical ledgers. At this snapshot:

| Measure | Current value |
| --- | ---: |
| Function candidates | 3,516 (provisional denominator) |
| Boundary reviewed / provisional | 3,452 / 64 |
| Origin rows pending | 49 |
| Authored / excluded / indeterminate | 2,214 / 1,253 / 0 |
| Source-present mappings | 149 |
| Source-present with authored origin | 144 |
| Authored source-present exact backlog | 54 |
| Canonical exact functions | 95 |
| Canonical exact codegen bytes | 5,489 |
| Canonical exact authored bytes | 5,231 |

The candidate inventory is not proven complete. The 49 pending origin rows
are 21 D3DX candidates, 12 LTCG-entry adapter candidates and 16 later-added
boundary-review candidates. Exact functions, source mappings and a clean Git
tree do not prove that the native Windows product is complete or runnable.
The native product has not been closed and runtime-validated.

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
`ZunTimer::SetCurrent` at `0x00403240`. Opcodes 40 and 41
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

### Related source

| Source | Scope | Status |
| --- | --- | --- |
| src/AnmVmScript.cpp | wait commands, opcode dispatcher, ready-instruction loop, view corners and texture rectangle | source-present; not exact |
| src/AnmVmTransitions.cpp | transition setup and interpolation | source-present; not exact |
| src/AnmVmEffects.cpp | render-state sync, world matrices, sprite/fog/primitive draw paths, effect 10 | mixed exact and frontier owners; check each ledger row |
| src/AnmVmOperands.cpp | four operand resolver owners | four complete linked-PE owners exact; replay affected owners after edits |
| src/Rng.cpp | RNG primitives and scaled overloads | unsigned/signed scaled helpers exact; opcode 41 uses unsigned scaled overload |
| src/AnmVmResource.cpp | VM resource binding and coordinate normalization | check each ledger row |
| src/AnmManager.cpp | VM allocation, lists, update and draw grouping | check each ledger row |

The two world-matrix paths are deliberately separate in
AnmVm::UpdateWorldMatrix:
- applyAlignment == true: anchor-adjusted path associated with target
  FUN_00460DD0 (0x00460DD0).
- applyAlignment == false: copy cached matrix then add VM translation,
  associated with target FUN_004609C0 (0x004609C0).

These are reverse-engineering hypotheses implemented in C++; neither path
has an accepted exact owner replay yet.

### Exact ANM owners

The current config/matches.csv contains 16 exact ANM owners:

- Setters: 0x00402B60, 0x00402C00, 0x00402C20, 0x00407020,
  0x00407040
- AnmVm::AnmVm: 0x004031A0
- Effect no-ops: 0x00412B30, 0x00412B40, 0x00412DF0, 0x00412E00
- StartAnmVmEffect06: 0x00412C80
- Operand resolvers: 0x00459900, 0x00459B50, 0x00459C30, 0x00459CE0
- NormalizeAnmCoordinates: 0x00459D50 (52 bytes; Update LTCG caller context)

Do not expand this list from source presence or successful compilation. Only
accepted rows in config/matches.csv count as exact.

## Natural-source cleanup decision

The target helper ZunVec3::SetPolarXY at 0x00432E20 uses x87 FSINCOS. Its
former inline-assembly implementation has been replaced with natural C++
cos/sin calls, and its exact ledger row was withdrawn. This deliberately
reduces the exact count; do not restore inline assembly to recover that count.
The target codegen mismatch is an honest frontier. The four laser slot-0
callers remain frontiers as well.

## Next useful work

1. Continue the full AnmVm::Update reconstruction from 0x00459D90. Recover
   the missing branches rather than polishing only small helpers. Follow
   incoming references and high-fan-in helpers where they clarify the boss
   function's behavior.
2. Audit ExecuteBasicInstruction/ExecuteReadyInstructions against the
   target: opcode payload offsets, operand flags, frame gates, wait/restart
   behavior, instruction pointer updates and termination.
3. Audit transition setup and UpdateTransitions mode-by-mode, including
   timer edge cases and vector/scalar differences.
4. Continue renderer dispatch and world/fog/primitive modes, keeping
   0x004609C0 and 0x00460DD0 semantics distinct.
5. Compile only changed/affected translation units while iterating. When an
   already-exact unit is changed, replay that unit and directly affected
   dependencies only.

## Workflow and constraints

- Natural C/C++ only. No inline ASM, target-byte injection, inert padding,
  fake returns or compiler-profile tricks to disguise incomplete source.
- Do not run a full cold replay for routine progress. Use per-file compile
  probes during exploration; use the canonical Oracle only for the affected
  exact owner(s) and required dependencies.
- Make substantive local checkpoints with the required format:
  gpt-web: <English summary>.
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
