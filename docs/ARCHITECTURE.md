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

The boundary review closed the game-band inventory: it now has 3,516
candidates overall, and no game-band byte is unclassified (see the knowledge
base). Most of the 1,038 added game functions are small callbacks and
unreferenced dead copies.

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
| `src/ZunTimer.cpp` | `ZunTimerModulo` `0x00402B20`; `SetCurrent` `0x00403240`, `SetCurrentF` `0x00432C40`, `Increment` `0x00402AC0`; comparison operators `<` `0x00402C40`, `<=` `0x00407130`, `>=` `0x004073B0`, `==` `0x00407E70`, `>` `0x00410CD0` | exact (linked PE) |
| `src/AnmVmSetters.cpp` | `AnmVmSetScale` `0x00402C20`, `SetScaleX` `0x00407040`, `SetScaleY` `0x00407020`, `SetScale2` `0x00402C00`, `SetUvScroll` `0x00402B60` | exact (linked PE) |
| `src/PbgFile.cpp` | `Pbg::File` Open `0x0044FE00`, Close/Read/Write/Tell/GetSize/Seek `0x0044FFB0..0x004500D5`, ReadWhole `0x004500E0`, `~File` `0x0044FDD0`, scalar deleting destructors of `File` `0x0044FD80` and `IFile` `0x00450210` | exact (normal COFF) |
| `src/PbgMemFile.cpp` | `Pbg::MemFile` Close/Read/Write/Tell/GetSize, `~MemFile` `0x00450EC0`, scalar deleting destructor `0x00450E70` | exact (normal COFF); `Seek` `0x00450FE0` frontier |
| `src/AnmVm.cpp` | `AnmVm::AnmVm` `0x004031A0` | exact (normal COFF) |
| `src/PbgArchive.cpp` | `Pbg::ArchiveEntry` constructor `0x0044FD20` and destructor `0x0044FCB0`, `Pbg::Archive::Open` `0x0044F5F0`, `Close` `0x0044F730`, `~Archive` `0x0044F5E0`, `ParseArchiveDirectory` `0x0044FB20` | exact (linked PE) |
| `src/DebugLog.cpp` | `DebugPrint` `0x0044FD30` | exact (normal COFF) |
| `src/BlockCipher.cpp`, `src/Lzss.cpp` | `DecryptBlocks` `0x00469A00`, `DecompressLzss` `0x00450860` | frontiers (linked PE, archive context) |
| `src/WindowProc.cpp` | `WindowProc` `0x00454420` | exact (normal COFF) |
| `src/GameWindow.cpp` | `CreateGameWindow` `0x00455320` | frontier (normal COFF) |
| `src/LaserData.cpp` | `LaserDataInf` 24 virtual defaults `0x004299D0..0x00429B52` | exact (normal COFF) |
| `src/LaserPoint.cpp` | `ZunVec3::SetPolarXY` `0x00432E20` (exact); the four laser slot-0 point methods `0x00429BF0..0x00429D59` | linked PE; slot 0 frontiers |
| `src/Spt.cpp` | `SptInf` five defaults and the scalar deleting destructor `0x00414630` | exact (normal COFF) |
| `src/AngleMath.cpp` | `NormalizeAngle` `0x0046A700`, `AddAngles` `0x0046A660` | exact (normal COFF) |
| `src/MenuCursor.cpp` | `MenuCursor::Move` `0x0046A970` | exact (linked PE) |
