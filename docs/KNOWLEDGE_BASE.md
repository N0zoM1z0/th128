# TH128 working knowledge

Verified findings, current blockers and failed approaches worth avoiding.
Facts here are TH128-local unless marked as a hypothesis.

## Target

- `resources/th128.exe` is the original Japanese v1.00a: 752,640 bytes,
  SHA-256 `ba515750…767d4`, PE timestamp 2010-08-03T04:56:54Z, image base
  `0x00400000`, entry `0x0047460E`, relocations stripped, four sections
  (`.text`, `.rdata`, `.data`, `.rsrc`). The pinned thcrap database lists this
  digest as `th128 v1.00a (original)`.
- The same installation's `th128_CN.exe` (SHA-256 `f51af65e…`) is the
  Chinese patch and `custom.exe` is the separate configuration tool
  (`th128_custom`). Neither is a target.
- Imports: `d3d9`, `d3dx9_42` (17 functions), `DINPUT8`, `DSOUND`, `WINMM`,
  `KERNEL32`, `USER32`, `GDI32`, `ole32`. There is no CRT DLL import, so the
  CRT is linked statically (`/MT`, `libcmt.lib`). `d3dx9_42` is the D3DX runtime
  of the August 2009 DirectX SDK; the exact SDK import libraries are open
  until product closure.

## Toolchain

- PE linker 10.0. Decoded Rich records (product id / build / count):

  | Product | Build | Count | Reading |
  | --- | --- | --- | --- |
  | 170 | 30319 | 134 | VC10 C (largely static CRT) |
  | 171 | 30319 | 48 | VC10 normal C++ |
  | 175 | 30319 | 56 | VC10 LTCG C++ (`/GL`) |
  | 158 | 30319 | 43 | VC10 MASM (CRT assembly) |
  | 154 | 30319 | 1 | VC10 cvtres |
  | 157 | 30319 | 1 | VC10 linker |
  | 1 | 0 | 181 | imported symbols |
  | 147 | 30729 | 12 | VC9 SP1 import-library records |
  | 131 | 30729 | 4 | VC9 SP1 C objects from a linked library |
  | 110, 123 | 50727 | 1, 2 | VC8 library records |
  | 93, 95 | 4035 | 5, 2 | older library records |
  | 152 | 20115 | 1 | unidentified |

  The product-id readings are the conventional mapping and are a hypothesis;
  the build numbers are direct observations. Which library supplies the
  30729/50727/4035 records (most likely the DirectX SDK) is open.
- `archaic-msvc/msvc1000` at `f8977e2c` reports cl `16.00.30319.01`, link
  `10.00.30319.01`, cvtres `10.00.30319.01` and RC `6.1.7600.16385`, matching
  the build-30319 records. SP1 (build 40219) does not match.
- VC10 `/GL` objects start with `00 00 FF FF 01 00 4C 01`; normal objects
  start with the i386 COFF machine `4C 01`.
- **cvtres needs `MSVCR100_CLR0400.dll`.** The payload omits it and the linker
  reports `LNK1123: failure during conversion to COFF` for every `.res` input.
  Copying the prefix's own `msvcr100.dll` to that name fixes it;
  `verify-toolchain.py --execute` does this inside the TH128 prefix.
- The full headless smoke (normal C and C++, `/GL` C++, RC, cvtres, `/LTCG`
  PE link with MAP and PDB, Capstone-decoded linked extent) passes on Wine 9.0
  with the TH11 Oracle scripts ported to VC10.

## AnmVm effect callbacks

A function table in `.data` around `0x004B1E9C` holds AnmVm effect start
and update callbacks with fixed `__fastcall (AnmVm *vm, const float *position)`
signatures, so LTCG cannot change them. `StartAnmVmEffect06` (`0x00412C80`)
is exact as normal COFF. It allocates 0x2C bytes into `vm->extraData`
(+0x4AC, which `~AnmVm` frees) and copies the screen position into
`vm->position` (+0x464) by **vector struct assignment**: per-component float
copies compile to `FLD`/`FSTP`, the original's integer `MOV`s need a struct
copy. `ZunTimer::Initialize` and an inline `Reset()` now live in the header
for such inline timer resets.

## Laser classes

`LaserDataInf` (vtable `0x004A2BB4`) is the base of `LaserLineInf`
(`0x004A2C1C`), `LaserCurveInf` (`0x004A2C84`), `LaserInfiniteInf`
(`0x004A2CEC`) and `LaserBeamInf` (`0x004A2D54`). It has 24 virtual slots
and no virtual destructor. Its defaults are exact as normal COFF; their
`RET n` sizes fix every slot's stack argument count (slot 8 takes five
arguments, slot 9 four, slot 10 six, slot 11 four). Slot 6 unlinks the laser
from a doubly linked list (prev +4, next +8). The defaults are laid out in a
different order from the slots (9, 10, 8, 11, 7 after slot 6), so the
original defines them in another order. Derived kinds override most slots with 60–2,400-byte methods; slot 0 of each
kind computes a point along the laser through a polar helper at 0x00432E20.
The target helper uses x87 FSINCOS, which VC10 does not naturally emit from
C. A previous source version used inline assembly to reproduce the 32-byte
target body, but that violated the project's natural C/C++ requirement. The
inline assembly has been removed: ZunVec3::SetPolarXY now uses ordinary C++
cos/sin calls. Its exact ledger claim was withdrawn and the owner is again a
codegen frontier. Do not reintroduce inline assembly just to recover the old
match percentage. The four slot-0 methods remain frontiers too; the original
loads the point before adding position, while the current compiler variants
load position first.

