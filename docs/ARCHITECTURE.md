# Observed target and source map

Observations from the attested v1.00a target and the initial Ghidra 12.1.4
inventory. Subsystem names are provisional until their owners are reviewed.

## Image layout

| Section | Address range | Notes |
| --- | --- | --- |
| `.text` | `0x00401000..0x00499C14` | 625,685 bytes |
| `.rdata` | `0x0049A000..0x004AF72D` | 87,854 bytes; import tables, constants, RTTI |
| `.data` | `0x004B0000..0x004DCD87` | 183,688 bytes virtual, 31,744 bytes raw |
| `.rsrc` | `0x004DD000..0x004DE53F` | 5,440 bytes |

The game code occupies `.text` from `0x00401000` to about `0x004708BB`.
After it come the `DirectInput8Create` import stub (`0x004708BC`), about
6.7 KB of DirectInput data-format tables (`0x004708D0..0x0047230F`, most
likely the four VC9 SP1 objects in the Rich header, from `dinput8.lib`) and
110 import stubs (`0x00472310..0x004725A3`). The static CRT starts at
`0x004725A4`. The end of `.text` holds 57 exception-unwind funclets
(`0x00499040..0x00499940`), static initializers (`0x004999B0..0x00499B30`)
and atexit destructors (`0x00499B40..0x00499C14`).

A `std::string` instantiation cluster from the VC10 headers sits at
`0x00452D50..0x004534EA` inside the game band.

The initial inventory is far from complete in the game band: 977
uninventoried code spans (29.3 KB) remain, for example `0x00401000`,
`0x004010A0`, `0x00402B30`, `0x004091A0` and `0x00407320`.

## Runtime dependencies

Direct imports are `d3d9`, `d3dx9_42`, `DINPUT8`, `DSOUND`, `WINMM`,
`KERNEL32`, `USER32`, `GDI32` and `ole32`. The CRT is static. Game data comes
from `th128.dat`; music streams from `thbgm.dat`. Configuration and scores live
under `%APPDATA%\ShanghaiAlice\th128`. On first start the game shows a display
mode dialog (fullscreen 640×480, window 640×480, 960×720, 1280×960).

## Maintained source

| Source | Owners | State |
| --- | --- | --- |
| `src/TimeScale.cpp` | `SetTimeScale` `0x00407310` | exact (linked PE) |
| `src/ZunTimer.cpp` | `ZunTimerModulo` `0x00402B20`; `SetCurrent` `0x00403240`, `SetCurrentF` `0x00432C40`; comparison operators `<` `0x00402C40`, `<=` `0x00407130`, `>=` `0x004073B0`, `==` `0x00407E70`, `>` `0x00410CD0` | exact (linked PE) |
| `src/AnmVmSetters.cpp` | `AnmVmSetScale` `0x00402C20`, `SetScaleX` `0x00407040`, `SetScaleY` `0x00407020`, `SetScale2` `0x00402C00`, `SetUvScroll` `0x00402B60` | exact (linked PE) |
| `src/PbgFile.cpp` | `Pbg::File` Open `0x0044FE00`, Close/Read/Write/Tell/GetSize/Seek `0x0044FFB0..0x004500D5`, ReadWhole `0x004500E0`, `~File` `0x0044FDD0`, scalar deleting destructors of `File` `0x0044FD80` and `IFile` `0x00450210` | exact (normal COFF) |
| `src/PbgMemFile.cpp` | `Pbg::MemFile` Close/Read/Write/Tell/GetSize, `~MemFile` `0x00450EC0`, scalar deleting destructor `0x00450E70` | exact (normal COFF); `Seek` `0x00450FE0` frontier |
| `src/AnmVm.cpp` | `AnmVm::AnmVm` `0x004031A0` | exact (normal COFF) |
| `src/PbgArchive.cpp` | `Pbg::ArchiveEntry::~ArchiveEntry` `0x0044FCB0`, `Pbg::Archive::Open` `0x0044F5F0`, `Close` `0x0044F730`, `~Archive` `0x0044F5E0` | exact (linked PE) |
| `src/DebugLog.cpp` | `DebugPrint` `0x0044FD30` | exact (normal COFF) |
