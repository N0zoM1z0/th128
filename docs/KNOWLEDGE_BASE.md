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
copy. `ZunTimer::Initialize` remains inline in the header; this callback
uses the local `ResetEffectTimer` helper because the shared `Reset()` is
now an out-of-line owner.

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

## Historical AnmVm reconstruction notes (2026-11-24, superseded totals)

> [!NOTE]
> The counts in this section (`95 exact overall`, `16 exact ANM owners`, and the
> `ResolveAnmVmId` 23-byte frontier) are from the 2026-11-24 snapshot prior to
> Phases 0–2. Current totals (`303 exact overall`, `17,121 exact bytes`, and
> exact 21-byte `ResolveAnmVmId` via `struct AnmId`) are tracked in
> `docs/PROGRESS.md`, `docs/RE_HANDOFF.md`, and the 2026-11-25 VC10 `/GL`
> section below. The `AnmVm::Update` opcode and geometry findings below remain
> valid reference notes.

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
call edges now match the target. The historical VM-id lookup experiment used
an unused-ECX fastcall shim and left a 23-byte wrapper. That shim has been
removed; the native `AnmId`
member and its canonical replay are described in
[the current ID/child finding](#anm-child-creation-and-native-id-lookup-2026-11-26).
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
timers sit inside `AnmInterp<T>` interpolator members (`start, goal, vel1,
vel2, ZunTimer timer, int duration, unsigned int mode`; verified in
`src/AnmInterp.hpp`/`src/AnmInterp.cpp`).
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

## VC10 `/GL` compiler debug findings from Phases 1–2 (2026-11-25)

Verified against TH128 (`resources/th128.exe`, VC10 RTM `16.00.30319.01`, link `10.00.30319.01`) across +201 newly accepted exact functions (`102 -> 303` exact, `6,620 -> 17,121` exact bytes):

- **`struct AnmId` by-value solves the `GetVmById` (`0x00467420`) / `ResolveAnmVmId` (`0x00467780`) LTCG stack-arg mystery.** In the tested ANM contexts, VC10 `/GL` promotes scalar and pointer parameters (`this`, `AnmId *`, `ZunVec3 *`) into registers while the by-value `AnmId` (`struct AnmId { unsigned int id; };`) stays on the stack. Declaring `AnmVm *AnmManager::GetVmById(AnmId id)` naturally keeps `this` in `EDX` and `id` on the stack (`push eax; call 0x467420`) with zero unused-`ECX` workaround, reproducing both the 89-byte `GetVmById` body and 30+ `AnmHelpers` / `ResolveAnmVmId` callers exactly.
- **`/Ob1` (inline only `__inline`/in-class) is required for ANM `/GL` units.** Under default `/O2` (`/Ob2`), VC10 `/GL` aggressively inlines out-of-line `.cpp` helpers (`GetVmById`, `EaseTransition`, `DrawAnmVmWithManager`, `Supervisor::EnterCriticalSection`/`LeaveCriticalSection`), inflating callers by 24–70 bytes. Adding `/Ob1` (`/TP /O2 /Ob1 /Oy- /Gy /MT /Isrc /GL`) keeps out-of-line `.cpp` calls intact while still inlining header-defined `inline` methods (`ZunTimer::Initialize`, `ResolveVmIdPtrInline`). Note: `Pbg::File` (`~File` inlining `Close`) still uses `/O2` without `/Ob1`, confirming per-TU/per-project inline settings differ between Pbg and ANM.
- **Interprocedural stack-alignment (`and esp, -8`) hoisting under `/GL`.** When a function has an 8-byte aligned local (`double`, `LARGE_INTEGER`) or calls a callee that does (`AnmVm::Update`), VC10 `/GL` hoists `and esp, -8` out of non-escaping callees into their callers. Taking the function's address in the probe (matching real `Chain` callback registration) restores `and esp, -8` inside the function (`SupervisorTime::GetElapsedSeconds` `0x00455160`, 266B exact; `UpdateWorldVms`/`UpdateUiVms` `0x00466AF0`/`0x00466BE0`).
- **Singleton `this` constant propagation unlocks IV strength reduction.** Calling `SoundManager` methods on `&g_SoundManager` lets `/GL` constant-propagate `this`, emitting the exact pointer+index induction variable (`cmp ecx, &queuedIds[12]`) and matching all 17 `SoundManager` functions (`0x00458410..0x0045921F`).
- **Nested signed range check vs unsigned range fold.** `if (x < 60 && x >= 0)` folds to `(unsigned)x <= 59` (`cmp/ja`, 5B). Writing `if (arr[slot] < 60) { int c = arr[slot]; if (c >= 0) { ... } }` preserves the separate `cmp ecx, 0x3c; jge; test ecx, ecx; js` (9B) observed in `SoundManager::PlaySfx` / `PlaySfxWithPan`.
- **x87 comparison operand order and `/fp:precise` ternary rounding merge.** Writing `if (g_TimeOrigin > elapsed)` emits `fcom qword ptr [g_TimeOrigin]; fnstsw ax; test ah, 5; jp` (-2B vs `fld; fcomp`). In `AnmInterp<T>::EvalProgress` (`0x0045DD90`/`C0`/`F0`/`0x0045DE30`), returning via ternary `(m <= 0xFu) ? EaseTransition(m, progress) : progress` causes `/fp:precise` to merge both arms at the `fstp [ebp-4]; fld [ebp-4]` float-rounding spill (`ja +9` instead of `ja +0xf`), matching all four 44-byte template instantiations.
- **Struct assignment (`mov`) unlocks `fldz` hoisting across `ZunTimer::Initialize`.** In `AnmVm::SetScaleTransition` (`0x0045E190`, 140B) and `SetSecondaryScaleTransition` (`0x0045E080`, 140B), copying the 2-float scale pair as a struct emits integer `mov`s instead of `fld`/`fstp`, freeing the x87 stack so VC10 hoists `fldz` to the prologue and shares `st(0) = 0.0f` across `Initialize()` (`fst`) and the post-init zeroing (`fstp`).
- **DLL import calls (`call dword ptr [__imp__...]`).** Win32/DirectX APIs such as `DirectSoundCreate8` must be declared `__declspec(dllimport)` so VC10 emits indirect 6-byte IAT calls instead of 5-byte direct `call rel32`.
- **`AnmInterp<T>` template layout (`src/AnmInterp.hpp`/`.cpp`).** `0x0045D3D0..0x0045E6B3` is a single C++ template `AnmInterp<T>` (`start, goal, vel1, vel2, ZunTimer timer, int duration, unsigned int mode`) instantiated for `AnmColor3`, `float`, `int`, and `ZunVec2`. Explicitly instantiating `template struct AnmInterp<T>;` in `src/AnmInterp.cpp` guarantees primary-TU PDB ownership under `compare-linked-function.py`.
- **Remaining natural-C++ frontiers from Phases 1–2:**
  - `Chain::CutImpl` (`0x004686D0`) and its 4 callers (`RunCalcChain`, `RunDrawChain`, `CutList`, `Cut`): exact-size natural C++, differing only in VC10 `/GL` `ECX`/`EDX` private tie-breaker.
  - `ClearD3DTexture` (`0x00467BD0`, 133B) / `ClearD3DTextureBlack` (`0x00467C60`, 272B): exact-size natural C++, currently reserving an extra 4B slot above `__security_cookie` (`[ebp-8]` vs `[ebp-4]`).
  - `AnmVm::Update` (`0x00459D90`, 13,604B), `SetEffectState` (`0x0045E6C0`, 133B), `UpdateWorldVms` (`0x00466AF0`), `UpdateUiVms` (`0x00466BE0`), `CreateVm` (`0x004670B0`, 365 vs 364B), `PreloadAnm` (`0x00465600`, 124B), `StopVmById` (`0x00467480`, 103B).

## ANM color snapshot and parameter order (2026-11-26)

Natural color snapshots plus a `(current, target, duration, mode)` API reproduce
both complete **227-byte** TH128 Japanese v1.00a color setters with VC10 RTM
`cl 16.00.30319.01`, `link 10.00.30319.01`, `/O2 /Ob1 /Oy- /GL`:
`AnmVm::SetColorTransition` at `0x0045E4D0` and
`SetSecondaryColorTransition` at `0x0045E2C0`. The canonical
[units](../config/match-units.toml) and [exact rows](../config/matches.csv)
cover each entire primary-TU PDB contribution and its only relocation,
`DIR32 g_TimeScale` at owner `+185` -> `0x004B6F58`, with zero differences.

[Hash-attested Ghidra review](../.analysis/anm-transitions-20261126-review.txt)
shows contiguous bodies ending at `0x0045E5B2` / `0x0045E3A2`, `RET 8`, no
external interior references or inline tables, and following INT3 padding.
Both are called only by Update. The observed private ABI is VM in EAX,
current bytes in EDX, target bytes in ECX, duration/mode on the stack.

In [AnmVmTransitions.cpp](../src/AnmVmTransitions.cpp), a three-int color
constructor reads byte components in 2/1/0 order. Constructing both endpoint
snapshots before either assignment preserves the target's complete input
loads before endpoint stores, including possible input aliases. Copying one
zero color into both velocity triples also reproduces the three separate
zero registers and the early x87 `fldz`. Construct current before target,
then copy current before target; the recovered API order lets LTCG choose
the target registers naturally. Update and probe calls were changed together
to preserve which endpoint is current and which is the goal.

The former scalar interleaving compiled to 197 bytes. Snapshot variants with
the former `(target, current)` API reached 227 bytes but differed in pointer
register allocation and endpoint store scheduling; reversing the constructor
or copy order alone did not close them. These are compiler observations, not
proof of the original C++ spelling. Separately, casting primary-effect mode
to a byte and moving its zero-velocity stores preserved 133 bytes but did not
match its prologue; those changes were reverted.
A subsequent byte-formal probe was rejected: VC10 promotes mode to DL,
producing a 132-byte owner ending in `RET 8`, whereas the target has three
stack slots and `RET 12`. The maintained `unsigned int` formal and low-byte
mask were restored. See the [byte-formal diagnostic](../.analysis/anm-effect-byte-formal-diff.txt).
The inventory signature and frontier symbol now agree with the maintained
`unsigned int` declaration; low-byte consumption alone does not prove the
original formal type.

Validation scope is the two new owners and the 81 existing exact units whose
build contexts compile the changed transition source, Update caller or probe
([selected units](../.analysis/anm-transitions-20261126-affected-units.json),
[cold replay](../.analysis/anm-transitions-20261126-affected-replay.log)).
All 83 cold replays passed with zero differences.
The header changes only parameter names, retaining the same C++ function
types and layout. In the shared normalization build, the Update support-TU
contribution is 13,688 bytes versus the 13,604-byte target
([diagnostic](../.analysis/anm-transitions-20261126-update-consumer.json));
this is a different context from `AnmVmUpdateBinding`. Update remains a
frontier; these matches do not establish a runnable product.

## ANM tick callbacks and internal update ABI (2026-11-26)

The target takes the addresses of the Chain callbacks at `0x00466690`
(46 bytes) and `0x004666C0` (7 bytes), which adapt ECX to the internal
EAX manager argument and tail-call `UpdateWorldVms` (`0x00466AF0`) and
`UpdateUiVms` (`0x00466BE0`). Directly taking the internal updates' addresses
in the diagnostic probe prevented VC10 RTM `/O2 /Ob1 /Oy- /GL` from choosing
that private ABI. Keeping the actual callbacks in the probe restores it
without ABI annotations or unused arguments. Both callbacks now have exact
linked-PE owner replays: the world owner has a DIR32 global-state field at
`+3` -> `0x004B8954` and a REL32 tail jump at `+42` -> `0x00466AF0`;
the UI owner has a REL32 tail jump at `+3` -> `0x00466BE0`. All 74 affected
units passed [cold replay](../.analysis/anm-tick-callbacks-affected-replay.log)
([selected units](../.analysis/anm-tick-callbacks-affected-units.json)).

The [hash-attested span review](../.analysis/anm-tick-callbacks-20261126-span.txt)
includes both callbacks and their intervening INT3 gap. The two external data
references land exactly at the entries (`0x004638FD` -> `0x00466690`,
`0x00463943` -> `0x004666C0`); neither callback has an external interior
reference. The world callback reads the global pointer slot `0x004B8954`
and flags at object `+0x60`. With a non-null state, it returns 1 without updating when
`((flags | (flags >> 2)) & 1) && (flags & 2)`; otherwise it tail-calls world
update. The UI callback always tail-calls UI update. `AnmPauseStateView` is
only a view of these observed fields, not a recovered full game class.

The [update-owner review](../.analysis/anm-manager-update-20261126-review.txt)
shows contiguous 229/246-byte bodies, no tables or external interior entries,
and INT3 following both. World update uses an extra-data callback's nonzero
return to delete the VM; UI update ignores that return and still calls the
VM update. UI resets the shared update count; world increments it.

Clearing `tails[i]->drawListNext` after assigning `tails[i]` reproduces world
initialization's base-pointer induction and shared zero register. The world
candidate shrank from 243 to 229 bytes. Its remaining raw difference is the
stack allocation (`0x84` candidate vs `0x8C` target), so it stays a frontier.
No unused array slots or padding were added. The 246-byte UI candidate still
schedules initial list-head clears and local tail stores differently. Aggregate
array initialization and caching the initial list node did not help; clearing
heads indirectly produced 240 bytes. Those UI experiments were reverted.
At that checkpoint both frontier units declared every reviewed address field for full owner replay:
the [world result](../.analysis/anm-manager-update-world-frontier.json) replays
12 fields and differs only at `+8`; the
[UI result](../.analysis/anm-manager-update-ui-frontier.json) replays 11 fields
and had 31 raw differences in `+43..+75`. Those historical mismatch records
are unchanged; UI update was subsequently closed by the loop below.

## UI update initialization and value-returned ANM IDs (2026-11-26)

A two-iteration head-initialization loop reproduces the entire 246-byte
`UpdateUiVms` owner at `0x00466BE0` under VC10 RTM `/O2 /Ob1 /Oy- /GL`,
including all 11 reviewed address fields. Unlike the previous scalar and
aggregate-initializer attempts, VC10 unrolls this loop into the target's
head clears, tail addresses and local stores in the original order.
[Initial complete replay](../.analysis/anm-ui-two-head-loop-final.json). All
75 affected exact units also passed the current
[cold replay](../.analysis/anm-id-return-affected-replay.log)
([selected units](../.analysis/anm-id-return-affected-units.json)).

The [factory/register/child review](../.analysis/anm-id-return-review.txt)
exposes an incorrect source interpretation of stack reuse. In the target
`SpawnChildVm` at `0x00467220`, the parent is cached in EDI at `0x0046729D`
before its old argument slot at `[ebp+0x10]` is reused for registration
results. EDI still supplies the parent child-list head at `0x004672E5`.
The old source explicitly passed `reinterpret_cast<unsigned int *>(&parent)`
to registration, then dereferenced `parent` afterwards. The reconstructed
source now receives an independent `AnmId` value, preserving the parent
through child-list insertion. The positioned child path likewise receives
its registration result as a local value.

A [pinned compiler ABI probe](../.analysis/anm-id-return-type-probe.cpp)
([normal-COFF output](../.analysis/anm-id-return-member-type.asm)) distinguishes
free functions from members: a free function returns a four-byte POD in EAX,
but a member returning that same POD uses a hidden stack result pointer and
`RET 4`. No constructor is needed. The four registration methods and
`AnmResource::CreateVm` therefore use ordinary `AnmId` returns. This supplies
a natural source API, not an exact private LTCG calling convention: some
hidden pointers are still promoted differently in candidate linked contexts.
An experimental ID constructor did not resolve that and added unnecessary
initialization; it was reverted.

The registration methods use the typed `g_AnmManager` slot and an inline
ordinary-pointer insertion helper. Forced volatile node reads, the child
helper's register hint and its volatile mode formal were removed. The
compiler now eliminates the empty-node insertion branch in head registration;
the target retains physical dead-branch instructions. Whole registration
owners remain frontiers (118 vs 124 bytes for tails, 93 vs 119 for heads in
the linked script context). Aggregate node-reset probes did not close them
and were reverted. At that checkpoint the child helper owners were
264 vs 266 bytes and 250 vs 230 bytes, without exact credit. The subsequent member-return
reconstruction below supersedes these measurements.
[Earlier observations](../.analysis/anm-id-return-frontier-observations.txt).
`CreateVm` remains 359 vs 364 bytes in its primary resource context. The
small child-owner size gap still hides many control/ABI differences
([complete diagnostic](../.analysis/anm-child-after-id-return-diff.txt)).
The world update remains a one-byte frontier after these changes
([current replay](../.analysis/anm-world-after-id-return.json)).
Seven affected factory/registration/child units now retain `/GL` as linked
frontiers with the maintained symbols instead of stale normal-COFF prototypes.

## ANM child creation and native ID lookup (2026-11-26)

For the original Japanese TH128 v1.00a and pinned VC10 RTM
`/O2 /Ob1 /Oy- /GL`, two more complete owners now replay exactly:
`AnmResource::SpawnChildVmAtPosition` at `0x00467330` (230 bytes,
11 address fields) and `AnmManager::StopVmById` at `0x00467480`
(103 bytes, two fields). The native `AnmManager::GetVmById(AnmId)`
replaces the old shim's canonical 89-byte row without adding byte credit.
[Positioned-child replay](../.analysis/anm-child-position-final.json),
[stop replay](../.analysis/anm-stop-final.json),
[native lookup replay](../.analysis/anm-lookup-native-final.json).

Both child creators are resource members returning the four-byte POD `AnmId`.
This naturally supplies the hidden stack result pointer; the resource is
promoted into EAX. An explicit caller-owned output pointer retained unwanted
aliasing with `Supervisor.flags`: even at 230 bytes it left 13 ordinary-byte
differences in the positioned creator. The member-return form permits the
target's flags load before the result store and closes the complete owner.
The creators now call the existing typed Supervisor section-9 lock methods;
their old absolute-address volatile lock implementation is removed.
[Complete target review](../.analysis/anm-id-return-review.txt),
[rejected output-pointer diagnostic](../.analysis/anm-child-value-position-diff.txt).

The parent main position is copied **before** `BindVmScript`; the three
script-variable words and the alternate position are copied afterwards.
`ResetForReuse` preserves the main position and binding executes an initial
VM update, so moving the main copy after binding changes script input and
can overwrite its output. Ordinary `ZunVec3` copies and `memcpy` for the three
adjacent script words reproduce the target's integer moves. The normal child
path also uses the reviewed list-insertion helper: it updates the new node's
next pointer only when the parent's existing next pointer is nonnull.

The core probe includes dynamic-manager lookup calls, following
`GetVmPosById` at `0x004676F0`, whose target forwards the receiver without
reloading the singleton. With only singleton callers LTCG specializes away
the manager and removes `StopVmById`'s required global load. Retaining the
actual dynamic-receiver use restores the complete 103-byte caller. Its
second argument is a signed 16-bit interrupt in BX: it calls the optional
callback and writes the interrupt to `currentFrame` on the VM and children;
it does not clear callbacks. The unused-ECX shim, scalar lookup overload and
probe pointer keeping that shim alive are all removed.
[Consumer/interrupt review](../.analysis/anm-native-id-consumers-review.txt),
[stop target review](../.analysis/anm-vm-factory-review.txt).

Remaining measured frontiers: normal child creation is 293 vs 266 bytes,
with registration return-pointer promotion and control-flow differences;
`SetInterruptAndUpdate` is 127 vs 128 bytes with a stack-alignment difference.
The latter now has a source mapping and linked frontier unit.
`BindVmScript` and `BindVmAtPosition` units now point to their actual
`AnmVmScript.cpp` definitions and retain `/GL`; both remain frontiers.
[Normal-child diagnostic](../.analysis/anm-child-member-flat-diff.txt).
The same current script link has a 262-byte `BindVmScript` with three
ordinary register-byte differences (DL vs CL), a 294-byte positioned binder
vs 295 target, and a 13,604-byte `Update` with extensive byte differences.
Equal owner sizes supply no exact credit.
[Bounded observations](../.analysis/anm-member-current-frontier-observations.txt),
[binder diagnostic](../.analysis/anm-bind-script-current-diff.txt).

The binders' shared callee at `0x00407080` is a complete 20-byte wrapper:
align ESP to eight bytes, load the stack VM into ECX, call `Update`, return
with `RET 4`. It has exactly those two binder callers. A natural manager
member taking only the VM could explain the binders' ECX singleton load;
this remains a hypothesis to compile before replacing their legacy volatile
singleton read and flag cast. The existing unimplemented `UpdateVm`
declaration's extra `stateFlags` parameter has no supporting evidence here.
[Wrapper review](../.analysis/anm-binding-update-boundary-review.txt).

The initial affected cold replay exposed five missing effect callbacks:
`14aee6d` had replaced their source while expanding effect-10, leaving their
canonical rows behind. The other 121 comparisons passed in a bounded audit.
[Failed replay](../.analysis/anm-member-affected-replay.log),
[failure audit](../.analysis/anm-member-comparison-audit.json).
The five callbacks are restored from their earlier natural source. Calling
the now-out-of-line shared `ZunTimer::Reset` produced 139 vs 187 bytes;
flattening the reset into the callback reached 187 bytes but moved the scale
constant field from target `+120` to `+115`. A local inline reset helper
restores the original nested inline operation: all 187 bytes and seven
relocations now match, as do the four complete three-byte empty callbacks.
This recovers existing claims and adds no exact credit.
[Recovered COFF replay](../.analysis/anm-effect-callbacks-reset-helper-replay.log),
[target review](../.analysis/anm-effect-callbacks-recovery-review.txt),
[explicit callback span](../.analysis/anm-effect-callbacks-recovery-span.txt).
The four script timer-helper contexts had identical executable contracts to
`AnmVmColorInterpolation`; only their output directories differed. Their four
`output_dir` entries now share that existing cold-link family. All six owners
still undergo separate complete comparisons. This reduces the affected family
from 12 compilation contexts to eight, without changing source, compiler or
link settings and without adding build machinery. After callback recovery,
the 45 linked consumers are covered by four cold-linked contexts; the five
COFF callbacks are freshly rebuilt and the other contexts' inputs are unchanged.
The final current-input comparison passed all 126 affected exact units.
[Final replay](../.analysis/anm-member-consolidated-final-replay.log),
[selected units](../.analysis/anm-member-affected-units.json).
The current world owner still has only the frame-size byte difference at `+8`:
[complete frontier replay](../.analysis/anm-world-after-callback-recovery.json).

## Natural ANM binding calls and inline timer resets (2026-11-26)

For the original Japanese TH128 v1.00a and pinned VC10 RTM
`/O2 /Ob1 /Oy- /GL`, `AnmResource::BindVmScript` at `0x004593B0`
(262 bytes) and `BindVmAtPosition` at `0x004594C0` (295 bytes) now
replay exactly as complete primary-script contributions. Each has six
reviewed address fields: VM reset, `g_TimeScale`, two singleton loads,
manager update and the failure-path `memset`.
[Script replay](../.analysis/anm-bind-script-natural-exact.json),
[positioned replay](../.analysis/anm-bind-position-natural-exact.json),
[complete target review](../.analysis/anm-binding-natural-target-review.txt).

The binders call the ordinary `AnmManager::UpdateVm(AnmVm *)` member.
This explains the target singleton load into ECX as a real receiver input;
the old free stdcall function needed an unrelated volatile global read.
The unsupported `stateFlags` formal is removed from the manager declaration.
The source now uses typed singleton and game-speed symbols and ordinary flag
accesses throughout both binders. This also closes the earlier DL/CL register
allocation differences without register hints.

A flat timer reset reached both full lengths but left 20 ordinary-byte
differences in each owner: the flag clear and argument push moved before the
two integer timer stores. The local inline `ResetBindingTimer` helper
preserves the reset operation as an inline call, reproducing all six fields
and every remaining byte. No additional timer state, stack slots or volatile
accesses were needed.
[Rejected flat reset](../.analysis/anm-bind-natural-member-diff.txt),
[helper diagnostic](../.analysis/anm-bind-reset-helper-diff.txt).

The target update wrapper at `0x00407080` remains a separate 20-byte frontier.
The earlier free stdcall body produces 15 bytes as normal COFF; the natural
member in the full `/GL` wrapper context produces only a five-byte tail jump.
The normal body keeps the stack VM input and `RET 4`, while omitting the
target's alignment and ESP restoration. The GL tail jump also loses that
stack-argument boundary. The source mapping and explicit linked frontier now cover that full
20-byte owner; its TU/ABI context remains open. The binder matches use the
reviewed symbolic wrapper binding in the existing diagnostic link family;
they do not establish closure with the real wrapper body or a native product.
[Normal-COFF observation](../.analysis/anm-binding-wrapper-normal-diff.txt),
[complete COFF extents](../.analysis/anm-binding-wrapper-complete-extents.txt),
[wrapper target review](../.analysis/anm-binding-update-boundary-review.txt),
[GL build](../.analysis/anm-binding-wrapper-native-gl-build.log),
[extent rejection](../.analysis/anm-binding-wrapper-gl-frontier.err).

The mixed COFF/GL consumer also builds with the natural member symbol; its
normal member wrapper is 15 bytes and its 13,604-byte `Update` remains a
codegen frontier.
[Mixed build](../.analysis/anm-binding-mixed-wrapper-build.log),
[bounded observations](../.analysis/anm-binding-mixed-wrapper-observations.txt).

The affected scope is 128 exact units across the existing eight compilation
contexts. The script family was already freshly cold-built for this source
and supplies eight of those owners; only the seven other contexts need
another cold build. All 128 current-input exact comparisons passed.
[Affected replay](../.analysis/anm-binding-affected-replay.log),
[selected units](../.analysis/anm-binding-affected-units.json).

## ANM first-stage loading, release logging and preload wait (2026-11-26)

For the original Japanese TH128 v1.00a and pinned VC10 RTM
`/O2 /Ob1 /Oy- /GL`, `AnmManager::PreloadAnm` at `0x00465600` now
replays its complete 124-byte owner and all eight address fields exactly.
The physical owner includes six internal alignment bytes omitted from the
sparse Ghidra body. It holds a manager snapshot across the cache-hit log,
calls the real `LoadAnm` member on a miss, sets resource `+0x124`, and waits
with `Sleep(1)` until that flag clears or Supervisor flags `0x180` are set.
[Exact preload replay](../.analysis/anm-preload-final-exact.json),
[target boundary and references](../.analysis/anm-load-slot-review.txt),
[snapshot diagnostic](../.analysis/anm-preload-manager-snapshot-diff.txt).

The loader at `0x004653F0` owns all 515 bytes through `0x004655F2`.
Ghidra's initial 96-byte function and apparent entry at `0x00465450` are
parts of this single owner; the complete span has no external interior
references. The source now implements the actual first loading stage:
check the slot upper bound of 38, read the ANM blob, allocate and cache a
zeroed resource, count linked archive entries/sprites/scripts, allocate
their tables, and cache external texture data. It does not yet populate
the sprite/script tables or create GPU textures; the later stage at
`0x00465770` remains unimplemented. Allocation and cache assignment precede
the null-blob test in the target, and the slot check has no lower-bound test.
[Complete loader span](../.analysis/anm-load-slot-complete-span.txt),
[loading-family hypotheses](../.analysis/anm-load-family-decomp.txt).

The target settles several resource fields: a **32-bit** file ID at `+0`,
a 260-byte filename at `+4`, the archive pointer at `+0x108`, texture count
at `+0x10C`, script count at `+0x110` and sprite count at `+0x114`.
The existing binding callers still narrow the file ID to the VM's 16-bit
field. Each texture entry is 20 bytes; raw data size is `+8`, and its pointer
is `+4`. The observed archive prefix has version `+0`, 16-bit sprite/script
counts at `+4/+6`, name offset `+0x10`, a byte flag at `+0x20` and relative
next-entry offset at `+0x24`. Advance by that offset, never by the prefix's
`sizeof`. The `hasEmbeddedTexture` field name is provisional: the reviewed
behavior is that a nonzero byte skips the external read.

`AnmResource::CacheExternalTexture` at `0x00465680` is a complete 231-byte
owner with one loader caller. It requires version 7; names beginning with
`@` also skip external reads. Otherwise it calls `OpenPath(path, &size, 1)`,
stores size then raw pointer, and returns 1. Version/read failures call the
193-byte `GameErrorContext::Fatal` at `0x0046A320` and return -1, rather than
the separate log method at `0x0046A250`.
[Complete cache review](../.analysis/anm-load-entry-review.txt).

The release logger at `0x00468130` is a one-byte `RET`. Its natural
ellipsis-only declaration `void __cdecl ChainDebugLog(...)` preserves the
format and payload pushes in the callers even though its body is empty.
With a fixed named format parameter, LTCG removed that parameter and
shortened the preload caller. The old probe's artificial volatile character
read is removed along with its fake load-slot implementation. This is a
source-shape explanation supported by compiler output, not recovery of an
original C++ declaration.
[Logger target review](../.analysis/anm-debug-log-review.txt),
[logger exact replay](../.analysis/anm-release-logger-final-exact.json),
[named-format failure](../.analysis/anm-preload-first-diff.txt).

The resource constructor is now inline in the class so the real loader can
inline its 312-byte zero initialization under `/Ob1`. Its separately emitted
19-byte constructor is still exact, including the `memset` call at `+9`.
The public contribution moved to the primary `AnmLoading` TU; the Oracle
correctly rejected the old resource-primary context where the constructor
belonged to a support object. The canonical constructor unit now follows
the actual primary contribution, without additional exact credit.
[Constructor replay](../.analysis/anm-inline-ctor-final-exact.json).

Two loading owners remain frontiers. `LoadAnm` is 511 vs 515 bytes: the
compiler rotates the target's loop-top null test into its backedge check.
Initializing `textureIndex` before allocation and inlining the constructor
fixed the earlier local lifetime and call differences. Rewriting the traversal as an explicit
infinite loop did not retain the missing physical test.
[Current loader diagnostic](../.analysis/anm-load-slot-explicit-loop-diff.txt).
The strict unit also rejects its short primary contribution before comparing
fields ([extent rejection](../.analysis/anm-load-slot-final-frontier.stderr)).
A controlled cold build using a pointer-width integer address sum in
`NextAnmEntry` still emits the same 511-byte candidate; pointer arithmetic
versus integer wraparound does not explain this loop shape.
[Integer-address probe](../.analysis/anm-load-integer-address-probe.py),
[observations](../.analysis/anm-load-integer-address-final-observations.txt).
The texture cache has the full 231-byte extent and all 13 address fields
replay, but raw SIB bytes at `+0xC4/+0xCE` are `0x0A` instead of target
`0x11`: `[edx+ecx+disp]` vs `[ecx+edx+disp]`. Equivalent addresses still
fail exact comparison. Typed/index-first and byte-offset accessors did not
change those bytes and were removed. The source retains direct typed stores;
no address normalization or padding was added.
[Strict cache frontier](../.analysis/anm-cache-texture-final-frontier.json),
[cache diagnostic](../.analysis/anm-cache-texture-conditional-diff.txt),
[rejected byte-offset accessor](../.analysis/anm-cache-texture-byte-offset-diff.txt).

The affected scope is 161 exact units in 12 compilation contexts, including
the Chain, ThreadInf and CoreMath consumers of the logging declaration.
The current preload, loader-primary constructor and Chain families already
had fresh cold builds; the other nine contexts were rebuilt for the changed
inputs. See the [final replay](../.analysis/anm-loading-final-replay.log) and
[selected units](../.analysis/anm-loading-final-affected-units.json): all
161 current-input comparisons passed. The current world-update frontier
also retains only its frame-size byte difference at `+8`
([strict replay](../.analysis/anm-world-after-loading.json)).

The next-stage target review also settles that resource `+0x124`, currently
named `isInvalid`, is a **one-based finalization cursor**, not a boolean.
The 1,000-byte owner at `0x00465770` walks the archive chain, accumulating
the earlier entries' sprite/script counts, and processes only the entry
whose zero-based index equals `cursor - 1`. It creates/selects that entry's
texture, fills its sprite records through `0x00466090`, and installs script
pointers from the eight-byte script table. If another entry follows it
increments the cursor and returns; reaching the final entry clears it to
zero. The reviewed fatal tails also clear it. The `+0x20` archive flag selects
the embedded-data helper with `entry + *(entry+0x1C)`; `@` names take separate
texture-creation paths. This is target behavior for the next reconstruction,
not new source or exact credit. The source field name is retained for this
checkpoint; rename it when implementing that stage and replay its consumers.
[Full finalization span](../.analysis/anm-finalize-textures-complete-span.txt),
[entry/caller review](../.analysis/anm-finalize-textures-review.txt).

The 140-byte scheduler at `0x00465E50` scans the 38 resource slots and
returns immediately after finalizing the first resource with a nonzero
cursor. Thus this path advances only one archive entry across the manager
per invocation. Resource `+0x128`, currently named `loadPending`, selects
the release path before finalization. The existing `ReleaseAnm` source
releases immediately; it does not set this flag. Preserve the scheduler's
target ordering when reconstructing it:
the ordinary in-range release branch clears the manager slot and then
reloads that slot before the `+0x128` clear. Do not silently repair this
apparent null dereference from a decompiler interpretation.
[Scheduler disassembly and caller](../.analysis/anm-load-scheduler-review.txt).

The bounded Ghidra `read_bytes` query was added to recover undefined CP932
diagnostics through the attested local route; see its
[usage](SHARED_TOOLS.md). Eight 96-byte data reads and the one-byte logger
read agree with the original target. Zero/oversized counts, unmapped starts
and a mapped range crossing its memory block are rejected. A short read
inside mapped section padding is allowed, so this operation supplies no
owner-boundary evidence.
[Target-byte smoke](../.analysis/anm-read-bytes-target-smoke.txt),
[zero](../.analysis/anm-read-bytes-zero.log),
[oversized](../.analysis/anm-read-bytes-oversized.log),
[unmapped](../.analysis/anm-read-bytes-unmapped.log),
[crossing](../.analysis/anm-read-bytes-section-crossing.log).

## ANM resolved sprites and finalization array bounds (2026-11-26)

For Japanese TH128 v1.00a and pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`,
`AnmResource::SetSprite` at `0x00466090` now reproduces all 145 bytes of
its complete primary-TU contribution, with no address fields. A fixed-size
72-byte `memcpy` followed by six float assignments naturally gives the
target's index in EAX, resource in EDX and input record in EBX. Its source
return type is void: EAX retaining the byte stride is an incidental result,
not evidence for the decompiler's integer return hypothesis.
[Current exact replay](../.analysis/anm-set-sprite-memcpy-final-exact.json),
[target/helper review](../.analysis/anm-sprite-and-texture-helpers-review.txt).

The finalization caller settles the resolved sprite layout:

| Offset | Observed role |
| --- | --- |
| `+0/+4` | file ID / texture index |
| `+8` | address of the texture slot |
| `+0x0C..+0x18` | pixel left, top, right, bottom |
| `+0x1C/+0x20` | actual texture height / width, as floats |
| `+0x24..+0x30` | normalized left, top, right, bottom |
| `+0x34/+0x38` | logical sprite height / width |
| `+0x3C/+0x40` | actual/archive width and height scale ratios |

The helper normalizes horizontal bounds by texture width and vertical
bounds by texture height. Logical width is `(right-left)/input.scaleX`;
height is `(bottom-top)/input.scaleY`. The caller first multiplies archive
coordinates by those scale ratios, so the resulting logical sizes recover
the archive sprite dimensions subject to float rounding. It initializes
the source record's input fields, copies the complete record, then
overwrites the six derived fields; zero-initializing the whole local would
add behavior absent from the target. The unrelated `+0x44` field remains
unknown. Existing VM binding uses the newly named fields at the same offsets.

The archive header's sprite-offset table starts at `+0x40`. Width, height
and format are unsigned 16-bit fields at `+0x0A/+0x0C/+0x0E`, texture origin
offsets are signed 16-bit fields at `+0x14/+0x16`, and embedded texture data
is at the relative offset `+0x1C`. The sprite record has an unused first
word followed by four floats; the maintained `id` label is provisional.
This path uses contiguous resolved indices. Eight-byte script descriptors
follow the sprite offsets; only
their second word is installed, as an absolute address. The script-address
array is therefore mutable. The legacy `isInvalid`/`loadPending` names are
now `finalizeCursor`/`releaseRequested`; `+0x12C` accumulates texture byte
counts as `textureMemoryBytes`.
[Complete finalization span](../.analysis/anm-finalize-textures-complete-span.txt).

`ProcessPendingAnms` initially compiled to 136 vs 140 bytes because
`slot >= 0 && slot < 38` folded into one unsigned range comparison.
Comparing the signed slot against the actual array's unsigned `sizeof`
count retains the target's separate negative and upper-bound checks. The
result is the complete 140-byte owner and all four fields replay. Its one
remaining ordinary-byte difference is `+0x73`: the source forwards the
resource in ESI, while the target uses ECX. This is a calling-context
frontier, not an address-field difference. The target's apparent null
reload on release remains in the source.
[Strict scheduler frontier](../.analysis/anm-process-pending-memcpy-frontier.json),
[array-count diagnostic](../.analysis/anm-process-pending-array-count-diff.txt).

`FinalizeNextEntry` now has maintained source, but its complete candidate
is 831 vs the 1,000-byte target. The source's integer-only release-log call
is eliminated, the eight-byte stack alignment is absent, the resource input
is ESI instead of ECX, and its remaining codegen differs. The earlier
ellipsis-only logger finding closes the preload caller, not this caller.
The four texture operations below remain declarations with reviewed target
identities; the diagnostic link anchors do not implement their behavior or
settle the caller's original private ABI. Do not tune register hints or
claim product closure from this build.
[Complete candidate diagnostic](../.analysis/anm-finalize-next-flat-diff.txt),
[extent rejection](../.analysis/anm-finalize-next-flat-frontier.stderr).

| Declared texture operation | Target | Full size |
| --- | --- | ---: |
| `LoadExternalTexture` | `0x00464FD0` | 511 |
| `LoadEmbeddedTexture` | `0x004651D0` | 296 |
| `CreateEmptyTexture` | `0x00465300` | 70 |
| `CreateRenderTarget` | `0x00465350` | 69 |

The first two use D3DX to decode/copy texture data; empty textures use
`D3DXCreateTexture`, and render targets use device slot `+0x5C`. Their
format and bytes-per-pixel tables begin at `0x004A4884/+0x24`; Supervisor
addresses `0x004D2F5C/60/64` provide render dimensions/format, and bit 0 at
`0x004D3068` selects a format fallback. Reconstruct those callees before
drawing conclusions about the finalizer's linked context. The separate
746-byte entry finalizer at `0x00465B60` is still unimplemented; it shares
the sprite and texture operations but returns 1/-1 instead of advancing
the one-based cursor.
[Texture hypotheses](../.analysis/anm-finalize-and-texture-hypotheses.txt),
[entry-finalizer target review](../.analysis/anm-finalize-entry-review.txt).

The nine observed format-table entries are `(0,21,25,23,20,26,29,28,27)`;
their byte counts are `(4,4,2,2,3,2,2,1,1)`. With the fallback bit set,
format values 0/21 select index 5, and value 20 selects index 3. These are
target data observations, not imported from an adjacent game's headers.
[Attested table reads](../.analysis/anm-texture-format-tables.txt),
[decoded values](../.analysis/anm-texture-format-table-values.txt).

The shared layout and caller changes affect 130 canonical exact units in
ten compilation contexts. The nine existing contexts were cold-built, and
the finalization family was rebuilt after selecting explicit byte copying.
An interim comparison reached that family while its artifacts were being
rebuilt; that incomplete-family rejection is retained. The final current-input
comparison passes all 130 units, including both resource binders, preload,
constructor, helpers and effect callbacks.
[Cold builds/interim replay](../.analysis/anm-finalization-affected-replay.log),
[finalization build](../.analysis/anm-finalization-memcpy-build.log),
[final comparison](../.analysis/anm-finalization-current-input-final.log),
[selected units](../.analysis/anm-finalization-affected-units.json).

## ANM GPU texture loading and shared graphics state (2026-11-26)

For Japanese TH128 v1.00a and pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`,
`AnmTextureLoading.cpp` now implements the four reviewed texture operations.
Empty-texture and render-target creation now replay exactly as complete
70/69-byte primary-TU owners, each with five reviewed address fields.
[Empty-texture replay](../.analysis/anm-texture-create-empty-final-exact.json),
[render-target replay](../.analysis/anm-texture-create-render-target-final-exact.json).
[Initial observations](../.analysis/anm-gpu-textures-first-observations.txt),
[complete creation/target review](../.analysis/anm-texture-creation-and-cpu-release-review.txt).

Empty texture creation clears flag bit 0, requests one managed mip level
through `D3DXCreateTexture`, ignores HRESULT, stores the format byte count,
and returns requested width times height times bytes per pixel. Render-target
creation sets bit 0 and calls device slot `+0x5C` using the current back-buffer
dimensions and format. It also ignores HRESULT and returns zero; its stored
byte count is four only for format 22, otherwise two. These unusual failure
and accounting paths are retained.

The external-file loader selects the 16-bit fallback, decodes the cached
raw file, and adopts the decoded texture when requested and actual
dimensions agree. That branch ignores the origin offsets. Otherwise it
creates a destination texture with mip count zero, clips a signed source
rectangle to the decoded dimensions, and copies the surface. Initial decode
failure returns -1; destination creation status is ignored, and copy failure
only logs. Temporary surfaces and the temporary source texture are released
in target order. CPU file data stays owned by the texture entry.

The embedded path uses a 16-byte payload header: signed format, width and
height at `+6/+8/+0xA`, followed by pixels at `+0x10`. The pitch uses payload
width and payload format's byte count, while the destination format can
use the configured fallback. The same rectangle is supplied as source and
destination bounds. Only texture-creation failure returns -1; the upload
HRESULT is ignored. The texture fields at `+0x0C/+0x10` are now named
`bytesPerPixel` and `flags`.
[Complete loader review](../.analysis/anm-sprite-and-texture-helpers-review.txt).

The Windows 7.0A SDK already present in the pinned toolchain supplies the
actual D3D9 interfaces. Four D3DX declarations are maintained separately;
their standard ABI was checked against Microsoft's
[creation](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxcreatetexture),
[file decoder](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxcreatetexturefromfileinmemoryex),
[surface copy](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxloadsurfacefromsurface)
and [memory upload](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxloadsurfacefrommemory)
documentation. TH128 parameters and behavior come from the local target,
including its literal filter/mip settings and nonzero HRESULT tests.

The source's separate `g_D3DDevice` storage and 90-slot placeholder interface
are removed. Sprite batching and texture creation now use
`g_Supervisor.d3dDevice` at physical `+0x10` (`0x004D2E70`) and actual SDK
interfaces. Existing batching bindings point to the same target address
through `g_Supervisor + 16`. Back-buffer dimensions/format occupy physical
`+0xFC/+0x100/+0x104`, and the fallback flag byte is `+0x208`.
This extends the maintained physical view without proving original C++
class or TU ownership.

The target passes `0x004D2F5C` directly to device slot `+0x40` at
`0x00454B60` and `0x00453E7A`. With the SDK's `IDirect3DDevice9::Reset`
signature this identifies a `D3DPRESENT_PARAMETERS` block there. Its
`Windowed` field is at `+0x20`, or `0x004D2F7C`, matching the maintained
Supervisor field formerly described as cursor visibility/windowed mode.
Recover the whole parameter block when continuing Supervisor work.
[Exact call-site review](../.analysis/anm-presentation-parameters-exact-call-review.txt),
[field references](../.analysis/anm-graphics-presentation-field-xrefs.txt).

Two loader owners remain frontiers. The external decoder is 477 vs 511
bytes. The embedded uploader has all 296 bytes and 13 address fields, but
16 ordinary bytes differ: failure-tail OR/POP scheduling and the order of
the final multiplications. Signed versus unsigned dimension formals,
expanding the release helper and spelling out the size accumulator all
generated the same remaining differences. The API-facing dimensions are
maintained as unsigned; this does not recover the original source types.
The final source keeps the concise release helper and product expression.
[Signed baseline](../.analysis/anm-gpu-textures-first-observations.txt),
[unsigned control](../.analysis/anm-gpu-unsigned-dimensions-observations.txt),
[flat-release control](../.analysis/anm-gpu-flat-release-observations.txt),
[accumulator control](../.analysis/anm-gpu-size-accumulator-observations.txt),
[embedded diagnostic](../.analysis/anm-texture-load-embedded-first-diff.txt).

The external candidate also promotes the X origin into ECX and returns
with `RET 12`, while the target leaves both origins on the stack and uses
`RET 16`. Its extra format local initially looked relevant: the target keeps
the effective format in EBX, where the candidate keeps the promoted origin.
A separate cold probe that mutates the format formal directly still emits
477 bytes, the same promotion and `RET 12`. The verified source is retained.
[In-place-format probe](../.analysis/anm-gpu-inplace-format-probe.py),
[observations](../.analysis/anm-gpu-inplace-format-observations.txt).

The external decoder calls `0x004645F0` after GPU creation. This is a
1,774-byte transparent-pixel RGB repair operation, not CPU buffer release:
it locks texture level zero and fills transparent pixels' color channels
from nontransparent orthogonal neighbors in several packed formats, then
unlocks and releases the surface. The embedded path does not call it.
`FillTransparentTextureColors` now has maintained source and a complete
frontier unit; see the transparent-pixel finding below for its current status.
[Postprocess hypothesis](../.analysis/anm-texture-postprocess-hypothesis.txt),
[entry/callers](../.analysis/anm-texture-creation-and-cpu-release-review.txt).

With the four real texture definitions in the link, `FinalizeNextEntry`
is now 887 vs 1,000 bytes and its resource input is naturally ECX.
`ProcessPendingAnms` consequently closes all 140 bytes and four fields,
including the previously differing receiver-forwarding byte at `+0x73`.
This adds a third exact owner (+140 bytes) without a register hint or ABI
shim. The exact sprite installer stays unchanged. The finalizer still has
extensive differences; the postprocess implementation is tracked below.
[Scheduler replay](../.analysis/anm-scheduler-with-real-gpu-frontier.json),
[owner observations](../.analysis/anm-finalization-with-real-gpu-observations.txt).

The Supervisor header, device-use changes and real texture definitions
affect 186 existing exact units. They and the two creation candidates all
passed in 19 cold compilation contexts. The newly closed scheduler is
included in the final canonical comparison: all 189 units passed. No further
cold build is needed for unchanged executable inputs.
[Cold comparison](../.analysis/anm-gpu-final-replay.log),
[canonical comparison](../.analysis/anm-gpu-canonical-final-compare.log),
[selected units](../.analysis/anm-gpu-canonical-affected-units.json).

## ANM transparent-pixel repair and inline neighbor helpers (2026-11-26)

For Japanese TH128 v1.00a and pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`,
`0x004645F0` fills transparent texels' RGB from nontransparent cardinal
neighbors. The complete 1,774-byte owner now has natural C++ source in
`AnmTextureLoading.cpp`, including all four packed-format paths. This closes
the missing source dependency of the external-file texture loader without
claiming exactness for the larger repair function.
[Executable-body review](../.analysis/anm-transparent-colors-body-review.txt),
[physical tables](../.analysis/anm-transparent-colors-tables.txt),
[table references](../.analysis/anm-transparent-colors-tail-refs.txt).

| D3D format | Alpha bits | Red / green / blue bits |
| --- | ---: | --- |
| 0 or 21, UNKNOWN / A8R8G8B8 | 8 | 8 / 8 / 8 |
| 25, A1R5G5B5 | 1 | 5 / 5 / 5 |
| 26, A4R4G4B4 | 4 | 4 / 4 / 4 |
| 29, A8R3G3B2 | 8 | 3 / 3 / 2 |

Only alpha-zero pixels are changed. Neighbors are considered in left,
right, up, down order and counted when alpha is nonzero, including partial
alpha. Red, green and blue are summed separately and divided with unsigned
integer truncation only when the count exceeds one. No qualifying neighbors
means zero RGB; one means its unmodified color. Writes retain the original
alpha. Consequently a repaired transparent pixel cannot become a contributing
neighbor later in the pass. Rows use the locked byte pitch; vertical pixel
offsets use signed division by four or two, rather than the image width.

The helper obtains texture level zero, gets the surface description, locks
with null rectangle and zero flags, then unlocks and releases unconditionally.
None of those HRESULTs is checked. Unsupported formats still take this
lock/unlock/release path. It does not free `rawData`, change the texture
entry's accounting, or run on the embedded-texture upload path.

The target retains four unreferenced helper copies immediately before the
repair owner: `0x00464540` (30 bytes), `0x00464560` (42), `0x00464590`
(42), and `0x004645C0` (41). They accumulate the same formats' RGB into
three four-byte sums and increment a four-byte count. The packed 16-bit
values are snapshotted before any sum writes. Their bodies explain the
repeated instruction sequences in the repair owner; explicit small inline
functions naturally retain complete out-of-line copies under VC10.
[TH128 helper review](../.analysis/anm-color-neighbor-complete-review.txt),
[initial whole-owner diagnostics](../.analysis/anm-colors-chained-family.txt).
TH10 and TH11 sources were consulted only for possible source shapes; their
addresses, compilers and exact claims are not used as TH128 evidence.

`0x00464510` is a separate 40-byte format normalizer. Supervisor byte
`+0x208` bit 0 enables UNKNOWN/A8R8G8B8 to index 5 and R8G8B8 to index 3;
otherwise it returns the input index. Both maintained texture-loading paths
now use its real inline definition instead of duplicating the branch.
[Complete TH128 review](../.analysis/anm-texture-normalize-format-review.txt),
[initial 40-byte/two-field diagnostic](../.analysis/anm-colors-normalize-format-observation.json).

The repair's physical tail belongs to its complete owner: executable code
ends at `0x00464CA9`, compiler alignment occupies two bytes, five absolute
jump-table entries start at `0x00464CAC`, and a 30-byte format dispatch map
occupies `0x00464CC0..0x00464CDD`. The manifest declares the map as inline
data; every byte still participates in comparison, and all seven local
address fields plus cookie/cookie-check fields remain explicit. The
instruction-only `review_span` command cannot decode the complete byte map
as instructions; the successful body review and raw table read cover the
owner separately. No analyzer database or Oracle code was changed.

Useful rejected approaches:

- A shared inline template containing the entire pixel loop was not inlined
  by `/Ob1`: the selected repair owner became 250 bytes with four separate
  loop owners. [Observation](../.analysis/anm-colors-template-observations.txt).
- Expanding four loops with independent scalar sums gave 1,790 bytes and a
  `0x40` frame. An array aggregate initializer changed the frame to `0x48`
  without closing the extent. [Scalar observation](../.analysis/anm-colors-expanded-observations.txt),
  [array observation](../.analysis/anm-colors-array-observations.txt).
- Real small neighbor accumulators and chained initialization of the three
  sums restored the complete 1,774-byte extent. That alone did not restore
  the target's `0x50` frame, register-save placement or every branch target.
  A shared array outside the switch retained the same remaining scope.
  [Chained-initialization observation](../.analysis/anm-colors-chained-observations.txt),
  [shared-array observation](../.analysis/anm-colors-shared-sums-format-observations.txt).

Five complete owners are now canonical exact matches (+195 bytes): the
40-byte normalizer with both reviewed `DIR32` fields and the four neighbor
accumulators totaling 155 bytes without address fields. Their original entry
references remain absent, so boundary confidence stays medium. All nine
affected exact comparisons passed after two cold builds: these five owners,
both creation methods, `SetSprite`, and `ProcessPendingAnms`. No shared header,
tool, profile or other compilation context changed.
[Cold construction](../.analysis/anm-colors-final-build.log),
[strict comparison](../.analysis/anm-colors-final-compare.log),
[selected owners](../.analysis/anm-colors-final-selected-units.json).

The large repair remains a frontier despite equal 1,774-byte extents. Its
frame is `0x4C` instead of `0x50`; the cookie and description are four bytes
higher, register saves move across COM calls, and local handler destinations
differ. Its byte-map address field is at `+87` instead of target `+89`, so
the exhaustive-field check rejects it before raw-byte acceptance. Original
source lifetimes and the relevant linked context remain unresolved; equal
size and matching small helpers do not settle this owner.
[Final owner/fields](../.analysis/anm-colors-final-observations.txt),
[strict rejection](../.analysis/anm-colors-final-repair-frontier.json).

The real repair and its neighbor helpers also change the external loader's
private ABI naturally. Before format normalization was factored, the owner
had 511 bytes and `RET 16`, but 57 ordinary differences concentrated in
cropping and register scheduling. The final real-normalizer context gives
515 vs 511 bytes and 18 fields; it promotes the format input to ECX and
returns with `RET 12`, leaving three stack arguments instead of the target's
four. Embedded upload remains 296 bytes with the same 16 raw differences
and 13 fields. `FinalizeNextEntry` is now 903 vs 1,000 bytes;
the scheduler and sprite installer remain exact. These observations update
the earlier GPU snapshot without adding credit to any larger frontier.
[Interim loader diagnostics](../.analysis/anm-colors-inline-neighbors-family.txt),
[final loader diagnostics](../.analysis/anm-colors-final-family.txt),
[current finalization owners](../.analysis/anm-colors-final-finalization-owners.json).

## ANM file-backed creation and existing-texture uploads (2026-11-26)

For Japanese TH128 v1.00a and pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`,
the three other direct callers of transparent RGB repair now have maintained
source: file-backed creation at `0x00464CE0` (245 bytes), image upload at
`0x00464DE0` (156), and region upload at `0x00464E80` (330). Together with
the existing decoder at `0x00464FD0`, all four direct caller definitions are
present in the texture-loading linked context. All three new owners remain
frontiers; source presence grants no new exact credit.
[Complete target owners/references](../.analysis/anm-additional-texture-creation-review.txt),
[final owner diagnostics](../.analysis/anm-additional-final-observations.txt).

`LoadTextureFile` normalizes the destination format, copies the path through
`sprintf(path, "%s", filename)`, and opens external file bytes. Failure to
read returns -1. The byte count is stored before D3DX decoding; decode failure
frees the new buffer and returns -1. Successful creation clears render-target
bit 0, repairs transparent RGB, stores byte accounting and retains the new
CPU buffer in `rawData`, returning zero. The extra caller-supplied argument
is the 32-bit color key, not an image-info pointer: its push is the fourth
argument from the end of `D3DXCreateTextureFromFileInMemoryEx`.
[Path-format bytes](../.analysis/anm-additional-texture-path-format.txt),
[complete 245-byte/14-field comparison](../.analysis/anm-additional-final-load-file-replay.json).

`UploadTextureImage` operates on an existing texture's level zero. It records
the supplied size, clears flag bit 0, loads the in-memory image with filter
1 and no color key, releases the surface unconditionally, repairs transparent
RGB and records format-index 1/5 byte accounting. Upload HRESULT is ignored
and the return value is zero. It does not store the input pointer. The
caller at `0x00427F09` selects `resource->textures[1]` and frees its image
buffer after this call, confirming that input ownership stays with the caller.
[Call-site review](../.analysis/anm-upload-texture-image-caller-review.txt).

`UploadTextureRegion` also changes existing level zero and records the input
size without taking CPU ownership. The external-image path gets actual
texture dimensions and uses destination `(0, destinationY, width, height)`:
the bottom is the absolute texture height. The embedded path follows the
archive header's `textureOffset`, uses signed payload dimensions, and uploads
the raw pixels with source `(0, 0, width, height)` and destination
`(0, destinationY, width, height + destinationY)`. Its source pitch and format
come from the embedded header, while byte accounting uses the normalized
requested format. Both paths ignore the upload result, unconditionally
release the surface, run transparent RGB repair and return zero.

`AnmD3DX.hpp` now declares the nine-argument surface-file upload import.
Its standard ABI was checked against
[Microsoft's function declaration](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxloadsurfacefromfileinmemory);
the TH128 rectangles, filters, cleanup and ignored HRESULTs come from the
local target, not that documentation.

The final file loader has the complete 245-byte owner and all 14 reviewed
fields, but eight ordinary bytes differ because filename and format use
different incoming registers/stack roles. Reordering the first two formals
and then normalizing the format formal in place produced the same eight
differences. The maintained API puts format first, consistent with the
existing format-led loader. Do not treat API ordering as the resolved cause
of this remaining private ABI difference.
[Initial observation](../.analysis/anm-additional-first-observations.txt),
[format-first control](../.analysis/anm-additional-format-first-observations.txt),
[final comparison](../.analysis/anm-additional-final-load-file-replay.json).

Image upload is 116 vs 156 bytes: the target's eight-byte stack alignment,
GS setup and size register are not reproduced. Region upload is 329 vs 330
bytes with ten fields and different format/data promotion, frame and return
stack count. Each has a full linked frontier unit with the target's six/ten
reviewed fields; no shortened comparison or ABI shim is used.
[Image rejection](../.analysis/anm-additional-final-upload-image-replay.json),
[region rejection](../.analysis/anm-additional-final-upload-region-replay.json).

Two bounded source-shape controls did not settle the larger repair either.
Sharing the loop indices and neighbor count changed a few spills but left
the 1,774-byte extent and `0x4C` frame. A common union for the three 16-bit
pixel views produced the identical complete owner hash
`ae0ac9d04cb5b5fe562b039ff3a9b502d8141279cf0ea26134e3c248633d3ad3`.
Both controls are retained privately; the simpler prior loop/view declarations
remain. Adding the three real caller definitions also leaves the repair's
frame unresolved, so absent caller bodies alone were not the explanation
in this measured context.
[Shared-counter control](../.analysis/anm-colors-shared-counters-observations.txt),
[union control](../.analysis/anm-colors-union-view-observations.txt),
[complete-caller context](../.analysis/anm-colors-complete-callers-final-observations.txt).

The shared entry declarations and local D3DX header change affect 138
existing canonical exact owners across 11 compilation contexts, selected
through their executable input contracts. All 138 passed complete cold
comparison; the three new frontiers were compared separately and rejected
honestly. `SetSprite` remains 145 bytes, the pending scheduler 140, and the
full finalizer stays at 903 vs 1,000. No compiler profile or comparison tool
changed. Tracking now records 375 mappings and 322 exact functions; the
three new source mappings do not increase the exact count.
[Cold build/comparison](../.analysis/anm-additional-final-replay.log),
[selected owners](../.analysis/anm-additional-final-selected-units.json),
[finalization observations](../.analysis/anm-additional-final-finalization-owners.json).

## HelpInf viewer, returned ANM IDs and page ownership (2026-11-26)

For Japanese TH128 v1.00a and pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`,
the caller at `0x00427CD0` is the nine-page help viewer. The target contains
`help.anm`, `help_%.2d.png`, and `src\game\help.cpp:92 HelpInf`; creation
allocates `0x1C0` bytes and publishes the object at `0x004B8A58`.
`HelpInf.hpp/.cpp` now maintain its physical layout, complete update flow,
page/resource worker bodies, VM creation, Chain callbacks and lifecycle.
[Target strings](../.analysis/manual-asset-strings.txt),
[viewer dispatch disassembly](../.analysis/anm-upload-image-caller-complete.txt),
[local creation/helper review](../.analysis/manual-local-helpers-review.txt).

| Physical field | TH128 offset |
| --- | --- |
| Calc/draw Chain elements | `+0x008/+0x00C` |
| Outer state / embedded timer | `+0x010/+0x014` |
| MenuCursor, size `0xD8` | `+0x028..+0x0FF` |
| Nine directory IDs and one page ID | `+0x100..+0x127` |
| Exit request / X origin | `+0x128/+0x12C` |
| ANM resource / pending CPU image | `+0x130/+0x134` |
| Page state / 128-byte filename / image size | `+0x138/+0x13C/+0x1BC` |

The initial outer state advances to browsing on its first tick. Page state
0 creates nine UI VMs, gives the selected label interrupt 2 and the others
3, clears texture slot 1, and falls through to directory input. Directory
input waits for timer 20 and uses trigger/repeat masks `0x10/0x20`.
Confirmation selects the image filename, resets the timer, queues a read and
stops all directory VMs. Cancel requests the outer exit state; it sets the
exit field after timer 30. These timer gates and the two fallthroughs are
preserved, including the upload-to-view fallthrough in page states 3/4.

The page worker reads through `FileSystem::OpenPath(..., 0)`, writes page
state 3 even if reading failed, then clears the shared thread's running flag
and sets its stop request. The next UI tick uploads into `textures[1]`, frees
the CPU image if nonnull, clears that pointer again, calls texture
`PreLoad()`, creates script 9, and enters page state 4. The viewing gate is
again timer 20. Up/down step only within pages 0..8; returning to the
directory recreates its nine VMs. No extra null/failure behavior was added.
[Worker bodies](../.analysis/manual-worker-callback-spans.txt),
[caller upload/free](../.analysis/anm-upload-texture-image-caller-review.txt).

The post-upload virtual call at `0x00427F44` uses slot `+0x24`, which the
pinned Windows SDK identifies as `IDirect3DResource9::PreLoad`. It is not
`GenerateMipSubLevels` at slot `+0x40`. The initial source used the latter;
the SDK review corrected this before final comparison.

The resource UI helper at `0x004281D0` creates a VM under Supervisor lock 9,
copies or zeros its position, binds the script, registers at the UI tail,
clears state flags `0xC0`, and returns an `AnmId`. The initial free-function
implementation returned this four-byte POD in EAX and emitted 184/20-byte
helper/caller owners at that stage. A resource member return supplies the
target's hidden result pointer and `RET 8` contract. Keeping the script index
dynamic in the callback probe
also preserves the generic `HelpInf::CreateVm` instead of specializing its
only visible direct call to script 9. The complete 35-byte caller now has
zero differences and its one call field binds to this actual resource member.
[Free-function observation](../.analysis/help-first-observations.txt),
[member observation](../.analysis/help-member-observations.txt),
[canonical caller replay](../.analysis/help-final-help-create-vm-replay.json).

The helper remains 202 vs 204 bytes. Fixed-size position `memcpy` preserves
the target's integer word copies; individual float assignments instead emit
x87 loads/stores. Snapshotting the registered ID's integer before constructing
the returned wrapper retains a separate registration temporary. The remaining
binder register input/result-lifetime differences are linked-context questions,
not grounds for register hints or a different ABI declaration.
[Measured final context](../.analysis/help-core-context-observations.txt).

The full update is now 1,104 vs 1,184 bytes (initially 1,092). Its complete
target extent includes seven bytes of internal alignment, a final alignment
byte, and five absolute dispatch entries at `0x0042815C..0x0042816F`; sparse analyzer ranges
alone omit these bytes. Source still differs in stack alignment/register
allocation, directory cursor clamping and the opaque task-launch calls.
The initial implementation's two public `ZunTimer::SetCurrent` edges are
now repaired through the maintained separate `0x00403280` copy; this fixes
call identity without closing the update owner.
[Body review](../.analysis/manual-update-body-and-tail-review.txt),
[owned table read](../.analysis/manual-dispatch-table.txt).

The shared task launcher at `0x00434CB0` starts a CRT thread; both HelpInf
worker bodies have plain RET tails and no consumed stack arguments.
The direct CRT path and source callback type boundary are reviewed in the
[lifecycle finding below](#helpinf-lifecycle-and-shared-update-setter-2026-11-26).

The initial HelpInf checkpoint matched five complete primary-TU owners,
adding 122 bytes and seven reviewed relocation fields:

| Target | Owner | Bytes | Fields |
| --- | --- | ---: | ---: |
| `0x00427880` | `HelpInf::CreateVm` | 35 | 1 |
| `0x004279C0` | `HelpInf::LoadPageWorker` | 68 | 5 |
| `0x00428170` | `HelpInf::Draw` | 6 | 0 |
| `0x00428180` | `HelpInf::OnTick` | 7 | 1 |
| `0x00428190` | `HelpInf::OnDraw` | 6 | 0 |

The unreferenced Draw copy retains a medium-confidence target boundary;
its six-byte extent and adjacent bytes were reviewed separately from the
callback. The resource worker remains 92 vs 90 bytes: its candidate private
`Fatal` receiver uses EDI rather than target ECX, adding an EDI save/restore.
The update and UI helper above also remain frontiers. Strict comparisons
reject all three owners at the complete-contribution size check.
[Page worker replay](../.analysis/help-final-help-load-page-worker-replay.json),
[Draw replay](../.analysis/help-final-help-draw-replay.json),
[tick callback replay](../.analysis/help-final-help-on-tick-replay.json),
[draw callback replay](../.analysis/help-final-help-on-draw-replay.json).

For that initial checkpoint, all 138 previously exact consumers of the changed
resource header were cold-built in 11 contexts; the new HelpInf context had
already been cold-built and receipt-verified. The final current-input comparison passed all 143
affected/new exact owners across these 12 contexts, including full owned
extents and relocation/call identities. That checkpoint did not implement
initialization or close launcher adaptation or runtime behavior.
[Final comparison](../.analysis/help-final-replay.log),
[selected owners](../.analysis/help-final-affected-units.json).

The existing texture context now includes the real HelpInf caller, MenuCursor,
sound operations and its callback probe. It still gives 116 vs 156 bytes for
image upload and 1,774 bytes with the same unresolved frame for RGB repair.
Adding the real caller alone did not explain those remaining GS/alignment
differences in this measured context.
[Texture consumer](../.analysis/help-texture-consumer-observations.txt),
[upload observations](../.analysis/help-upload-consumer-observations.txt).

## HelpInf lifecycle and shared update setter (2026-11-26)

For the same Japanese TH128 target and VC10 RTM `/GL /Ob1 /Oy-` context,
`HelpInf::Destroy` at `0x00427CB0` now reproduces its complete 29-byte
primary-TU owner and three reviewed fields: global pointer `+3`, Shutdown
call `+14` and operator delete call `+20`. It snapshots `g_HelpInf`, calls
Shutdown when nonnull, then deletes the cached pointer after Shutdown has
cleared the global. The larger Shutdown owner remains a frontier.
[Target review](../.analysis/help-shared-timer-and-shutdown-callback-review.txt),
[canonical replay](../.analysis/help-lifecycle-closure-help-destroy.json).

The setter at `0x00403280` is shared far beyond ANM: the analyzer's reviewed
xrefs identify 20 caller functions, including four ANM Update calls and the
two HelpInf calls at `0x00427F65/0x00428094`. `UpdateTimer.hpp` exposes the
existing natural implementation, still owned by `AnmVmScript.cpp`, without
aliasing the separate byte-identical public setter at `0x00403240`. Its
complete 61-byte canonical replay and `g_TimeScale` field at `+38` remain
exact after changing the anonymous symbol to a shared one. The original TU
partition is not established by this API.
[Setter replay](../.analysis/help-shared-timer-canonical-first.json),
[HelpInf call observation](../.analysis/help-shared-timer-first-Update.json).

Creation, initialization and shutdown now have maintained source and full
linked frontier units. Creation's target initializes the embedded timer bit
and MenuCursor defaults before clearing all `0x1C0` bytes and publishing the
object. The inline MenuCursor constructor preserves those ordinary defaults:
stack depth/value/disabled count zero, wrapping one and count 999.
Initialization allocates two Chain elements, installs callbacks, registers
priorities 10/64 with execution disabled, queues resource loading, resets
the timer and returns zero. Making the existing default ChainElem constructor
inline enables its actual body under `/Ob1`; its separate 27-byte canonical
owner still replays exactly.
[Lifecycle target review](../.analysis/manual-local-helpers-review.txt),
[constructor replay](../.analysis/help-lifecycle-chain-ctor-first.json).

Shutdown snapshots each element and Chain pointer before entering Supervisor
lock 0, cuts the elements, releases/deletes ANM slot 31 and clears the slot
and global. The first implementation reread each element after locking;
review corrected that ordering. The target does not free pending `pageData`
or stop the shared worker in this function.

The latest complete candidates remain 138/135 bytes for Create, 265/267 for
Initialize and 229/222 for Shutdown. Initialize's candidate promotes its
HelpInf argument into EDI and has a plain RET, while the target consumes a
stack pointer and returns with `RET 4`; Create consequently saves/restores
EDI around that call. Shutdown still has the known private CutImpl receiver
differences and a split zero-store epilogue. Resetting the timer in the usual
float/current/previous order did not close those differences. No artificial
function-pointer escape or ABI shim was added to force the target frame.
[Initial observations](../.analysis/help-lifecycle-first-observations.txt),
[current observations](../.analysis/help-lifecycle-closure-observations.txt).
The current-input strict comparisons reject all six HelpInf frontiers at
the complete-owner size check; none receives exact credit.
[Frontier rejections](../.analysis/help-lifecycle-closure-frontier-rejections.json).

The launcher's third caller, `0x00426910`, supplies entry `0x00426420`.
That five-byte entry jumps to `0x00425E60`; its complete 1,467-byte body
consumes no stack argument and both return paths have plain RET tails.
Thus this other call does not supply a conventional `RET 4` adapter either.
[Third caller](../.analysis/shared-task-launcher-third-caller-review.txt),
[entry jump](../.analysis/shared-task-launcher-third-entry-span.txt),
[complete worker](../.analysis/shared-task-launcher-third-worker-complete.txt).

The pinned target CRT stores the unmodified entry and argument in per-thread
fields `+0x54/+0x58`. It has no normal-path adapter: `__callthreadstartex`
pushes its stored argument at `0x004742CD`, calls the stored entry directly
at `0x004742D0`, then immediately pushes EAX and calls `_endthreadex`.
That function calls `ExitThread`. This explains why a plain-RET worker can
leave the unused argument on the stack without a subsequent normal return
depending on ESP; that is an inference from these instructions, not a runtime
validation or a recovered C++ callback declaration. The binary path is now
understood; the source type bridge remains external rather than inventing an
adapter or using an incompatible callback cast to obtain an exact launcher.
[CRT start review](../.analysis/shared-task-launcher-crt-start-review.txt),
[CRT call/exit review](../.analysis/shared-task-launcher-crt-entry-review.txt).

The final current-input comparison passed **174/174** affected/new exact
owners across 15 cold-built contexts. After placing the inline HelpInf
constructor definition in its header, only its two consuming contexts needed
another cold build; the other 13 current contexts were reused. The shared
setter and ChainElem constructor retain their existing credit. Only Destroy
adds credit here: one function and 29 bytes, bringing the canonical totals
to 328 functions and 19,658 bytes. Product and runtime closure remain later work.
[Final comparison](../.analysis/help-lifecycle-closure-replay.log),
[selected owners](../.analysis/help-lifecycle-closure-selected.json),
[current setter replay](../.analysis/help-lifecycle-closure-anm-vm-update-timer-set-current.json).

## Natural ANM draw dispatch and probe callee cleanup (2026-11-26)

For Japanese TH128 v1.00a and pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`,
the renderer at `0x00461B00` now has a maintained `AnmVm::Draw(manager)`
implementation in `AnmDraw.cpp`. Its complete target owner is 1,392 bytes:
1,276 bytes of instructions followed by 22-entry and 7-entry absolute jump
tables at `0x00461FFC/0x00462054`. All 29 apparent outside-body references
come from those owned tables. The frontier declares all 58 target fields:
26 calls, two table pointers, 29 entries and the graphics-state view.
[Target review](../.analysis/anm-render-dispatch-review.txt),
[complete instructions](../.analysis/anm-render-dispatch-complete.txt),
[owned tables](../.analysis/anm-render-dispatch-tables.txt).

Both `anm-core-context.cpp` and `anm-helpers-context.cpp` previously defined
a synthetic `DrawAnmVmWithManager` returning `flags ^ nextVmId` through a
volatile pointer. The helpers probe also fabricated BindAnmFile results by
summing arguments. Those bodies are removed. The real dispatcher is now in
every linked context containing its manager/helper consumers; unreconstructed
bind dependencies use the Oracle's explicit unresolved-symbol boundary.
The old unused one-argument draw approximation is removed from
`AnmVmEffects.cpp`, and the three `register` specifiers in the script source
are removed. None of these spelling or source-presence changes grants credit.

The dispatcher retains independent visibility tests for bits 0/1 and the
delete flag `0x20000000`. Modes 0..8 and 18 reject only when both colors'
alpha bytes are zero. Modes 1/3 call different rotated paths; modes
9/12/13/14 share triangle strips and 11 uses a fan, each with twice the VM
integer count. Mode 18 brackets world-matrix drawing with fog enable/disable
and returns zero. The primitive families compose parent X/Y position, scale
and angle only when state bit `0x100` is clear. They reread the mode after
applying render state rather than dispatching from a cached pre-call value.
Sprite/primitive leaf declarations remain source/API hypotheses; their
private conventions and implementations are not closed by this dispatcher.

`AnmFogState` is a partial view rooted at Supervisor `+8` (`0x004D2E68`),
with the device at `+8` and cached fog state at `+0x990`. Enable/Disable at
`0x00434A70/0x00434AB0` flush sprites before changing the cache and invoking
the SDK `SetRenderState(D3DRS_FOGENABLE, value)`. They return its HRESULT,
or zero when the cache already has the requested state. Both complete
54-byte primary-TU owners reproduce with two fields each: `g_AnmManager`
at `+12` and the real `FlushSpriteBatch` edge at `+17`.
[Leaf target review](../.analysis/anm-render-leaves-review.txt),
[enable replay](../.analysis/anm-fog-enable-current-canonical.json),
[disable replay](../.analysis/anm-fog-disable-current-canonical.json).

The draw-position helper at `0x004630F0` first rounds secondary + position
to three floats, then adds alternate position. A flat aggregate emitted
90 bytes and wrote directly to the result; nested value-returning vector
addition restores the target's two temporaries and 108-byte owner. Eight
ordinary bytes still differ at `+52..+59`: the X addition's memory-operand
order. Reversing the outer vector operands instead introduced a GS cookie,
a stack result argument, `RET 4` and a 133-byte owner; that hypothesis is
rejected without disabling GS. The retained helper remains a frontier.
[First observation](../.analysis/anm-draw-first-observations.txt),
[nested-vector observation](../.analysis/anm-draw-direct-gates-observations.txt),
[rejected reverse](../.analysis/anm-draw-member-observations.txt).

The free-function draw hypothesis emitted 1,324 bytes; the natural VM member
retains that size after restoring the position helper. Its 58 fields are
present, but its frame and private VM/manager registers differ from the
target. The former transparency helper stayed out of line under `/Ob1`,
adding ten calls; direct per-case source checks remove those unsupported
boundaries. No function-pointer escape, register hint or inert frame storage
was introduced to force the target ABI.
[Current observations](../.analysis/anm-draw-current-observations.txt),
[direct caller review](../.analysis/anm-render-direct-callers-review.txt).

The next relevant real callees include the small sprite wrappers at
`0x0045F5D0/0x0045FA10/0x0045FD40/0x0045FD70/0x00460240/0x00460B50`.
Their review distinguishes stack-manager paths from the two-stack-pointer
paths; the latter pass manager before VM in the physical call frame.
Their real definitions, rather than a synthetic callee, are needed to settle
the dispatcher/caller private ABI.
[Sprite wrapper review](../.analysis/anm-render-sprite-wrappers-review.txt).

The complete affected comparison cold-built 16 contexts and checked all
154 formerly exact owners plus four new candidates. Of the old rows, 129
still match. The 23 DrawLayer wrappers, DrawVmGroup and DrawSingleVm fail
the complete-contribution size check after the real draw replaces the
synthetic callee. Those **25 rows / 489 bytes** are withdrawn from the
canonical ledger, preserving their source mappings and reviewed boundaries.
For example, the layer-0 wrapper becomes 7 vs 12 bytes; DrawVmGroup becomes
127 vs 126 in its primary context. The previous probe took the manager
argument's address through a volatile pointer; its resulting private ABI
cannot be treated as the real renderer's ABI. Recover these callers through
real leaf definitions rather than restoring the probe body.
[Full current-input results](../.analysis/anm-draw-current-input-results.json),
[withdrawn rows](../.analysis/anm-draw-withdrawn-conditional-rows.json),
[core extents](../.analysis/anm-draw-core-current-owner-sizes.json).

The two fog owners add 108 bytes. All **131 currently accepted affected
owners** (129 retained and two new) have zero-difference comparisons in the
same full run; the dispatcher and position helper remain rejected frontiers.
The canonical totals are now **305 functions / 19,277 bytes**. Historical
logs retain the former conditional results; they were not rewritten into
passes. This is exact per-owner evidence, not native rendering closure.
[Comparison log](../.analysis/anm-draw-current-input-replay.log),
[final scope](../.analysis/anm-draw-final-scope.json).

## ANM sprite state, quad append and real corner wrappers (2026-11-26)

The original Japanese TH12.8 v1.00a renderer's state function at `0x0045E770`
now reproduces exactly under pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`.
`AnmDrawLeaf::ApplyState` uses the **passed manager**, with the VM in
private `EDI` and manager in `EAX`. It reads the blend selector from VM flags
bits 5–7, flushes before changing a cache, and rereads the VM selector
after the call. Cache byte manager `+0x4E9692` controls **point/linear
sampler filtering**: the target calls device vtable `+0x114`,
`SetSamplerState(0, MAGFILTER/MINFILTER, value)`, with linear for bit 2
clear and point for bit 2 set. The former one-VM approximation incorrectly
called `SetTextureStageState` at `+0x10C`; it now delegates to the real
explicit-manager implementation.
[Target helper review](../.analysis/anm-sprite-batch-helpers-review.txt).

The state owner's full extent is 416 bytes: 387 code bytes, one alignment
byte and seven DIR32 dispatch entries through `0x0045E90F`. Its 21 fields
are two real FlushSpriteBatch calls, eleven Supervisor device references,
one local table pointer and seven local entries. An ascending source case
order emitted 392 bytes and shared the source-blend call across cases
0/4/6. Target layout instead places case 6 before case 5. That natural
switch order produces the target's duplicated first calls and shared case
3/5 tail; no compiler flag or ABI hint is needed.
[Owned table read](../.analysis/anm-state-and-batch-owned-tables.txt),
[ascending-order observation](../.analysis/anm-sprite-delegated-state-state.json),
[target-order observation](../.analysis/anm-sprite-case-order-state.json).

The append owner at `0x0045F040` copies six 28-byte transformed vertices
in TL/TR/BL, TR/BL/BR order. Six ordinary struct assignments, each using
the manager's current `+0x8696F0` cursor, naturally emit the target's six
REP MOVSD sequences and cursor reloads. It then advances the cursor by
168, increments `+0x4E96EC` and returns zero. The complete 138-byte owner
has no address fields. Caching a destination pointer would assert a
stronger alias relationship than the observed reloads.
[Complete append review](../.analysis/anm-sprite-batch-helpers-review.txt),
[initial compiler observation](../.analysis/anm-sprite-first-append.json).

Both owners pass the canonical complete-primary-TU comparison: 416 bytes / 21 fields and 138 bytes / no
fields, including physical tails and all local table destinations.
[State canonical replay](../.analysis/anm-sprite-state-canonical.json),
[append canonical replay](../.analysis/anm-sprite-append-canonical.json). The separate
five-byte entry at `0x0045E910` jumps to the state **entry**, so it does
not create an unowned interior entry. It has no reviewed callers and
receives no source or exact credit in this checkpoint.
[Following jump review](../.analysis/anm-state-following-jump-review.txt).

Four small sprite wrappers now call the maintained mode-0, mode-2/3 or
mode-1 corner builder and the external real submission boundary at
`0x0045E920`. `0x0045FA10` receives manager before VM on the stack and
returns with `RET 8`; its logical source signature and dispatcher edge
now follow that order. The shared TL/TR/BL/BR quad at `0x004DAF28` has
four 28-byte elements; the attested target data is initially all zero.
The two rotated entries remain separate natural functions. None of these
four wrappers has exact credit while its corner/submission private ABI
still differs.
[Wrapper review](../.analysis/anm-render-sprite-wrappers-review.txt),
[quad data](../.analysis/anm-sprite-quad-data-and-state-table.txt).

The complete submission extent is 1,532 bytes through `0x0045EF1B`,
including three alignment bytes and four table entries. It adds the passed
manager's XY view offset, optionally snaps the four axis-aligned edges,
caches all four XYZ vectors into the VM, computes UVs, rejects quads outside
the unsigned viewport, updates texture/vertex-format state, then calls
the real state and append owners. Its table selects uniform
primary/secondary colors for modes 0/1 and horizontal/vertical gradients
for modes 2/3. Submission flag 2 skips **all** color selection and
modulation, preserving the shared quad's existing color words. Otherwise,
when manager color modulation is enabled, it uses the existing 25-byte
`AnmInterpHelpers::BlendColorChannel` owner (`0x0045E750`) for all four
channels: multiply, shift right by 7, saturate to 255. The viewport pointer
at `0x004D33B4` is read unconditionally.
[Complete submission disassembly](../.analysis/anm-sprite-batch-complete.txt),
[boundary and xrefs](../.analysis/anm-sprite-batch-owner-review.txt),
[table and constants](../.analysis/anm-batch-table-and-round-constants.txt).

The submission's FRNDINT operations follow the ambient x87 rounding mode
before subtracting `0.5f`. The pinned VC10 headers do not declare
`rint`/`nearbyint`; ordinary floor or a fixed nearest-even helper does not
settle that contract. Submission stays external pending a natural source
solution and a complete-owner comparison. The old Effects approximations
are not evidence for this owner.

The complete current-input replay cold-built **11 affected contexts** and
compared **87 owners**. All 77 existing exact owners remain exact; the two
new owners add 554 bytes. The four wrappers, Draw, draw-position helper,
DrawVmGroup and DrawSingleVm remain rejected frontiers. The four wrapper
candidates are 55/52, 51/55, 51/48 and 51/48 bytes (candidate/target);
Draw is 1,348/1,392 bytes. Draw-position retains its eight X-addition
operand-order differences. No former caller credit was restored. Canonical
totals are **307 functions / 19,831 bytes**, including 299 authored exact
functions / 19,502 bytes. Native rendering closure remains separate.
[Complete results](../.analysis/anm-sprite-deps-current-results.json),
[replay log](../.analysis/anm-sprite-deps-current-replay.log),
[final scope](../.analysis/anm-sprite-deps-final-scope.json),
[current owner observations](../.analysis/anm-sprite-canonical-draw.json).


## Maintained sprite submission and ambient rounding (2026-11-26)

`AnmDrawLeaf::SubmitSprite` now supplies the complete natural source for
original Japanese TH12.8 v1.00a `0x0045E920`, including offsets, snapping,
XYZ cache, UVs, viewport rejection, texture/vertex-format caches, all four
color layouts and the real state/append calls. Under pinned VC10 RTM
`/O2 /Ob1 /Oy- /GL`, the candidate is **1,660 / 1,532 bytes** and remains
a complete-owner frontier. Its private VM `EAX`, manager `ECX`, flags on
the stack and `RET 4` now agree with the observed target entry.
[Current candidate](../.analysis/anm-submit-direct-colors-submit.json),
[target review](../.analysis/anm-sprite-batch-complete.txt).

The viewport slot at `0x004D33B4` has a named external data declaration,
with a partial D3DVIEWPORT9 view at pointee `+0xCC`. Texture slot cache
`+0x4E9688` is assigned **before** flushing and reread for SetTexture;
vertex-format cache `+0x4E968E` is set **after** flushing. Culling occurs
after XYZ and UV writes but before any rendering-state or color updates.
Flag 2 preserves all four existing quad colors. Color selection is read
after the state-changing calls. No null-pointer fallback was added to
these unconditional target accesses.
[Target paths](../.analysis/anm-sprite-batch-complete.txt),
[maintained source](../src/AnmDraw.cpp).

A first inline `ModulateSpriteColor` helper stayed out of line under
`/Ob1`, replacing the target's twelve channel calls with three helper
calls. That 1,484-byte candidate had the manager in private EBX and
unsupported boundaries. Direct channel statements restore all twelve
calls to the existing 25-byte `AnmInterpHelpers::BlendColorChannel` and
the target manager/VM register assignment. All linked draw contexts now
include its real `AnmInterp.cpp` definition. The helper's complete 25-byte
code also agrees in this new support context; this diagnostic observation
adds no separate exact credit.
[Rejected helper observation](../.analysis/anm-submit-first-submit.json),
[direct-color observation](../.analysis/anm-submit-direct-colors-submit.json),
[blend support observation](../.analysis/anm-submit-direct-colors-blend-support.json).

The target owner declares **100 exhaustive address fields**: 69 shared
quad references, four half-pixel constants, one viewport slot, four
unsigned-conversion constants, one Supervisor device reference, sixteen
real calls, and five local table fields. The original PE has no relocation
directory; its absolute instruction operands and four owned table entries
must be reviewed explicitly. The candidate's PE HIGHLOW fields are still
fully enumerated by the Oracle. The early relocation-directory-only draft
found just sixteen calls and was incomplete; it was replaced before any
comparison by the attested-instruction/table inventory.
[Complete target fields](../.analysis/anm-submit-target-fields-complete.json),
[reviewed table/constants](../.analysis/anm-batch-table-and-round-constants.txt).

The compiler rejects `rint`, `nearbyint` and `_frnd` as intrinsics (C4163).
It accepts `floor` in the pragma but still emits a 22-byte function with a
real floor call in this fixed profile. None of those declarations yields
FRNDINT. The probe kept `/GL` and the existing optimization/FP profile;
no alternative profile or forced ABI was used.
[Compiler diagnostics](../.analysis/anm-round-intrinsic-probe.log),
[linked observations](../.analysis/anm-round-intrinsic-owner-observations.txt).
The separate mode-0 corner builder still calls the actual CRT floor at
`0x0048FDF0`; its floor operation must remain distinct from submission's
ambient FRNDINT. [Attested mode-0 callee](../.analysis/anm-submit-mode0-floor-edge.txt).


`AnmSpriteRounding.hpp` now expresses the mathematical edge result for
all four x87 rounding modes: floor, ceil, truncation, or nearest with ties
to even, then subtract one half. Submit reads the current mode once via
`_control87(0, 0)` and never changes it. A bounded VC10 `/GL` executable
linked against the selected real static CRT passed 24 mode/value checks
and a value above the signed-int range; it also checked that rounding
control was preserved. This does **not** establish floating-point
exception equivalence or whole-game runtime behavior.
[Bounded rounding smoke](../.analysis/anm-sprite-rounding-smoke-linked.log),
[smoke source](../.analysis/anm-sprite-rounding-smoke.cpp).

The remaining physical differences are explicit: the candidate adds a
control-mode query and four real rounding-helper calls instead of the
target's four FRNDINT instructions and half-pixel references. Those math
calls also introduce eight-byte stack alignment. It has 21 REL32 and
81 DIR32 fields (102 total), versus target 16 and 84; its 70 quad references
include one extra load when copying the last snapped edge. No barriers,
register hints, fake callees or alignment escapes were introduced to erase
these differences. Exact acceptance still requires the entire 1,532-byte
owner and all 100 fields.
[Candidate assembly](../.analysis/anm-submit-direct-colors-submit.asm),
[canonical rejection](../.analysis/anm-submit-current-results.json).

The final current-input replay cold-built **10 affected linked contexts**
and compared **106 owners**. All **74 existing exact owners** remain
exact, including state and append. All 32 frontiers remain rejected,
including all 23 maintained DrawLayer callers; no caller credit was
restored. This checkpoint adds one authored source mapping and no exact
rows: **397 mappings / 307 exact functions / 19,831 exact bytes**.
[Full results](../.analysis/anm-submit-current-results.json),
[replay log](../.analysis/anm-submit-current-replay.log),
[selected exact owners](../.analysis/anm-submit-current-selected.json),
[cold contexts](../.analysis/anm-submit-current-contexts.json).


## Rectangle drawing and paired vertex buffers (2026-11-26)

The original Japanese TH12.8 v1.00a solid/gradient rectangles and their
two border wrappers now have maintained natural source in `AnmDraw.cpp`,
under pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`. These are the real mode
15/19/20/21 callees. Their logical return type is int: every target ends
with zero in EAX. Draw's four call bindings now name those int-returning
definitions. All five new owners, including buffer reset, remain frontiers.
[Rectangle target review](../.analysis/anm-rectangle-family-review.txt),
[border/reset review](../.analysis/anm-outline-and-primitive-submit-review.txt).

| Owner | Target bytes | Current candidate bytes | Target fields |
| --- | ---: | ---: | ---: |
| ResetBuffers `0x0045EF20` | 56 | 56 | 1 |
| Rectangle `0x004620E0` | 533 | 517 | 12 |
| GradientRectangle `0x004626B0` | 535 | 520 | 12 |
| OutlinedRectangle `0x004628D0` | 139 | 141 | 2 |
| OutlinedGradientRectangle `0x00462960` | 160 | 163 | 2 |

The rectangle functions halve width and height using the target's double
`0.5` at `0x004A65A8`, then form four rotated XYZRHW/color vertices.
Products are combined before adding the center; preserving those
parentheses matters for floating-point results. Gradient colors are left
on vertices 0/2 and right on 1/3. The functions flush the **global** sprite
manager and disable Z writes. Texture-stage mode is cached globally;
vertex-format mode, vertex cursor and draw count belong to the **passed**
manager. They select diffuse color, set FVF `0x44`, draw two triangle-strip
primitives, reread/advance the cursor by 80 bytes and increment draw count.
[Half constant](../.analysis/anm-rectangle-half-constant.txt),
[maintained functions](../src/AnmDraw.cpp).

Borders draw an expanded rectangle first: width + 1, height + 1, RGB
unchanged and each alpha channel shifted right once. They then draw the
original rectangle and color(s). Both
real calls are declared as REL32 identities in each complete owner.
The target rectangle uses FSINCOS, while the current natural source emits
real sin/cos calls. Those calls add two fields and change private color
registers and stack alignment in the callers; trigonometric/range/exception
equivalence is unverified. The candidates retain those differences.
[Current rectangle observations](../.analysis/anm-rectangle-common-view-solid.json),
[border candidate](../.analysis/anm-rectangle-common-view-outlined.json).

ResetBuffers has three reviewed render-pass callers. Its
stores establish two fixed vertex regions in the manager:

| Region/field | Manager offset | Extent or role |
| --- | --- | --- |
| Sprite counter | `+0x4E96EC` | cleared on reset |
| Sprite storage | `+0x4E96F0` | `0x380000` bytes = 131,072 × 28-byte vertices |
| Sprite cursor/start | `+0x8696F0/+0x8696F4` | both point to sprite storage |
| Primitive counter | `+0x8696F8` | cleared on reset |
| Primitive storage | `+0x8696FC` | `0x50000` bytes = 16,384 × 20-byte vertices |
| Primitive cursor/start | `+0x8B96FC/+0x8B9700` | both point to primitive storage |

A partial `AnmDrawBuffers` view now expresses that region as one object.
The reset candidate has the target offsets and 56-byte extent but 23
ordinary differences: it materializes zero in ECX after the sprite-pointer
stores; the target prepares EDX earlier. Separate arena views, earlier
primitive-head values and chained count assignments changed scheduling or
registers without matching. The complete Oracle grants no credit for the
same size or field offsets.
[Target reset](../.analysis/anm-outline-and-primitive-submit-review.txt),
[current reset observation](../.analysis/anm-rectangle-common-view-reset.json),
[head-lifetime observation](../.analysis/anm-rectangle-reset-lifetime-reset.json).
The three callers reset after BeginScene and before the draw chain. A
separate diagnostic probe invoking the real reset body before DrawVmGroup
retained the same 23 differences, so that probe change was not adopted.
[Render-pass callers](../.analysis/anm-buffer-reset-graphics-callers-review.txt),
[modeled frame-caller observation](../.analysis/anm-rectangle-frame-caller-reset.json).
A separate zero-argument default-calling-convention probe emitted the same
56 bytes and 23 differences; the maintained declaration remains stdcall.
[Default-convention observation](../.analysis/anm-rectangle-default-reset-reset.json).

Five bounded vector-source probes also leave the position helper open:
copy-plus-`+=` emits 144/108 bytes; a free operator+, named intermediate,
component assignments and a value-construction helper each reproduce the
108-byte size but retain the same eight differences at +52..+59. The
original source is retained. These observations give no exact credit and
avoid repeating equivalent spellings as new hypotheses.
[Copy/add probe](../.analysis/anm-draw-position-copy-add-position.json),
[operator probe](../.analysis/anm-draw-position-operator-plus-position.json),
[value-construction probe](../.analysis/anm-draw-position-value-construction-position.json).

The reviewed mode-17 entry `0x00462D60` generates count + 1 polar vertices
at `2*pi/count` angular steps and draws a D3DPT_LINESTRIP. Its capacity
check also supports the primitive-store end at `+0x8B96FC`; the polar
helper at `0x004630A0` is another FSINCOS frontier to reconstruct later.
[Line-strip target](../.analysis/anm-outline-and-primitive-submit-review.txt).

The real rectangle/border bodies also recover the target private caller
ABI for **23 unchanged DrawLayer wrappers**: Layer0 is 12 bytes and the
other 22 are 15 bytes each, restoring **342 exact bytes**. Each complete
owner has one REL32 field to the real DrawVmGroup at `0x00466CE0` (offset
6 for Layer0, 9 for the others). DrawVmGroup and DrawSingleVm themselves
remain complete-owner frontiers. No synthetic renderer or probe was
restored; the earlier withdrawal was valid for that earlier input context.

The final current-input replay cold-built 11 contexts and compared all
116 selected owners. All 79 affected existing exact owners and all 23
recovered callbacks pass the full linked Oracle; 14 frontiers are rejected.
The current draw dispatcher is 1,452/1,392 bytes. Canonical totals are
**402 source mappings / 330 exact functions / 20,173 exact bytes**.
[Full results](../.analysis/anm-rectangle-current-results.json),
[replay log](../.analysis/anm-rectangle-current-replay.log),
[existing exact selection](../.analysis/anm-rectangle-current-selected.json),
[cold contexts](../.analysis/anm-rectangle-current-contexts.json).

## Frame entry points and the 2D viewport (2026-11-26)

Original Japanese TH12.8 v1.00a has three reviewed frame entry points at
`0x004547D0`, `0x00454BD0` and `0x00454E50`. They now have maintained natural
source in `SupervisorFrame.cpp` under pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`.
They receive a stack pointer to the main-window/frame-clock view rooted at
`0x004D3970`, which is Supervisor `+0xB10`. This is distinct from the camera
view rooted at `0x004D3184`. The primary loop passes that exact frame base.
[Complete frame bodies](../.analysis/anm-buffer-reset-graphics-callers-review.txt),
[primary-loop call sites](../.analysis/frame-root-instructions.txt).

The common path flushes sprites, selects the default camera, updates its
matrices and D3D viewport, sets the mode at `0x004D33B8` to 1 and runs the
calc chain.
Calc result zero stops the Supervisor thread and returns 1; result -1 stops
it and returns 2. The frame counter is **signed byte** `+0x14`; it increments
before comparison with unsigned config byte `0x004D304E` plus one. When due,
the path calls BeginScene, ResetBuffers, DisableFog, the draw chain, sprite
flush, SetTexture(0, NULL), EndScene and the mode's presentation boundary.
It resets the counter before presenting and finally stores elapsed work
time at `0x004D3820`.

The write of `0xFF` to `0x004D37F8` is **fog-cache invalidation**, immediately
before DisableFog; it aliases Supervisor `+0x998`, or AnmFogState `+0x990`.
It forces the real fog path to flush and issue SetRenderState even if fog
was previously off. It is not a draw-color field. The first source probe
had that partial-view field four bytes too high; it was corrected before
the maintained-source replay.
[Fog implementation](../src/AnmDraw.cpp),
[frame source](../src/SupervisorFrame.cpp).

| Frame-clock field | Offset from `0x004D3970` | Absolute address |
| --- | --- | --- |
| Frames since draw | `+0x14` | `0x004D3984` |
| Current / previous sample | `+0x2040/+0x2048` | `0x004D59B0/0x004D59B8` |
| Next tick deadline | `+0x2050` | `0x004D59C0` |
| Existing GetElapsedSeconds time origin | `+0x2058` | `0x004D59C8` |
| Last present / raster sample | `+0x2060/+0x2068` | `0x004D59D0/0x004D59D8` |

Window flag `0x10` selects the adaptive entry, which advances the deadline
while current time is greater, using the attested double `1/60`. Without
that flag, presentation interval 1 and config byte `0x004D304E == 0` select
the vsync entry; the other cases select the timed entry. Timed entry samples
the clock, resets its deadline on rollback, sleeps once when at least 1.5ms
early, and performs no frame work until current time is greater than the
deadline. It then catches up the deadline in 1/60 steps. The source retains
the ordered `>` guard, including its unordered no-work case.
[Clock constants](../.analysis/frame-timing-constants.txt),
[primary-loop decompiler hypothesis](../.analysis/frame-root-and-present-hypotheses.txt).

The float tangent boundary at `0x00435EC0` reproduces all 31 bytes and its
one REL32 identity to CRT `__CItan` at `0x0048FD00`, including the float
round-trip and stack cleanup. Its existing `vc10-math-wrapper-review`
compiler/header exclusion is preserved: the pinned `math.h` has the same
float-rounded tan expression. This is no new authored exact credit.
[Complete linked comparison](../.analysis/frame-tangent-cached-manager-canonical.json),
[existing origin review](../.analysis/origin-review/NOTES.md).

`AnmSpriteViewport::Set2DView` (`0x00435090`, 343B / 17 target fields) now
has source in `AnmViewport.cpp`. The shared view names its matrices at
`+0x4C/+0x8C`, SDK viewport at `+0xCC`, and view offsets at `+0xE8/+0xEC`.
It flushes a nonnull manager, converts unsigned width/height to float,
builds a centered camera with Z = half-height / tan(pi/20), target Z = 0,
and up = (0, -1, 0). Projection uses FoV pi/10, width/height aspect, near 1
and far 10000. The two D3DX9_42 imports and D3D SetTransform slots are
reviewed. After those calls it snapshots the manager and copies both view
offsets, retaining that pointer across the two stores.
[Complete camera and presentation review](../.analysis/frame-present-and-viewport-review.txt),
[camera source](../src/AnmViewport.cpp).

The maintained camera remains 339/343 bytes with all 17 fields. Component
assignments instead of vector aggregates produce the same candidate. A
separate dimensions-vector probe emits 343 bytes but still has ordinary
differences; that variant was not adopted. Float temporary lifetimes,
aspect division scheduling, local vectors and the GS frame remain open.
The real frame context also leaves ResetBuffers at 56 bytes / 23 ordinary
differences; adding real callers alone did not close it.
[Maintained camera](../.analysis/frame-cached-manager-anm-viewport-set-2d.json),
[component probe](../.analysis/frame-components-anm-viewport-set-2d.json),
[dimensions probe](../.analysis/frame-dimensions-anm-viewport-set-2d.json),
[reset in real frame context](../.analysis/frame-cached-manager-anm-draw-reset-buffers.json).

The three presentation functions at `0x00454940`, `0x00454D90` and
`0x00454F70` are reviewed real external boundaries, not fabricated bodies.
They handle adaptive raster/sleep waits, Present and device-loss recovery;
`0x004550B0` is screenshot handling, not a timing-statistics update. Their
private receivers and the full pacing bodies are the next useful source
context. The 653-byte adaptive present owner includes a three-byte physical
alignment hole inside the reviewed extent; a future comparison must retain it.
[Presentation and screenshot reviews](../.analysis/frame-timing-data-review.txt),
[full adaptive present](../.analysis/frame-present-and-viewport-review.txt).

The next device-loss dependencies are bounded. `0x004536A0` (124B) walks
all 38 loaded ANM slots, releases nonnull texture pointers whose entry
flags contain bit 0, and clears each released pointer. `0x00453720` (165B)
walks the same slots and recreates those entries from the Supervisor
back-buffer dimensions/format, recording four bytes per pixel for format
22 and two otherwise. The latter contains the same work as the existing
exact CreateRenderTarget owner, with no call to that owner in the target.
`0x00435D20` (217B) reacquires level-zero surfaces and binds three real VM
scripts through the exact BindVmScript boundary. The conditional
post-Present hooks at `0x0041F990/0x0040DA50` sample the clock and update
counters; they are separate from these reset helpers.
[Dependency target review](../.analysis/presentation-loss-leaf-review.txt).
The 802-byte D3D state setup at `0x00455A70` finishes by invalidating the
manager's texture/format caches; this is separate from per-frame buffer
reset. Its first 250 instructions are in that review and its complete
remaining tail is [reviewed separately](../.analysis/presentation-state-reset-tail.txt).

The final current-input comparison covers 116 owners: all 97 affected
existing exact owners pass, the excluded tangent wrapper adds one pass,
and 18 frontiers are rejected. Eleven affected contexts were cold-built;
the final-input AnmViewport cold context was reused without source changes
and its current receipt passed the Oracle again. Canonical totals are
**407 source mappings / 331 exact functions / 20,204 exact bytes**. The four
new authored mappings remain frontiers; authored exact totals are unchanged
at **322 owners / 19,844 bytes**.
[Full results](../.analysis/frame-current-results.json),
[replay log](../.analysis/frame-current-replay.log),
[existing exact selection](../.analysis/frame-current-selected.json),
[affected contexts](../.analysis/frame-current-contexts.json).

## Render-target release and recreation (2026-11-26)

The original Japanese TH12.8 v1.00a device-loss ANM passes now reproduce
both complete VC10 RTM `/O2 /Ob1 /Oy- /GL` owners: ReleaseAnmRenderTargets
at `0x004536A0` (124B) and RecreateAnmRenderTargets at `0x00453720` (165B).
They add **two authored exact functions / 289 bytes** in
`SupervisorDevice.cpp`. Each has four reviewed main-loop/presentation
callers, no interior references or owned tail table. The complete comparison
includes loop alignment and all 1/6 DIR32 fields.
[Target review](../.analysis/presentation-loss-leaf-review.txt),
[complete comparisons](../.analysis/device-target-final-results.json),
[source](../src/SupervisorDevice.cpp).

Both walk exactly 38 loaded ANM slots and use signed texture counts.
Release skips null resources and unflagged/null texture pointers. After
the COM Release call it rereads the resource slot and texture array before
clearing the pointer; caching an entry across that call would change the
observed access contract. Recreate processes every entry with flag bit 0,
sets that bit again, and calls SDK CreateTexture with current back-buffer
width/height/format, one level, render-target usage and default pool. It
ignores HRESULT and rereads format after the call: format 22 records four
bytes per pixel, all others record two. It does not clear the render-target
flag during release or test the existing texture pointer during recreation.

In this pinned context, the first natural source emitted 130/179 bytes.
Advancing the slot **before** decrementing the remaining count recovers the
target's memory-decrement loop tail. Direct slot/array expressions also
recover the release owner's COM input and post-call reloads. Recreation
with a cached entry remained 173 bytes even after factoring the real texture
creation into an inline helper. Testing the slot expression directly and
passing its entry address to that helper recovers the separate byte test,
DWORD flag update, private registers and complete 165-byte owner. The
compiler profile stayed fixed throughout these source probes.
[First candidates](../.analysis/device-target-first-supervisor-recreate-anm-render-targets.json),
[cached-entry candidate](../.analysis/device-target-inline-supervisor-recreate-anm-render-targets.json),
[final recreation](../.analysis/device-target-direct-access-supervisor-recreate-anm-render-targets.json).

The existing standalone CreateRenderTarget owner and all shared headers
are unchanged. The new device TU, header and probe do not change any
existing exact unit's inputs, so no existing exact unit needs rebuilding. Both new owners
pass from the final cold context. Canonical totals are **409 source mappings /
333 exact functions / 20,493 exact bytes**, including **324 authored exact
functions / 20,133 authored bytes**.
[Affected scope](../.analysis/device-target-final-scope.json),
[cold build](../.analysis/device-target-direct-access-link.log).

## Surface restoration and maintained Present paths (2026-11-26)

Original Japanese TH12.8 v1.00a `RestoreSupervisorSurfaces` at `0x00435D20`
now reproduces its complete **217-byte** pinned VC10 RTM `/O2 /Ob1 /Oy- /GL`
owner and three real BindVmScript REL32 fields, adding one authored exact
function. The first natural source matched, including the `0x118`-byte
runtime state copy and RET 4. Its linked context contains the maintained
BindVmScript body; the source does not simulate its register effects.
[Complete target](../.analysis/presentation-loss-leaf-review.txt),
[complete linked results](../.analysis/surfaces-present-preserved-timer-final-results.json),
[source](../src/SupervisorSurfaces.cpp).

The receiver is the graphics view rooted at Supervisor `+8`. If its first
surface is nonnull, the entire function returns. Otherwise it gets texture
entry 2's level-zero surface and binds the first VM to script `0x52`.
**After that binding**, it tests the first surface pointer again; only when
still null does it copy the viewport-state block. It then gets texture
entry 3's level-zero surface, binds the other VMs to `0x55` and `0x52`, and
calls SDK **GetBackBuffer(0, 0, MONO, ...)**. HRESULTs are ignored. There is
no independent retry/validation of each output surface.

| Graphics-view field | Offset from Supervisor `+8` |
| --- | --- |
| Device | `+0x008` |
| Texture surfaces / back buffer | `+0x1AC/+0x1B0/+0x1B4` |
| Three VM pointers | `+0x1BC/+0x1C0/+0x1C4` |
| Viewport copy source / fallback destination | `+0x204/+0x434`, each `0x118` bytes |
| Resource | `+0x588` |

The middle range at `+0x31C` begins at the default 2D camera's
`0x004D3184` root. Only the copied extent is settled here; the complete
viewport class remains opaque beyond its reviewed fields. The fifth
caller at `0x004334B0` is flag-controlled and also reenables four callback
records after restoring surfaces.
[Wrapper review](../.analysis/surface-restore-wrapper-review.txt),
[partial view](../src/SupervisorSurfaces.hpp).

`SupervisorPresent.cpp` now maintains Immediate and WithSleep at
`0x00454D90` (190B / 21 target fields) and `0x00454F70` (314B / 30 fields).
Both perform screenshot handling and Present; negative HRESULT releases
three held surfaces, clears the first slot again, releases ANM render
targets, calls Reset with the current presentation parameters, recreates
targets, restores surfaces/VMs, initializes render state and writes mode 2.
The two optional timing/replay hooks remain reviewed external boundaries.
WithSleep additionally samples the clock, conditionally sleeps a positive
integer budget `(lastPresent + 1/60 - now)*1000 - 3.5`, and records the two
presentation timestamps. The branch requires config byte `0x004D3053 == 1`
and elapsed time strictly between zero and 1/60.
[Present target review](../.analysis/frame-present-and-viewport-review.txt),
[maintained source](../src/SupervisorPresent.cpp).

Render-state setup at `0x00455A70` has no live incoming argument: its
initial PUSH ECX reserves a local slot, overwritten by a float before its
first read. Screenshot handling at `0x004550B0` receives the frame in ECX
and is now declared as a frame member. Neither boundary has a fabricated
body. All existing frame-entry candidate contexts still omit the new
Present definitions; integrate the real bodies before revisiting their
private caller ABI.

The maintained Present owners remain frontiers at **189/190** and
**323/314** bytes. Immediate shares a popped register across the last
conditional branch where the target has branch-specific pops. Explicit
void-tail returns and an early null guard emit the same layout. WithSleep
has an extra caller alignment/frame setup. Its timer support owner initially
shrank to 259 bytes; the timer's address-taken probe and canonical profiles
restore 266 bytes but leave Present at 323. Removing the named double local
also leaves the output unchanged, so neither the named local nor the
timer's own missing alignment alone explains the caller frame.

A bounded mixed-boundary diagnostic kept the Present owner `/GL` and
compiled only SupervisorTime support as normal COFF. That removed the
extra caller frame and produced 313 bytes, but the timer itself was then
259 bytes and lost the target's independent alignment. **That hypothesis
was rejected and removed from the maintained context.** A closer caller
alone does not settle the callee ABI or original TU boundary. The final
context preserves the timer's canonical profiles and all-LTCG ownership;
neither Present receives exact credit. No Present owner flags were varied.
[First timer frame](../.analysis/present-first-time-frame.txt),
[all-LTCG timer frame](../.analysis/present-known-timer-time-frame.txt),
[rejected mixed timer frame](../.analysis/present-mixed-current-time-frame.txt),
[final retained timer frame](../.analysis/present-preserved-timer-final-time-frame.txt),
[canonical timer settings reused](../.analysis/present-canonical-timer-profile-inputs.json),
[direct-condition negative](../.analysis/present-direct-time-supervisor-present-with-sleep.json),
[mixed-boundary settings](../.analysis/present-mixed-timer-context.json),
[rejected mixed Present observations](../.analysis/present-mixed-current-supervisor-present-with-sleep.json).

Final comparison covers the three new owners and the one existing exact
unit affected by the frame header, AnmViewportTan. Surface restoration and
the existing wrapper pass; both Present owners fail the complete-size gate.
Only those three contexts were cold-built, with current receipts reused
when inputs remained unchanged. Canonical totals are **412 source mappings /
334 exact functions / 20,710 exact bytes**, including **325 authored exact
functions / 20,350 authored bytes**.
[Affected scope](../.analysis/surfaces-present-preserved-timer-final-scope.json),
[final results](../.analysis/surfaces-present-preserved-timer-final-results.json).

## Initial D3D9 state and ANM cache reset (2026-11-26)

Original Japanese TH12.8 v1.00a `InitializeSupervisorRenderState` at
`0x00455A70` now matches its complete **802-byte** VC10 RTM
`/O2 /Ob1 /Oy- /GL` owner and **37 DIR32 fields**. The first natural C++
implementation passed without changing the owner profile. It contains 20
SetRenderState calls, eight stage-zero SetTextureStageState calls, six
sampler-zero SetSamplerState calls and a conditional ANM cache reset.
HRESULTs are ignored. There are five direct callers, no interior references,
no owned tables, and the return is at `0x00455D91`.
[Target review](../.analysis/presentation-loss-leaf-review.txt),
[complete final tail](../.analysis/presentation-state-reset-tail.txt),
[complete exact comparison](../.analysis/state-reset-first-canonical.json),
[source](../src/SupervisorRenderState.cpp).

The setup enables Z buffering, alpha blending/testing and vertex fog,
disables lighting, culling and multisample antialiasing, uses Gouraud
shading, SRCALPHA/INVSRCALPHA additive blending, ALWAYS Z comparison and
GREATEREQUAL alpha comparison with reference 1. Fog is linear, colored
`0xFFA0A0A0`, with density 1, start 1000 and end 5000. Both texture color
and alpha modulate TEXTURE by TFACTOR. The texture transform uses COUNT2
and coordinate set zero. Magnification/minification use LINEAR, mip
filtering is NONE, U/V wrap and W clamps.

The initial PUSH ECX reserves one local DWORD; it does not preserve a live
input argument. `FloatState(float)` naturally generates the observed
FLD/FSTP bit reinterpretation through that slot for the three fog values,
including FLD1 for density. The binary32 start/end constants at
`0x004A66CC/0x004A66C8` are 1000/5000. The helper is inlined under the fixed
owner profile, so no fake parameter or conversion body is necessary.
[Attested constant bytes](../.analysis/state-reset-fog-bytes.txt),
[first complete disassembly comparison](../.analysis/state-reset-first-diff.txt).

When the global manager exists, the function sets byte `+0x4E968C` to 7,
bytes `+0x4E968D/+0x4E968E/+0x4E9690` to `0xFF`, and pointer `+0x4E9688`
to null. Separate adjacent byte assignments naturally combine into the
target's word store `0xFF07`. Only the blend/vertex-format/texture cache
roles are currently named; the other two byte meanings remain unknown.
The source uses a partial view instead of expanding the shared manager
layout. No existing exact context consumes the changed inputs.
[Affected scope](../.analysis/state-reset-final-scope.json).

Both Present contexts now link this real initializer. They still produce
189/190 and 323/314 bytes, so completing this callee does not close their
branch/pop and alignment differences. Their strict comparisons reject
both complete extents. The support initializer remains 802 bytes and the
timer 266 bytes; the surface/texture recovery support owners retain their
observed complete lengths and ordinary bytes. These support observations
grant no additional exact credit. Only two affected contexts were built.
Canonical totals are **413 source mappings / 335 exact functions / 21,512
exact bytes**, including **326 authored exact functions / 21,152 bytes**.
[Final three-owner comparisons](../.analysis/state-reset-final-results.json),
[Present observations](../.analysis/present-state-reset-final-supervisor-present-with-sleep.json),
[support ownership](../.analysis/state-reset-present-support-owners.json),
[Present cold build](../.analysis/state-reset-present-link.log).

## Screenshot capture, writer and pending surface copies (2026-11-26)

Original Japanese TH12.8 v1.00a now has complete VC10 RTM
`/O2 /Ob1 /Oy- /GL` matches for **CaptureSupervisorScreenshot** at
`0x004343D0` (**520B / 39 fields**) and **WriteSupervisorScreenshot** at
`0x00434270` (**352B / 34 fields**). Both are primary-TU owners in the
maintained screenshot context; all bytes and absolute/relative identities
pass the canonical comparator. The capture's source cdecl filename
parameter naturally becomes the target's private EAX receiver. The writer
uses the legal cdecl CRT thread-callback signature with an unused pointer;
the target does not read an incoming argument.
[Capture comparison](../.analysis/screenshot-capture-final-canonical.json),
[writer comparison](../.analysis/screenshot-writer-final-canonical.json),
[source](../src/SupervisorScreenshot.cpp),
[complete target leaves](../.analysis/screenshot-leaves-review.txt),
[complete writer target](../.analysis/screenshot-worker-review.txt).

Screenshot state begins at Supervisor `+0x6D8`: worker handle `+0`, packed
14-byte BITMAPFILEHEADER `+4`, BITMAPINFO pointer `+0x14`, pixel pointer
`+0x18`, filename `+0x1C`. Capture waits in Sleep(10) while the worker slot
is nonzero, obtains the mono back buffer, prepares the BMP file header,
and copies the filename. Format 22 (X8R8G8B8) allocates a 44-byte BITMAPINFO
and 921,600 pixel bytes. It writes a 640x480, 24-bit BI_RGB header, locks
the surface and copies BGR triples from rows 479 down to 0, using the
source pitch and a 1,920-byte destination row stride. It unlocks, starts
the actual writer via `_beginthread`, stores its return and releases the
surface. The separate empty logger at `0x004360F0` remains a reviewed
varargs boundary, without a fabricated body or new exact credit.
[Logger target](../.analysis/screenshot-logger-review.txt),
[attested capture strings](../.analysis/screenshot-capture-data.txt),
[remaining error text](../.analysis/screenshot-error-data.txt).

The R5G6B5 path logs refusal, releases the surface and returns zero; other
unsupported formats log and return one before surface release. A failed
pixel allocation leaves the info block allocated. Back-buffer and
LockRect HRESULTs are ignored. The writer ignores all stream results,
writes 14 header bytes, 40 info-header bytes and the pixels, conditionally
frees/clears both allocations and clears the worker slot. The maintained
FileSystem bodies naturally inline to the writer's complete 352-byte
target, including the original stream error paths. These retained failure
behaviors are reconstruction findings, not repairs or runtime validation.

The first capture was 517B. Moving row decrement before destination-offset
increment and using the observed `y > -1` bound restored the target's
520B loop. Its last four ordinary differences were the order of two
pointer additions; initializing the destination pointer first closed them.
Only these natural source expressions changed; owner profiles stayed fixed.
The private relink predicted the result, then a canonical cold build
accepted the full owner and all 39 fields.
[First capture](../.analysis/screenshot-first-supervisor-capture-screenshot.json),
[full-size four-byte mismatch](../.analysis/screenshot-capture-real-fs-canonical.json),
[isolated pointer-order diagnostic](../.analysis/screenshot-destination-first-supervisor-capture-screenshot.json).

The 168-byte screenshot handler now has source. It always flushes pending
surface copies, then tests InputState zero's trigger mask bit `0x40000`.
It changes to the global directory at `0x004D3999`, creates `snapshot`,
formats `"%ssnapshot\\th%.3d.bmp"` using the receiver's string at `+0x29`,
and scans indices 000..999 through FileExists. It captures only when a name
is free and restores the global directory at `0x004D4999`, including when
all names exist. Global directory pointers and the dynamic receiver string
are distinct source accesses.
[Handler target](../.analysis/frame-timing-data-review.txt),
[literal strings](../.analysis/screenshot-text-data.txt).

`FlushAnmSurfaceCopies` at `0x004537D0` is a four-entry copy queue, not input
polling. The manager's first `0xA0` bytes are four `0x28`-byte records:
ANM slot, texture index, **source** x/y/width/height, then **destination**
x/y/width/height. The D3DX call's argument order establishes the rectangle
roles; an initial source draft had them reversed and was corrected.
Negative slots are skipped. Active slots with a texture flush sprites,
obtain the mono back buffer and destination level-zero surface, copy with
filter 2, dirty the texture only on D3D_OK, release acquired surfaces and
consume the slot by writing -1, including failed attempts. The queue
assumes that active resource slots are valid.
[Copy source](../src/SupervisorSurfaceCopies.cpp),
[complete copy target](../.analysis/screenshot-leaves-review.txt).

The handler and copy owners remain frontiers at **301/168** and **369/371**
bytes. FileExists is inlined into the handler, which also has an extra
aligned frame. Reusing the canonical FileSystem/GameErrorContext profiles
and existing FsErrUtil call probe did not close it. Original TU ownership
is not established by these builds; preserve the fixed owner profiles
and inspect real call/ABI evidence before changing a boundary. The copy
owner differs in its initial signed-slot load/test and local/control-flow
layout. Neither owner is cropped or granted exact credit.
[Final copy-context observations](../.analysis/screenshot-final-copies-supervisor-flush-surface-copies.json),
[unchanged handler after Fs context reuse](../.analysis/screenshot-real-fs-supervisor-screenshot-handler.json).

Both Present contexts now link the actual screenshot/copy/FileSystem
bodies and still produce 189/190 and 323/314 bytes. Their timer and state
initializer support owners remain 266B and 802B. Final strict comparison
covers all four new owners, both Present consumers and the existing
AnmViewportTan exact affected by the header comment. Only capture, writer
and that existing compiler/header wrapper pass. The affected scope spans
four contexts; unchanged current receipts were reused after their cold
builds. Totals are **417 source mappings / 337 exact functions / 22,384
bytes**, including **328 authored exact functions / 22,024 authored bytes**.
[Seven-owner results](../.analysis/screenshot-final-results.json),
[affected scope](../.analysis/screenshot-final-scope.json),
[Present support ownership](../.analysis/screenshot-final-present-support-owners.json),
[final cold build](../.analysis/screenshot-final-link.log).

## Real frame/Present closure and adaptive pacing (2026-11-26)

Original Japanese TH12.8 v1.00a **PresentImmediate** (`0x00454D90`,
**190B / 21 fields**) and **PresentWithSleep** (`0x00454F70`,
**314B / 30 fields**) now pass complete pinned VC10 RTM
`/O2 /Ob1 /Oy- /GL` primary-TU comparisons. The maintained frame entries,
camera, screenshots, ANM surface/texture recovery and state initialization
are all present in the linked family. This closes the former 189/190 and
323/314 frontiers with genuine callees and callers, preserving GetElapsedSeconds'
independent 266-byte body. No Present owner flags or ABI declarations changed.
[Twelve-owner final comparison](../.analysis/adaptive-family-final-results.json),
[retained source family/profiles](../.analysis/adaptive-present-family.json),
[source](../src/SupervisorPresent.cpp).

Frame, viewport and Present contexts now use the same 35 maintained source
inputs, with their own primary TU and entry. Duplicate probe-owned globals
were removed from the configured source lists. The existing frame probe
supplies the QPC globals/time origin and exposes the real timer's address;
the ANM/load probes retain the Supervisor/manager/error-context definitions.
Timer, FileSystem and GameErrorContext support keep their canonical profiles;
the release logger stays normal COFF. The prior timer normal-COFF diagnostic
was not reused.

`PresentAdaptive` (`0x00454940`) now has natural source for the complete
**653-byte** owner. It estimates `(lastPresent + 1/60 - current)*1000 - 2.5`,
lowers a profile's sleep immediately when the estimate falls, and otherwise
increments its stable-frame counter. After 15 stable frames, sleep can rise
by one up to the profile maximum; the counter resets. Negative estimates
are clamped to zero. Clock rollback resets the raster timestamp. An interval
below **1/59 seconds** enters Sleep(1) polling until 13ms, then GetRasterStatus
polling until scan-line wrap, line zero, vertical blank or nonzero HRESULT.
A longer/unordered interval lowers a positive profile sleep by one. The
function records the raster time, performs real screenshot/Present/recovery,
runs optional hooks, takes the target's discarded clock sample, sleeps the
positive profile budget and records last-present time.
[Complete physical span](../.analysis/adaptive-present-complete-span.txt),
[attested constants](../.analysis/frame-timing-constants.txt).

The frame's estimate/index are `+0x2074/+0x2078`. Four 12-byte profile
records begin at `+0x207C`, each maximum/current milliseconds/stable count.
CreateGameWindow initializes them to `(15,15,0)`, `(12,12,0)`, `(12,12,0)`,
`(8,8,0)`, then selects index zero. This identifies the old opaque WindowState
fields at `+0x44..+0x70`; it does not assign scene names to the profiles.
[Initialization](../.analysis/adaptive-profile-window-create.txt),
[index references](../.analysis/adaptive-profiles-xrefs.txt),
[partial frame layout](../src/SupervisorFrame.hpp).

The first adaptive source was 655B. Placing GetRasterStatus in the loop
condition reproduces the target's rotation and its three-byte LEA alignment
at `+333`, yielding 653B. All 39 address fields replay, but ordinary bytes
**+90/+92** still differ: the saturation result stays in EAX instead of EDX.
Both normal Windows max argument orders were tested in isolated links;
`max(0, estimate)` leaves those two differences, and the other order adds
one. Neither was adopted. Reversing the equivalent frame deadline comparison
operands also emits unchanged code. Adaptive receives no exact credit.
[Complete two-byte rejection](../.analysis/adaptive-current-canonical.json),
[max-zero negative](../.analysis/adaptive-max-zero-supervisor-present-adaptive.json),
[max-value negative](../.analysis/adaptive-max-value-supervisor-present-adaptive.json),
[frame-expression negative](../.analysis/adaptive-frame-order-observations.txt).

Primary-object placement still changes LTCG observations despite the same
source family and fixed profiles. The Present-primary image has WithSleep
314B and tangent support 21B; Frame/viewport-primary images have WithSleep
support 323B and tangent 31B. The existing tangent's own 31B canonical owner
passes. Frame owners remain 360/453/288B versus 360/443/288B targets; the
adaptive entry fails exhaustive field layout and vsync has 12 ordinary-byte
differences in its two early-return epilogues. Camera remains 339/343B.
These results establish per-unit matches, not a single closed native image;
original TU/link order remains a concrete reconstruction question.
[Present-primary observations](../.analysis/adaptive-present-final-observations.txt),
[Frame-primary observations](../.analysis/adaptive-frame-current-observations.txt),
[viewport-primary observations](../.analysis/adaptive-viewport-current-observations.txt).

The changed frame header affects three existing exact owners: capture,
writer and tangent. All pass their complete current-input canonical replay;
the screenshot handler and copy consumer remain frontiers. The bounded
affected scope covers twelve owners in five contexts. Totals are **418
source mappings / 339 exact functions / 22,888 exact bytes**, including
**330 authored exact functions / 22,528 authored bytes**.
[Affected scope](../.analysis/adaptive-family-final-scope.json),
[cold family build](../.analysis/adaptive-family-current-link.log),
[copy consumer build](../.analysis/adaptive-copy-consumer-link.log).

## Post-Present frame accounting and replay pause timing (2026-11-26)

This section records checkpoint `59848a9`. The later CardInf finding below
supersedes the `ReplayPauseState` type hypothesis and the 389B frontier;
the historical compiler results are retained unchanged.

Original Japanese TH12.8 v1.00a `UpdatePostPresentTiming` (`0x0041F990`,
**256B / 8 address fields**) and `ReplayPauseState::HasInvalidDurationCode`
(`0x0040D340`, **81B / no address fields**) pass complete pinned VC10 RTM
`/O2 /Ob1 /Oy- /GL` primary-TU comparisons. The real replay callback at
`0x0040DA50` also has maintained source, but remains a frontier at
**389/382B**. No owner flags or ABI shims were introduced.
[Timing comparison](../.analysis/post-present-timing-final-canonical.json),
[checksum comparison](../.analysis/post-present-code-final-canonical.json),
[source](../src/PostPresentUpdates.cpp),
[partial layouts](../src/PostPresentUpdates.hpp).

All three Present owners call both callbacks conditionally through the
globals `0x004B894C` (frame accounting) and `0x004B8934` (replay). The
callbacks read those globals themselves and take no stack arguments.
The complete timing/replay spans have no external interior entries or owned
tables; the checksum's only caller is the replay callback.
[Attested complete owners and references](../.analysis/post-present-hooks-review.txt),
[checksum boundary and body](../.analysis/post-present-hook-related-review.txt).

The frame-accounting object's doubles are aligned to four-byte member
boundaries: window start `+0x14`, consecutive over-65 windows `+0x1C`,
unsigned frame count `+0x20`, effective/nominal frame totals `+0x24/+0x2C`,
and float FPS `+0x34`. Clock rollback resets the window start. After at
least one second, the callback advances that start by the measured interval,
converts the **unsigned** count to float FPS, and increments or resets the
over-65 counter. If the gameplay object at `0x004B8954` exists and its
`+0x60` flags lack `0x14`, nominal frames increase by 60 and effective
frames increase by 60 for rates strictly above 57, otherwise by the
float-rounded rate. The gameplay flag `0x80` is cleared whenever that
object exists at window completion. The count resets and then increases by
`g_Supervisor.configFlag304E + 1`; it includes configured frame skips and
is not simply the number of physical Presents. The callback returns 1.
[Constants, including 2^32 for the unsigned conversion](../.analysis/post-present-hook-constants.txt).

The replay object's observed fields are flags `+0x7C`, sample index
`+0x8C`, current/previous markers `+0x90/+0x94`, pause start `+0x98`,
temporary duration `+0xA0` and encoded duration `+0xA8`. Flag bit 0 starts
pause timing once, guarded by bit `0x80`. On resume, the callback copies
the marker, subtracts the saved clock and quantizes to the target's decimal
**0.0167-second** period using `fmod`. A remainder **greater than or equal
to 0.00835** rounds upward; the assembly's `TEST AH,0x41` establishes the
tie case. These constants are not exact 1/60 and 1/120. Whole seconds come
from `floor`, capped at 999 only on the upper side; hundredths come from
the remaining fraction times 100, truncated to int. The temporary duration
is cleared and bit `0x80` is reset before record/playback.

For seconds `s` and hundredths `h`, the code is:

```cpp
((s + h + 22) * 1000 + (s + 66) % 1000) * 100 + (h + 33) % 100
```

The integer checksum rejects a code `c` when
`c/100000 - 22 != ((c/100)%1000 + 934)%1000 + (c%100 + 67)%100`.
All divisions and remainders are signed. Natural C++ emits the target's
IDIVs and optimized `/100000`; no handwritten reciprocal multiply is
needed. The body zeroes EAX before `SETNZ AL`, and its caller tests all
of EAX. A first `bool` declaration reproduced the leaf but incorrectly
made the caller test AL. The accepted interface returns `int`; LTCG
naturally passes its receiver in EAX.

The gameplay object's `+0x74 == 0` selects recording. The storage global
at `0x004B8A84` contains record stream pointers at `+0x20 + stage*4`, and
playback stream pointers at `+0x148 + stage*36`; stage is the int at
`0x004B4D14`. Samples are ints at stream `+0x30 + index*4`. Playback
replaces a failed checksum with **0x06AD1584**, the encoded **999.99-second**
fallback. Both paths advance the sample index once. Source preserves the
target's pointer assumptions and CRT call identities (`_CIfmod`, `floor`,
`__ftol2_sse`); this is not runtime validation.

The first replay source was 402B. Direct branch-local table accesses yielded
395B and duplicated a global address field. Sharing the storage pointer
before the mode branch yields 389B / 16 fields. Its arithmetic register
assignment now agrees with the target, but the table/index load scheduling
and index caching across the checksum call still differ. A version caching
the index, encoded value and storage together happened to be 382B while
failing exhaustive field placement and ordinary bytes; equal size was not
accepted. Do not add clobbers or pretend that the pure checksum mutates the
index to force a reload.
[Direct-table observation](../.analysis/post-present-direct-tables-post-present-replay-update.json),
[common-storage observation](../.analysis/post-present-common-storage-post-present-replay-update.json),
[389B rejection](../.analysis/post-present-replay-common-storage-canonical.err),
[382B rejection](../.analysis/post-present-replay-caller-types-canonical.err).

The new primary context uses the existing real timer and timer probe, with
their canonical no-`/Ob1` support profile. Its small entry calls the two
real callbacks. Undefined external state uses the existing Oracle's distinct
normal-COFF data anchors in a non-runnable diagnostic image. The actual
callback source is also added to the three frame/viewport/Present contexts,
which now share 36 source inputs. These remain per-unit comparisons and
do not establish a runnable native product or the original TU partition.

The final bounded replay covers **eleven owners in four cold contexts**:
the two new exact owners plus the existing immediate/sleep Present and
viewport tangent all pass. The other six owners remain rejected frontiers;
adaptive Present still differs only at ordinary bytes +90/+92. No shared
frame header changed, so screenshot capture/writer contexts are unaffected.
The ledgers now contain **421 source mappings / 341 exact functions /
23,225 exact bytes**, including **332 authored exact functions / 22,865
authored bytes**. The net exact gain is two functions / 337 bytes.
[Complete final results](../.analysis/post-present-final-results.json),
[affected scope](../.analysis/post-present-final-scope.json),
[three-context cold build](../.analysis/post-present-integrated-link.log).

## CardInf identity and exact active-duration update (2026-11-26)

The singleton at **0x004B8934 is CardInf** in original Japanese TH12.8
v1.00a. The loader at `0x0040D540` requests `card.anm` (`0x004A216C`),
and nearby target strings name `shutdown CardInf` (`0x004A21A8`) and
`src\game\card.cpp` (`0x004A21BC`). Its factory at `0x0040D7B0`
allocates **0xC0 bytes** and stores the singleton. The earlier generic
`ReplayPauseState` name described only the pause/replay fields and is
superseded; it did not identify the owning game module.
[Attested literal bytes](../.analysis/replay-pause-resource-strings.txt),
[singleton references](../.analysis/replay-pause-stage-storage-xrefs.txt),
[lifecycle owners](../.analysis/replay-pause-lifecycle-review.txt),
[current layout](../src/CardInf.hpp).

The earlier **pause** interpretation is also superseded. The card-start
path `0x0040DD70` copies the card name and sets flag `+0x7C & 1`;
`0x0040E350` clears that bit and stops the card VMs. Update `0x0040D850`
runs while it is set, advances `+0x90`, updates the timer and recalculates
the displayed bonus. The measured interval is CardInf activity. Legacy
`pause*` member names and the old flag comment remain working labels in
this checkpoint; rename them with the next lifecycle source edit. They
are not evidence of a game-pause state. The unimplemented update also
exposes a VM ID at `+0x20` and three floats at `+0xAC/+0xB0/+0xB4`.
[Card start/end paths](../.analysis/replay-pause-lifecycle-review.txt),
[complete 507B active update](../.analysis/card-update-active-interval-review.txt).

The post-Present active-duration callback `UpdatePostPresentReplay`
(`0x0040DA50`) now passes its complete **382-byte owner and all 16 address
fields** under the unchanged pinned VC10 RTM `/O2 /Ob1 /Oy- /GL` primary
profile. `CardInf::EnableChains` (`0x0040D680`) is newly reconstructed and
exact at **35B / one global field**. It ORs flag 2 into each existing
calc/draw ChainElem, with no additional singleton guard. Both callsites
in `0x004269D0` overwrite EAX without consuming a result, supporting the
void declaration. [Callsite ABI evidence](../.analysis/card-enable-call-sites.txt).
The prior FPS
256B/8-field and renamed integer-checksum 81B/0-field owners also replay.
[Pause update](../.analysis/card-pause-update-first-canonical.json),
[chain activation](../.analysis/card-enable-first-canonical.json),
[renamed checksum](../.analysis/card-code-first-canonical.json),
[FPS](../.analysis/card-fps-first-canonical.json).

The duration update closes through ordinary C++ control flow: read the sample
index once, use direct record/playback table accesses in the two branches,
and increment the object's index once after their join. No clobbers,
volatile accesses, register hints or owner profile changes are needed.
The compiler naturally shares the storage-global load after the mode test,
keeps the common index in ECX only while needed, and emits an in-memory
increment after the checksum call. All CRT and checksum call identities
remain intact.
[Source](../src/PostPresentUpdates.cpp),
[matching isolated observation](../.analysis/post-present-shared-index-direct-post-present-replay-update.json).

The bounded intermediate probes explain why isolated edits did not close
the old source. Unifying the increment alone yields **388B** and fixes
the increment instruction. Adding a shared index while retaining the
pre-branch storage local yields **382B**, but hoists the gameplay-global
load before the integer divisions and fails ordinary bytes and field
placement. Removing that storage local lets the branch accesses be shared
naturally and closes the complete target. These are different observations
from the earlier 382B version that cached the encoded value as well; equal
length alone was never accepted. Private candidate links reused attested
support objects for diagnostics; acceptance used fresh canonical cold builds.
[Unified increment](../.analysis/post-present-unified-advance-post-present-replay-update.json),
[shared-index negative](../.analysis/post-present-shared-index-post-present-replay-update.json),
[cold build](../.analysis/card-pause-first-canonical-link.log).

Lifecycle evidence extends the layout without claiming those functions as
reconstructed source. Calc/draw elements are at `+0x08/+0x0C`, three VM
IDs at `+0x14/+0x18/+0x1C`, a ZunTimer at `+0x24`, pause fields remain
at `+0x7C..+0xA8`, and the ANM resource pointer is at `+0xB8`.
The constructor path first clears the timer initialization bit, then
zeroes all 0xC0 bytes, sets lifecycle flag 2 and publishes the singleton.
Initialization loads ANM slot **37**, creates initially disabled calc/draw
elements at priorities **26 / 10**, and resets the timer to current 0,
previous -1. It installs callback wrappers `0x0040DD50/0x0040DD60`, which
bridge the real Chain callback receiver in ECX to the update/draw bodies'
private EDI/EBX receivers.
[Initialization](../.analysis/replay-pause-registration-review.txt),
[wrapper spans](../.analysis/card-lifecycle-and-callback-spans.txt).

The 249B shutdown owner `0x0040D6B0` releases/deletes ANM slot 37,
deletes the three VMs, cuts both ChainElems under Supervisor critical
section 0 and clears the singleton. The 94B factory calls initialization
and invokes shutdown/delete on a nonzero result. The 29B global destroy
wrapper at `0x0040D830` guards the singleton before shutdown/delete.
These three and the 285B initializer are the next source scopes. The
unreferenced 19B delete-and-return-pointer body at `0x0040D810` remains
unclassified at source level; do not assume its compiler-generated origin
from its shape alone.
[Shutdown and factory](../.analysis/replay-pause-lifecycle-review.txt),
[complete undeclared wrapper bytes](../.analysis/replay-pause-delete-wrapper-span.txt).

Final replay covers **twelve owners in five cold contexts**: all seven
exact owners pass, including five retained matches and the two new ones.
The five other owners remain rejected frontiers; adaptive Present retains
its ordinary-byte differences at +90/+92. The ledgers now contain **422
source mappings / 343 exact functions / 23,642 exact bytes**, including
**334 authored exact functions / 23,282 authored bytes**. The net exact
gain is two functions / 417 bytes. Native product and runtime closure
remain unproved.
[Complete results](../.analysis/card-pause-final-results.json),
[affected sources, owners and profiles](../.analysis/card-pause-final-scope.json),
[family cold build](../.analysis/card-pause-family-cold-link.log).

## Open questions

- Per-unit flags, the TU partition, which objects were `/GL`, the library set
  and the link order. Both lanes in the handoff reproduce exact owners, so
  the split between normal and LTCG objects is still a hypothesis per file.
- Remaining LTCG register-allocation and stack-layout frontiers: why VC10
  leaves 4 unused bytes above `__security_cookie` in `ClearD3DTexture`
  (`0x00467BD0`), two extra 4-byte slots in `CreateGameWindow` (`0x00455320`),
  keeps values in memory across stores (`ZunTimer::Add`, `LaserInfiniteInf::Slot01`),
  and swaps `ECX`/`EDX` in `Chain::CutImpl` (`0x004686D0`).