`LaserInfiniteInf::Slot01` (`0x0042EEB0`) runs the laser's timed commands:
an 18-entry table of 0x2C-byte records at +0x61C indexed by +0x594 (type
+0x20, value +0x10, an `immediate` word +0x24 that bypasses the busy flag at
+0x598). The source reproduces every operation and the layout, but VC10
keeps the index in `ESI` across the mode-bitfield store, while the original
re-reads it (`INC [ecx+0x594]` in the bitfield arms, loop test from memory).
Per-case increments and `continue` arms compile to the same code as one
increment after the switch.

## Script resource registry

`SptResourceInf` (vtable `0x004A26DC`) is the base of `EclResourceInf`
(`0x004A2720`): slot 0 `Load` (`0x004703F0`, 538 bytes) is shared, slot 1
`LoadIncludes` is overridden. `Load` validates a SCPT (`0x54504353`)
version-1 file, records it in `files[32]` (+0x0C), and merges its subs into
a name-sorted `{name, code}` table at +0x8C (realloc by `malloc`/`memcpy`/
`free`, insertion with inline `strcmp`). The source reproduces its size,
flow and block layout; two separate `if` checks are needed for the failure
block to sit inline after the magic test. Register assignment still differs.

## Window procedure

`WindowProc` (`0x00454420`, 420 bytes) is exact as normal COFF. Fixed
Windows callbacks are good large targets because LTCG cannot change their
convention. Two source shapes mattered: `WM_SIZE` tests `wParam` with a
nested `switch` (`MOV EAX, EDI; SUB EAX, 2; JNE`), not `==`, and the
`WM_SETCURSOR` branch tests `!g_CursorAlwaysVisible` first. Globals touched:
`0x004D33F8` (game flags: bit 7 quit), `0x004D59A8` (window flags),
`0x004D397C`/`0x004D3980` (activation), `0x004D2F7C` (cursor always shown);
names are provisional.

`CreateGameWindow` (`0x00455320`, 566 bytes) reproduces every instruction
when `WNDCLASSA` is cleared with `memset` (a `{0}` initializer schedules
`XOR ESI, ESI` before `PUSH 4`). Two differences remain: the original frame
is `0x30` with the class at `[ebp-0x2C]` (ours `0x28`/`[ebp-0x28]`, so two
4-byte slots are unaccounted for) and `EBX`/`EDI` hold the instance and the
frame-size constants the other way round. Declaring `width`/`height`/
`window` at the top in any order did not change either. The window mode is
a 2-bit field at bits 2–3 of `0x004D59A8`; mode 0 is the 640×480
`WS_POPUP` fullscreen window, modes 1–3 are framed 640/960/1280 windows.

## Archive reader chain

`ReadHeader` (`0x0044F9A0`), `ReadEntry` (`0x0044F7D0`) and `DecryptBlocks`
(`0x00469A00`) are now real source in one archive link context, so their
LTCG conventions come from real callers. Open, Close, `~Archive`, the entry
constructor/destructor and the directory parser stay exact there. The LZSS
decoder (`src/Lzss.cpp`) is real too, so the probe no longer holds any
placeholder code; it only adds caller shapes from the replay and score
loaders and the cipher key table.

