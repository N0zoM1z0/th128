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

The game code occupies `.text` from `0x00401000` to about `0x004725A3`
(1,255 inventory candidates, about 408 KB). The first function matched to the
VC10 static CRT archives is at `0x004725A4`; 841 candidates from there to
`0x004991F0` are pinned `libcmt.lib`/`libcpmt.lib` code. The 125 unclassified
candidates in that tail are expected to be CRT variants, compiler helpers and
library stubs that the archive scan could not place uniquely; they need
individual review.

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
| `src/ZunTimer.cpp` | `ZunTimerModulo` `0x00402B20`; comparison operators `<` `0x00402C40`, `<=` `0x00407130`, `>=` `0x004073B0`, `==` `0x00407E70`, `>` `0x00410CD0` | exact (linked PE) |
| `src/AnmVmSetters.cpp` | `AnmVmSetScale` `0x00402C20`, `SetScaleX` `0x00407040`, `SetScaleY` `0x00407020`, `SetScale2` `0x00402C00`, `SetUvScroll` `0x00402B60` | exact (linked PE) |
