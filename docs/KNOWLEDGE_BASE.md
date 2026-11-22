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
convention bodies rather than linker artifacts.

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