- **LZSS convention.** The original decoder is a plain `__stdcall` with all
  four arguments on the stack. In every context tried, VC10 `/GL` instead
  moves `size` into `EAX` (`RET 0xC`, exactly TH11's observed convention)
  unless the function's address escapes; with `g_ProbeDecoder =
  DecompressLzss` it keeps the original convention. Whether the original
  really takes the address somewhere is unverified. `int cached` (not
  `unsigned char`), the `>= packedSize` fetch test and a nested allocation
  check are required; what remains is stack-slot assignment for the history
  position and the 13/4-bit accumulators (435 vs 443 bytes).

- **Cipher convention.** VC10 passes the key in `AL` like the original only
  when `size` is modified in place (`size -= (size & 1) + tail;` and the
  loop counts `size` down). With a separate `remaining` variable LTCG also
  moved `size` into `ECX`; putting the key first moved `size` into `EAX`.
  The remaining difference is stack-slot assignment: the original keeps
  `length` in `[ebp-4]` and the tail in the `size` slot (54 differing
  instructions, 273 vs 274 bytes).
- **ReadHeader.** Header magic `THA1`; the three header words are unmasked
  by `-123456789`, `-987654321` and `-135792468`. Every call site matches,
  including both cipher calls and the LZSS call; VC10 still prefers the
  listing offset over the packed size for `EDI`/`EBX` (396 vs 384 bytes).
  Top-of-function C89 declarations and `int` types did not change that.
- **ReadEntry.** `this` is passed on the stack (`RET 0xC`), as in the
  original. The cipher parameters are `g_ArchiveCipherKeys[sum % 8]` at
  `0x004B1200` (12-byte records: key, step, block, limit), where `sum` is the
  byte sum of the entry name. The original lays the found block directly
  after the search loop; `continue`/nested-`if` forms both put it after the
  error path, and an indexed `entries[i]` loop changed the convention.

## Current AnmVm reconstruction status (2026-11-24)

The recent local work has concentrated on the large AnmVm update, operand
resolution and rendering subsystem. Treat implementations without accepted
rows in the canonical ledger as source frontiers, not as exact codegen or
runtime proof. The canonical ledger currently has 95 exact functions overall;
16 exact owners are in the ANM area:

- Setters: 0x00402B60, 0x00402C00, 0x00402C20, 0x00407020, 0x00407040
- AnmVm::AnmVm at 0x004031A0
- Effect table no-ops: 0x00412B30, 0x00412B40, 0x00412DF0, 0x00412E00
- StartAnmVmEffect06 at 0x00412C80
- Operand resolvers: 0x00459900 (588 bytes), 0x00459B50 (211 bytes),
  0x00459C30 (162 bytes), 0x00459CE0 (104 bytes)
- NormalizeAnmCoordinates at 0x00459D50 (52 bytes), recovered with its
  real AnmVm::Update caller in the LTCG context.

All other ANM source mappings are not exact unless a matching row is added to
config/matches.csv.

### Main update owner

AnmVm::Update starts at 0x00459D90 and owns 13,604 target bytes
(through 0x0045D2B3). It has a source mapping to src/AnmVmScript.cpp,
but no accepted exact replay. The current C++ covers the early exits,
game-speed save/restore, script update gate, continuous angle/scale/texture
motion, view texture-rectangle refresh, render-state synchronization,
transitions, custom geometry, callback and timer advancement. It is still a
partial reconstruction of the large target function; branch ordering,
missing paths and side effects require further target comparison. The latest
selected VC10 `/GL` candidate owns 13,764 bytes versus the 13,604-byte target
contribution (160 bytes larger), with 284 direct calls versus the target's
286. This is a useful frontier measurement, not an exact replay.

Opcode 40 now selects the UI/world RNG in separate branches and keeps integer
operand resolution inside each branch, matching the target's two resolver
call sites. Opcode 41 uses the exact `Rng::GetFloat(float scale)` leaf at
`0x0040D130` with the same UI/world selection. Opcodes 17 and 27 share the
same float-result store as opcodes 42..46, reproducing the target's single
destination-resolution block; opcode 47 uses `AddAngles(angle, 0)` as the
target does. The opcode-113 view transition preserves the target's three
`NormalizeAngle` calls, including the normalized zero terms, and the mode-13
and mode-14 geometry paths retain their separate normalization call sites.
The Update transition-reset call at `0x00406F80` is not a direct
`ZunTimer::Reset`: it receives the transition/view owner and resets the timer
embedded at owner `+0x30`. `ResetTransitionTimer(void *)` now reproduces that
52-byte helper exactly, including lazy initialization, the `g_TimeScale`
pointer relocation, and the final `previous = -1` store. Opcodes 56, 100 and
59 share this owner-relative helper; the standalone timer reset remains a
distinct operation. Texture-offset opcodes 107 and 108 use a second exact
52-byte helper, `ResetScalarTransitionTimer(void *)` at `0x0040CEA0`, which
resets the timer at owner `+0x10`; the target and candidate both retain its two
call edges for the X/Y scalar transitions. Opcode 113 has a third, byte-identical
52-byte owner-relative helper copy, `ResetViewTransitionTimer(void *)` at
`0x0045DAD0`, also using owner `+0x10`; it remains a separate function and
preserves the view-transition call edge. The four Update-owned timer-frame
writes also use a separate exact 61-byte `SetUpdateTimerCurrent` copy at
`0x00403280`, distinct from `ZunTimer::SetCurrent` at `0x00403240`; all four
call edges now match the target. The VM-id lookup at `0x00467420` now has an
exact 89-byte natural-C++ replay: `GetVmByIdTargetAbi` keeps the manager in
EDX, the ID on the stack, and the unused ECX fastcall slot through an
address-taken probe reference. It searches world then UI VM lists. The
`0x00467780` `ResolveAnmVmId` wrapper now calls this exact helper and keeps
both Update call edges; its current candidate is 23 bytes versus the 21-byte
target because LTCG emits a zeroing instruction for the unused ECX slot.
Opcode 45 is `acos`, confirmed by the target's `__CIacos` call; the source now
uses `acos` rather than `asin`. The latest targeted `AnmVm::Update` linked
build is 13,628 bytes against the 13,604-byte target (3,721 vs 3,769 decoded
instructions; 286 vs 289 calls, with 283 vs 286 direct). Opcode 40 now keeps
both target `Rng::GetU32` callsites, and opcode 102 keeps both target
`BindAnmFile` callsites; each count is exact. The remaining three direct-call
difference is the target's five `SpawnChildVm` callsites versus two candidate
calls: LTCG folds the four list-mode-only opcodes (88, 91, 90 and 92) into one
shared natural-C++ call. `SpawnChildVmAtPosition` remains two calls on both
sides. The candidate also has one fewer `ResolveFloatOperandAddress` call and
one extra `ResolveIntegerOperandAddress` call. Its random-float call resolves
to the exact 52-byte `Rng::GetFloat(float scale)` leaf at `0x0040D130` (the
target inventory names this overload `Rng::GetFloatScaled`), so that call is
not an outstanding mismatch. `Rng::GetU32` is also an exact 149-byte natural-C++
leaf with all eight reviewed relocation fields matching; its canonical unit
is `rng-get-u32`. The `SpawnChildVm` helper itself is currently
252 bytes versus the 263-byte target; its EAX resource/stack-owner ABI remains
a separate natural-C++ frontier.
The view-corner helpers now preserve the target's anchor-3 no-write behavior
for modes 0 and 2/3. Their parent translations also retain per-corner x87
operand order and intermediate float stores; the non-rotated paths perform
the target's transient parent-Z additions before restoring the child center Z.
LTCG measurement frontiers remain larger in the target because its helper
call sites use a private register convention that the natural C++ probe does
not reproduce: mode 0 is 834/1209 bytes, mode 2/3 is 799/1020 bytes, and mode
1 is 537/623 bytes. These are measurements, not exact replays.
The effect-10 geometry path now keeps the signed particle count and the
mode-specific evaluation order. Mode 9 loads the raw start angle before the
count-minus-one conversion, computes the double-constant 2π step, initializes
V, then computes the integer V step. Modes 13/14 normalize their initial angle
before the count-minus-one conversion and compute their angle/V steps before
center accumulation; mode 14 then replaces the angle after local-center
composition but before parent-center composition. The ring and strip loops
advance a natural vertex cursor; the ring closes by copying the first pair to
the cursor left by the loop. Zero and negative counts still execute the
target's divisor operations before the loop test. No assembly or padding is
used. The Update-specific
`SetPolarForUpdate` leaf at `0x0045E010` remains a natural-C++ codegen frontier:
the target uses x87 `FSINCOS`, while the source uses ordinary `cos`/`sin` calls.
Do not introduce inline assembly to force that leaf. The four operand resolver
call counts now match the target: 93 float source,
82 integer source, 12 float destination and 13 integer destination calls.
The per-frame `stateFlags & 0x20` view refresh performs coordinate normalization
inline; only opcode 98 calls the separate `NormalizeAnmCoordinates` helper
four times. The boss remains unclassified until its complete owner and
linkage set replay with zero differences.

The color-vector cubic path is tracked separately by
`anm-vm-interpolate-color-vector-linked`. Its mode-8 source now rounds the
normalized time through a float and materializes the twelve individually
converted Hermite terms in basis-major order (h11, h10, h01, h00), matching
the target's conversion call order. The selected owner is currently 719 bytes versus the 721-byte target; its
remaining differences include x87 coefficient scheduling, stack layout,
register allocation and the target's internal register ABI. This is source
progress, not an exact helper.

The easing dispatcher `EaseTransition` at `0x0046B190` is tracked separately
by `anm-vm-transition-easing-linked`. Its LTCG candidate owns the complete
728-byte contribution and all 35 absolute fields (the jump-table pointer,
16 handler entries and floating-point constants) are mapped. The natural C++
mutation order for cubic and quartic easing now reproduces the target x87
operand forms. The selected linked-function replay reports `exact` with zero
normalized byte differences. The normal-COFF view has a different extent and
calling convention; the linked caller context is the relevant replay.

Useful evidence:
- .analysis/anm-vm-update-target.txt — target Ghidra decompilation
- .analysis/anm-vm-update-full.asm and
  .analysis/anm-vm-update-boss.asm — target instruction evidence
- .analysis/anm-vm-update-helpers.txt and
  .analysis/anm-vm-update-leaves.txt — related helper decompilations

### Operand resolution

All four `AnmVm::Resolve*Operand*` owners now have complete pinned VC10
`/GL` linked-PE replays. `ResolveFloatOperand` uses the target conversion
helper for the encoded float operand and calls the scaled signed-random leaf
for operand 10010. The integer resolver uses the same runtime conversion
helper for float-backed integer slots and selects the VM's world/UI RNG for
operand 10022. The two address resolvers preserve their target pointer-return
semantics and local switch tables.

`ResolveIntegerOperand` owns a 211-byte contribution containing a 12-entry
DIR32 jump table at +0x8C and a 23-byte byte dispatch table at +0xBC. The
latter is explicitly declared as an inline data span in the linked Oracle;
those bytes are excluded from instruction decoding but remain in the complete
owner and are compared raw. All 19 relocation fields and all remaining bytes
replay with zero differences. This is a compiler-generated table in natural
C++ output, not embedded assembly or padding.

### Coordinate normalization helper

`NormalizeAnmCoordinates` at `0x00459D50` is now exact as a 52-byte
linked-PE owner. Its standalone normal-COFF ABI did not match the target, so
the accepted replay includes the real `AnmVm::Update` caller in the VC10 LTCG
context. The target uses ECX for the output pointer and EAX for the input
pointer; the two DIR32 constants resolve to the 640- and 480-pixel scale
constants at `0x004A65C8` and `0x004A65C0`. Both relocation fields and all
remaining bytes replay with zero differences. The function divides X by 640
and Y by 480, then clamps negative results to zero without an upper clamp.

### Related source files

- src/AnmVmScript.cpp: wait handling, a large opcode switch, instruction
  advancement, view-corner construction and texture-rectangle updates. Check
  opcode operands, frame gates, script-address advancement and side effects
  against the target; presence of a case is not proof of parity.
- src/AnmVmTransitions.cpp: transition setup and interpolation for
  position, scale, colors, effect state and scalar/vector modes. Modes 7/17
  were corrected to use raw additive accumulation where the target does.
  This file has not been accepted as an exact owner.
- src/AnmVmEffects.cpp: render-state synchronization, world-matrix
  generation, sprite batching, projected/world fog, primitive modes and
  effect 10. UpdateWorldMatrix(..., true) models the anchor-adjusted
  path at 0x00460DD0; false models the matrix-copy/translation-add
  path at 0x004609C0. Keep those paths distinct and verify both before
  making exactness claims.
- src/AnmVmOperands.cpp: the four operand resolver owners above are exact;
  keep any adjacent helper or future edits separate and replay affected owners.
- src/AnmVmResource.cpp: `NormalizeAnmCoordinates` at 0x00459D50 is exact
  only in the recorded `AnmVm::Update` LTCG caller context; adjacent resource
  helpers remain separate frontiers.
- src/AnmManager.cpp contains manager support. Check its ledger rows individually.

The renderer's draw dispatcher is separate from the per-VM update routine.
Do not infer that an implemented render mode, a successful per-file compile,
or a source mapping means the full VM owner is exact. Use the canonical
ledger and accepted replay receipts as the authority.

## Origin review

All 1,378 candidates left after the archive scan were reviewed from TH128
bytes, call graph, globals, strings and vtables (`.analysis/origin-review/`
holds the private evidence). Results:

| Evidence id | Rows | Reading |
| --- | ---: | --- |
| `game-band-origin-sweep` | 1,177 | Authored game code. High with game callers/callees, globals, strings or a game vtable slot; medium for unreferenced copies |
| `crt-tail-archive-multihit` | 63 | CRT code whose masked bytes match several archive placements or have too few fixed bytes for the strict scan |
| `vc10-eh-funclet-review` | 57 | Unwind funclets after the CRT |
| `vc10-stl-header-instantiation` | 16 | `std::string` and allocator code from VC10 headers (`library`) |
| `vc10-special-member-review` | 14 | Scalar deleting destructors (`compiler_generated`) |
| `vc10-math-wrapper-review` | 12 | Out-of-line float `math.h` wrappers (`cosf`, `sinf`, `sqrtf`, ...) |
| `crt-tail-archive-extent-review` | 3 | Archive matches whose Ghidra extent overruns (`_floor`, `__CIatan2`, inside `__cos_default`) |
| `vc10-import-thunk-review`, `vc10-dynamic-init-tail-review` | 2 | `RtlUnwind` stub; one atexit-registering initializer |
| `d3dx-header-candidate-review` | 21 | **Open:** bodies identical to `d3dx9math.inl` vector helpers; a game vector type would compile the same |
| `ltcg-entry-adapter-review` | 12 | **Open:** 10-byte `PUSH r; MOV r, ECX; CALL body; RET` callback entries; authored static callbacks or linker-made adapters |
| `boundary-review-new-function` | 16 | **Open:** later-added candidate rows whose origin has not yet been settled |

Six tiny game vtable slots (`0x004144A0`, `0x004144B0`, `0x004144D0`,
`0x00414680`, `0x00429A00`, `0x00450FD0`) also match tiny library bodies by
chance and stay authored. The historical origin sweep left 34 rows open at that time. The current
canonical origin ledger has 49 pending rows: 21 D3DX candidates, 12 adapter
candidates and 16 later-added boundary-review candidates. Use
config/function-origins.csv as the current authority; the private
.analysis/origin-review/NOTES.md records the earlier snapshot.

## Pbg archive directory

The THA1 directory is a `new[]` array of 16-byte entries (name, offset,
size, reserved). A cold `/GL` probe with `new Entry[n]`/`delete[]`
reproduces the out-of-line entry constructor (`0x0044FD20`, `mov eax, ecx;
mov [eax], 0; ret`) and destructor (`0x0044FCB0`) byte for byte, because the
vector iterators take their addresses; both are exact.

The same probe explains the 10-byte `PUSH r; MOV r, ECX; CALL body; RET`
entries the origin review left open: `0x0044F5E0` is a `thiscall`
destructor whose whole body is a call to the archive `Close` (`0x0044F730`,
private `ESI` receiver), with its address passed to an array destructor
iterator. These look like authored one-line wrappers around private-
convention bodies rather than linker artifacts; `~Archive` reproduces
exactly that way.

`Archive::Close` (`0x0044F730`) is exact with two source facts worth reusing:

- **Include `<new>`.** Without a declaration of `operator delete[]`, VC10
  lowers `delete[]` to scalar `operator delete` (`??3`); the original calls
  `??_V` (`0x00473A61`). The same applies to `new[]` (`??_U`, `0x004725A4`).
- **The release logger is not LTCG.** `DebugPrint` (`0x0044FD30`) is an empty
  varargs function; under `/GL` the compiler drops the constant format
  argument at call sites, while the original still pushes it. Building the
  logger as normal COFF reproduces the calls.
- `Archive::Open` (`0x0044F5F0`) is exact with `this` in `EAX` and the name
  in `EBX`, chosen by LTCG. It depends on its callee `ReadHeader` having the
  original's private `ESI` receiver and stack filename, which the real
  reader now provides; early placeholder readers moved the call to
  `EDX`/stack-`this` and even changed Open's own convention.
- `ParseArchiveDirectory` (`0x0044FB20`) is a `__stdcall` free function with
  a C++ EH frame (`/EHsc`). Its `__ehhandler$` DIR32 binds through the MAP
  `Static symbols` section with `eh_static = true`; the target handler is
  `0x0049910B` and its unwind funclet `0x00499100` frees the half-built
  array with `operator delete[]`.
- The original source file is `src\\pack\\_ArcMngr.cpp` (a trace string at
  `0x004A472C` names line 113, `ResFile`).
- Null stores follow the original statement order: `name` is cleared both
  inside and after its free, `entries = NULL` after the `delete[]` block, and
  `file = NULL; count = 0;` last.

## Boundary review and inventory closure

A second pass reviewed every remaining boundary and every uninventoried
`.text` span from TH128 bytes (Capstone recursive descent plus raw scans of
every call, jump and dword for references; private evidence in
`.analysis/boundary-review/`). The inventory grew from 2,221 to 3,516 rows:

- **Existing rows.** 1,409 reviewed as exact extents (`bdesc-exact`, or
  `bdesc-exact-weak` when unreferenced); 116 extents corrected
  (`bdesc-extent-correction`), mostly trailing jump/index tables, plus four
  missing code blocks (`0x0046B190` +718 bytes, `0x00424B80` +1,057 bytes,
  `0x004653F0`, `0x00453050`) and three CRT shrinks (`_floor` at
  `0x0048FDF0` now ends at `0x0048FF10`); 40 rows that start inside another
  function (switch arms, SEH blocks, catch blocks) were removed; the rest
  stay provisional (`bdesc-open`, all CRT).
- **New rows.** 1,335 functions: 1,038 in the game band (113 referenced,
  mostly callbacks passed as address immediates; 925 unreferenced but cleanly
  delimited dead copies), 182 import stubs, and CRT/EH-tail code. A raw scan
  found only chance hits for the unreferenced ones.
- **Origin of new rows.** The library scan placed 51 more rows in libcmt/
  libcpmt; game-band rows are `authored_game` (medium,
  `game-band-origin-sweep-new-rows`); 50 rows after `0x00499000` are EH
  handler stubs, dynamic initializers and atexit destructors
  (`compiler_generated`, `vc10-eh-tail-new-rows`); import stubs are
  `import_thunk`. 16 CRT-band rows remain open.
- Every game-band byte is now accounted for: the leftover gap spans are
  switch arms, alignment fillers, jump tables, the DirectInput data block and
  padding.

## Ported infrastructure

The Oracle, receipt, LTCG-link and Ghidra scripts come from the TH11
reconstruction (VC9 RTM, also LTCG-heavy) with VC10 paths and names. The
synthetic Oracle suites (8 normal-COFF and 27 linked-PE cases) pass
unchanged. The TH11 EH-static negative test was dropped because it is bound to
real TH11 units; recreate an equivalent once TH128 has an owner with a
compiler-generated EH handler.

## First exact unit

`SetTimeScale` (`0x00407310`, 16 bytes) stores its float argument in the
global at `0x004B6F58`, which about 40 sites read; the name is provisional
and no direct caller reference was found. A cold `/TP /O2 /Oy- /Gy /MT /GL`
linked context reproduces it with its single DIR32 field and zero byte
differences. Without `/Oy-` VC10 omits the `PUSH EBP; MOV EBP, ESP` frame and
the contribution no longer has the target size, so the Oracle rejects it.
Most small game leaves near it keep the frame pointer too, which makes `/Oy-`
the working default hypothesis for game code.

## LTCG private conventions

`ZunTimerModulo` (`0x00402B20`, 16 bytes) returns the count at +4 modulo a
stack divisor, with the object in `EAX` and `RET 4`. Declared plain
(`__cdecl`), VC10 `/GL` already moved the object into `EAX` but left the
divisor caller-popped (`RET`, `ADD ESP, 8`). Declared `__stdcall`, the same
link moves the first pointer argument into `EAX` and keeps callee cleanup,
reproducing the target exactly. The probe in `probes/zun-timer-context.cpp`
calls it twice with independent inputs so the convention is not anchored to
one caller. Expect the same `__stdcall` reading for the many small
`EAX`-receiver `RET n` leaves. The five `ECX`-receiver timer comparisons (`<` `0x00402C40`, `<=`
`0x00407130`, `>=` `0x004073B0`, `==` `0x00407E70`, `>` `0x00410CD0`) are
ordinary out-of-line `bool` `__thiscall` members: VC10 keeps `ECX`, emits
`XOR EAX, EAX; CMP; SETcc AL` and `RET 4`. They are separate functions rather
than one folded body, consistent with distinct operators.

The AnmVm float-pair setters (`0x00402B60`, `0x00402C00`, `0x00402C20`,
`0x00407020`, `0x00407040`) follow the same `__stdcall` + `EAX` rule. Source
order `flags |= 8` before the stores reproduces VC10's schedule, which loads
the first float, ORs the dirty bit at +0x4B0, then stores.

The ZunTimer field names beyond +4 come from adjacent games and remain a
hypothesis.

## Angle helpers

`NormalizeAngle` (`0x0046A700`, 133 call sites) and `AddAngles`
(`0x0046A660`) are exact as normal COFF `__stdcall` functions with TH11's
source shape: float-rounded pi and two pi stored as doubles (`0x004A6580`,
`0x004A6578`), `-pi` folded to `0x004A65B8`, and one 33-correction budget
shared by both loops. The lower bound is the float-rounded negative-pi literal
widened to double, not the exact negation of `kPi`; `AddAngles` keeps its
initial sum in the x87 register and rounds through the float slot only when a
wrap adjustment is needed. Both owners replay with zero byte differences.

`ZunTimer::Increment` (`0x00402AC0`, 39 call sites) is exact as an 88-byte
out-of-line linked-PE member. It uses three DIR32 constant relocations and one
REL32 `__ftol2_sse` call. Inside the speed window it does `current++;
currentF += 1.0;`; otherwise `currentF += *gameSpeed; current = (int)currentF;`.
The body was moved from the header-inline definition to `src/ZunTimer.cpp`
with `__declspec(noinline)` so the real transition helpers retain their
original call boundary.

`ZunTimer::Add` (`0x0046AA20`) now replays as an exact 97-byte LTCG
contribution. Its four reviewed DIR32/REL32 fields bind the two floating-point
constants and both `_ftol2_sse` calls; the complete PDB-owned function has
zero byte differences. The natural C++ implementation preserves the
target's game-speed tolerance: use an unscaled increment inside `(0.99, 1.01)`,
otherwise scale by `*gameSpeed`; both branches store the accumulated
`currentF` before converting it to `current`. The explicit volatile access
also preserves the aliasing case where `gameSpeed` points at another timer's
`currentF`. Canonical unit `zun-timer-add`.

`MenuCursor::Move` (`0x0046A970`, 99 call sites) is exact on the first
try as a plain member: LTCG gives it the original's `EAX` receiver from a
three-call probe. The retry when a disabled entry is hit is a `goto` back to
the `value += delta` step, which reproduces the original loop shape.

## Random number generator and global aliasing

`Rng` (`src/Rng.cpp`) is exact for `GetU16` (`0x0046A3F0`), `GetU32`
(`0x0046A460`, 44 call sites) and the three float forms `GetFloat` [0, 1),
`GetFloatSigned` [-1, 1) and `GetAngle` [-pi, pi) (`0x0046A5D0..0x0046A65E`).
Every method takes an optional critical section when bit 15 of the game
flags (`0x004D33F8`) is set (lock `0x004D3768`, depth byte `0x004D37A2`).

- **Escaping globals change LTCG scheduling.** In a small probe VC10 LTCG
  proves that `g_GameFlags` is never address-taken, so it moves the flag
  reload ahead of the seed store and the counter update ahead of the seed
  read. The original whole program does take that address. Adding
  `unsigned int *g_ProbeFlagsAddress = &g_GameFlags;` to the probe restores
  the original order and both methods become exact. Expect the same effect
  wherever a frontier differs only by a store/load order or a reload.
- The float forms convert with `(float)GetU32()` first (float `2^32` bias
  constant), then scale by double constants (`2^-32`, `2^-31` then `- 1.0`,
  `/ 683565248.0` then `- pi`).
- The scaled overload `Rng::GetFloatSigned(float scale)` at `0x00412E90`
  is exact as a 58-byte normal-COFF leaf with four reviewed relocations. It
  rounds the signed unit-range intermediate to float before multiplying by
  `scale`; `AnmVm::ResolveFloatOperand` uses it for the random-angle slot.
- The unsigned scaled overload `Rng::GetFloat(float scale)` at `0x0040D130`
  is exact as a 52-byte linked-PE leaf with three reviewed relocations. It
  converts `GetU32()` to the unsigned unit interval, rounds that intermediate
  to float, then multiplies by `scale`; `AnmVm::Update` opcode 41 calls it on
  the VM-selected world/UI RNG.

`AnmManager::GetVmById` (`0x00467420`, 277 call sites) searches two VM
lists (`+0x8B9704`, `+0x8B970C`, nodes `{vm, next}`) for `vm->id` (AnmVm
+0). It is a frontier on convention alone. With only global-pointer callers
LTCG folds `this` into a load of `g_AnmManager` inside the callee; one call
through another manager pointer restores the original `EDX` receiver, but
VC10 then also passes the id in `ECX` while the original pushes it. Calls
with field-loaded ids did not change that.

## Timer game speed

`0x004B6F58` is the game-speed float that every `ZunTimer` points at. The
inline timer initialization in the Ascii append routine (`0x004015F0`) sets
`previous = -999999` (`0xFFF0BDC1`), `current = 0`, `currentF = 0.0`,
`gameSpeed = &g_TimeScale` and the init bit, then resets to
`previous = -1`. This confirms the `ZunTimer` layout (+0 previous, +4 current,
+8 currentF, +0xC gameSpeed, +0x10 flags) and the `SetTimeScale` reading.

Out-of-line `ZunTimer::SetCurrent(int)` (`0x00403240`) and
`SetCurrentF(float)` (`0x00432C40`) are ordinary members: VC10 `/GL` moves
`this` to `EAX` and `ESI` respectively by itself. The lazy initializer must
store the init bit last, and `SetCurrentF` converts before storing
`currentF`. Byte-identical copies at `0x00403280` and `0x00432C80` were not
folded, so they are distinct functions (other names or types).

## AsciiInf strings

`AsciiInf` (0x1C8A8 bytes) keeps 0x164-byte strings from +0x9E4: text at +0,
an id at +0x100, position +0x104, colors +0x110..+0x118, scale +0x11C/+0x120,
a `ZunTimer` at +0x150. The append routine refuses a new string when the
count at +0x1C848 reaches 0x140, but the constructor's timer loop runs 0x141
times (last bit at +0x1C884), which no single 0x140-element array explains.
The per-call settings it copies live at +0x1C84C..+0x1C884.

## AnmVm construction

`AnmVm` is 0x4E8 bytes. Its constructor (`0x004031A0`) is the plain C++
shape `memset(this, 0, sizeof(AnmVm)); anmFileIndex = -1;` after member
construction: thirteen embedded `ZunTimer`s at +0x70, +0xD4, +0x120, +0x14C,
+0x198, +0x1C4, +0x200, +0x23C, +0x288, +0x2B4, +0x2E0, +0x30C and +0x3FC
each clear bit 0 of their +0x10 flag word (`ZunTimer() { initialized = 0; }`),
which VC10 emits as thirteen `AND [ESI+off], EAX` with `EAX = ~1`. The
timers sit inside interpolator members whose layout is still open.
`AsciiInf` embeds two AnmVms at +0x14 and +0x4FC.

## Pbg file streams

`Pbg::IFile` (vtable `0x004A4850`: seven `_purecall` slots, then the virtual
destructor) and `Pbg::File` (vtable `0x004A4828`: Open, Close, Read, Write,
Tell, GetSize, Seek, deleting destructor, ReadWhole) are real C++ classes; the
RTTI names come from the target. A plain virtual class hierarchy compiled as
normal COFF with `/TP /O2 /Oy- /Gy /MT` reproduces nine owners exactly,
including the compiler-generated scalar deleting destructors.

- `/Ob1` is wrong here: `~File` inlines `Close` in the target (46 bytes), so
  the default `/O2` inlining is required.
- `/Oy-` gives `Read`, `Write` and `Seek` their `PUSH EBP` frames, while
  frameless leaves such as `Close`, `Tell` and `GetSize` stay frameless.
- Whether these objects were `/GL` in the original is unknown; normal COFF
  reproduces them.
- `File::Open` (`0x0044FE00`) resolves relative names inline: `strcpy` when
  the name contains `:`, otherwise `GetModuleFileNameA` + `strrchr('\\')` +
  `strcat`. The original writes `slash[1]` even when `strrchr` returned NULL;
  keeping that literal reproduces the 419 bytes. Default `/GS` is on (cookie
  `0x004B01F4`, initial value `0xBB40E64E`). TH11's Pbg open called a
  separate path resolver, so the shape changed between the games.
- `ReadWhole` (`0x004500E0`) seeks to the current position with
  `FILE_BEGIN` before and after reading.
- `Pbg::MemFile` (vtable `0x004A491C`) keeps `_purecall` in the Open slot
  and adds a slot-8 virtual (`0x00450F00`) that loads its buffer through
  `0x00450380`/`0x00450450`. Layout: size +4, cursor +8, data +0xC. Seven
  owners are exact.
- `MemFile::Seek` (`0x00450FE0`) is a frontier. Computing a base pointer in
  the CURRENT and END cases and storing `cursor = base + offset` after the
  switch reproduces the original's shared tail and every instruction except
  one: the original pushes ESI before the dispatch, while VC10 shrink-wraps
  it past the FILE_BEGIN branch. Tried without success: per-case returns,
  `abs`/ternary/in-place negation, if/else chain, success-only returns with a
  shared `return 0`.

## VC10 whole-binary codegen profile and subsystem map (2026-11-25)

A Capstone sweep across all 2,214 confirmed authored game functions (`0x00401000..0x004708BC`) combined with VC10 RTM (`cl 16.00.30319.01`) probe builds (`.analysis/compiler-debug/`) establishes the global compiler flags:

- **`/Oy-` is globally enabled.** Across all authored functions with a local stack frame (`sub esp`), 356 use `push ebp; mov ebp, esp` (including aligned frames `and esp, -8`), while the remaining 6 only use `sub esp, 8` to stage float call arguments. Functions with stack parameters (`ret N`) keep EBP frames (438/440). Frame-less functions (`AnmVm::AnmVm`, `Chain` leaves) simply have no stack parameters or local frame.
- **`/fp:precise` (default x87, zero SSE) is globally enabled.** Game code contains zero scalar SSE instructions (`movss`/`addss`/`mulss`/`cvttss2si`/`ucomiss`), zero raw `fsin`/`fcos`, 43 inline `fabs`, and float-rounding `fstp/fld` spills before `ret` (`0x004070E0`, `0x00407260`, math wrappers). Under `/fp:fast`, VC10 emits raw `fsin`/`fcos` and drops the return rounding spill, falsifying `/fp:fast`.
- **`fsincos` is original inline assembly, not a compiler flag.** All 22 `fsincos` instructions in TH128 (14 identical 32-byte `SetPolar` copies that spill `this` to `[ebp-4]` and reload `eax`, plus 6 ANM geometry functions with `fstp [ebp-8]; fld [ebp-8]; fsincos`) cannot be generated by VC10 from pure C++ under `/fp:precise`, `/fp:fast`, or `/Oi`. Keep natural C++ `cos`/`sin` as honest frontiers.
- **`/GS`, `/EHsc`, `/GL`, `/OPT:NOICF`:** 146 functions carry the `/GS` cookie (`0x004B01F4`), 39 install `fs:[0]` C++ EH frames (57 unwind funclets in `.text$x`), Rich records 56 LTCG C++ objects, and identical bodies (`0x00403240`/`0x00403280`, `0x00406F80`/`0x0040CEA0`/`0x0045DAD0`) are not folded by ICF.
- **LTCG entry adapters and CRT pending origins:** All 16 `boundary-review-new-function` pending rows (`0x004725F6..0x004917E3`) lie in the static CRT/STL band and are now excluded (`crt-band-library-review`). All 12 `ltcg-entry-adapter-review` rows (`0x00404AC0..0x00445C80`) are 10-byte `push ebx/esi; mov r, ecx; call body; pop r; ret` address-taken `Chain` callback entries (identical to exact `Pbg::Archive::~Archive` at `0x0044F5E0`) and are now classified as authored (`ltcg-entry-adapter-resolved`). Only the 21 unreferenced `d3dx9math.inl` candidates remain open (`review`). All 64 provisional boundaries lie inside the CRT band (`>= 0x004725A4`); zero game-band boundaries are provisional.

### Coarse engine vs gameplay address map

- **Gameplay & UI (`0x00401000..0x0044F500`, ~305 KB):** `AsciiInf` (`0x401000..0x403FFF`), `Bullet` (`0x404000..0x40BFFF`), `Ice`/`Card` (`0x40C000..0x40FFFF`), `SprtView`/`SptInf`/`EclResourceInf` (`0x410000..0x415000`), `EnemyInf`/ECL VM (`0x415000..0x41FFFF`, incl. 18.8 KB `0x417FF0`), `Gui`/`Help` (`0x420000..0x427FFF`), `Laser*Inf` & `Player`/`Stage` (`0x428000..0x433FFF`), `Mother`/`GameManager`/`Pause` (`0x434000..0x43FFFF`), `Replay`/`Title`/`Score` (`0x440000..0x44F500`).
- **Engine & Platform (`0x0044F500..0x004708BC`, ~128 KB):** `Pbg` archive/file/LZSS (`0x44F500..0x451300`), D3D9/Window/App/Snapshot (`0x451300..0x458000`), Sound/BGM manager (`0x458000..0x4593B0`), **ANM engine** (`0x4593B0..0x468100`), `Chain` (`0x468100..0x468900`), Input/File/Cipher/Rng/AngleMath/MenuCursor/ZunTimer/ThreadInf/Easing (`0x468900..0x46C800`), DirectSound `CSound`/`CStreamingSound` (`0x46C800..0x46E100`), tail engine/SPT (`0x46E100..0x4708BC`).

## Open questions

- Per-unit flags, the TU partition, which objects were `/GL`, the library set
  and the link order. Both lanes in the handoff reproduce exact owners, so
  the split between normal and LTCG objects is still a hypothesis per file.
- How the original call graph made LTCG keep some arguments on the stack
  (`AnmManager::GetVmById`, the archive cipher before `size` became
  in-place) and keep values in memory across stores (`ZunTimer::Add`,
  `LaserInfiniteInf::Slot01`). Address escapes in probes fixed two such
  cases; the others may need the real callers.
- Why the original `CreateGameWindow` frame has two extra 4-byte slots.
