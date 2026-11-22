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

## Laser classes

`LaserDataInf` (vtable `0x004A2BB4`) is the base of `LaserLineInf`
(`0x004A2C1C`), `LaserCurveInf` (`0x004A2C84`), `LaserInfiniteInf`
(`0x004A2CEC`) and `LaserBeamInf` (`0x004A2D54`). It has 24 virtual slots
and no virtual destructor. Its defaults are exact as normal COFF; their
`RET n` sizes fix every slot's stack argument count (slot 8 takes five
arguments, slot 9 four, slot 10 six, slot 11 four). Slot 6 unlinks the laser
from a doubly linked list (prev +4, next +8). The defaults are laid out in a
different order from the slots (9, 10, 8, 11, 7 after slot 6), so the
original defines them in another order. Derived kinds override most slots
with 60–2,400-byte methods; slot 0 of each kind computes a point along the
laser through a polar helper (for example `0x00432E20`) that contains
`FSINCOS`, which VC10 never emits from C, so those helpers use inline
assembly in the original. `ZunVec3::SetPolarXY` reproduces `0x00432E20`
exactly from that inline assembly (`mov eax, this; fld angle; fsincos;
fmul radius; fstp [eax]; fmul radius; fstp [eax+4]`). It must be an
ordinary out-of-line definition, not `inline`: only then does VC10 LTCG
learn that it clobbers just `EAX`, so callers keep `ECX`/`EDX` live across
the call as the original does. That also explains the byte-identical copies
in other subsystems (separate per-file definitions). The four slot-0
methods then match except the x addition, where the original loads the
point first; operator, component, reference and `/fp:fast` variants all
load the position first.

`LaserInfiniteInf::Slot01` (`0x0042EEB0`) runs the laser's timed commands:
an 18-entry table of 0x2C-byte records at +0x61C indexed by +0x594 (type
+0x20, value +0x10, an `immediate` word +0x24 that bypasses the busy flag at
+0x598). The source reproduces every operation and the layout, but VC10
keeps the index in `ESI` across the mode-bitfield store, while the original
re-reads it (`INC [ecx+0x594]` in the bitfield arms, loop test from memory).
Per-case increments and `continue` arms compile to the same code as one
increment after the switch.

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
| `ltcg-entry-adapter-review` | 13 | **Open:** 10-byte `PUSH r; MOV r, ECX; CALL body; RET` callback entries; authored static callbacks or linker-made adapters |

Six tiny game vtable slots (`0x004144A0`, `0x004144B0`, `0x004144D0`,
`0x00414680`, `0x00429A00`, `0x00450FD0`) also match tiny library bodies by
chance and stay authored. The two open groups can be settled by cold VC10
compiles (the D3DX one needs the DirectX SDK headers).

## Pbg archive directory

The THA1 directory is a `new[]` array of 16-byte entries (name, offset,
size, reserved). A cold `/GL` probe with `new Entry[n]`/`delete[]`
reproduces the out-of-line entry constructor (`mov eax, ecx; mov [eax], 0;
ret`, uninventoried at `0x0044FD20`) and destructor (`0x0044FCB0`, exact)
byte for byte, because the vector iterators take their addresses.

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
  in `EBX`, chosen by LTCG. Its callee `ReadHeader` (`0x0044F9A0`) is not
  reconstructed yet; the probe defines a placeholder whose shape (several
  virtual stream calls and a local buffer) makes VC10 give it the original's
  private `ESI` receiver and stack filename. Simpler placeholders changed
  the call to `EDX`/stack-`this` and even Open's own convention, so keep the
  placeholder until the real reader replaces it.
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
shared by both loops.

`ZunTimer::Increment` (`0x00402AC0`, 39 call sites) is exact: inside the
speed window it does `current++; currentF += 1.0;` (a double constant),
otherwise `currentF += *gameSpeed; current = (int)currentF;`, which VC10
compiles with a temporary slot and converts from `ST0`.

`ZunTimer::Add` (`0x0046AA20`) is a close frontier: the game speed is
ignored inside `(0.99, 1.01)` (a double `0.99` and a float `1.01f`
compare), otherwise the step is scaled. VC10 keeps the sum in the argument
slot and converts from `ST0`, where the original stores `currentF`, reloads
it and converts; each branch carries its own conversion. With the timer
probe letting `gameSpeed` point at another timer's `currentF` (as the game
can), the scaled branch matches the original exactly; the unscaled branch
is still merged into the scaled tail because VC10 canonicalizes both
additions to the same operand order.

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

## Open questions

- Compiler flags per unit, the TU partition, which objects are `/GL`, the
  library set and link order.
- Whether one profile covers all game code, and how to reproduce the LTCG
  custom conventions (receiver in `EAX`/`ECX`) seen in small leaves.
