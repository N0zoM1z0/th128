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

The gap-classification pass accounts for every byte in the game band;
this is not a claim that every candidate boundary or origin is closed. The
current ledger has 64 provisional candidate boundaries (all in the CRT band) and
21 origin rows awaiting review (unreferenced `d3dx9math.inl` candidates; all
16 CRT-band boundary-review rows and all 12 LTCG-entry-adapter rows were closed
in Phase 0). The 3,516-candidate denominator remains provisional; see
[PROGRESS](PROGRESS.md) for generated counts and the knowledge base for the
historical review evidence. Most of the 1,038 added game functions are small
callbacks and unreferenced dead copies.

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
| `src/LaserPoint.cpp` | `ZunVec3::SetPolarXY` `0x00432E20` (natural C++ frontier); four laser slot-0 point methods `0x00429BF0..0x00429D59` | target FSINCOS codegen unmatched; slot 0 frontiers |
| `src/Spt.cpp` | `SptInf` five defaults and the scalar deleting destructor `0x00414630` | exact (normal COFF) |
| `src/AngleMath.cpp` | `NormalizeAngle` `0x0046A700`, `AddAngles` `0x0046A660` | exact (normal COFF) |
| `src/MenuCursor.cpp` | `MenuCursor::Push` `0x0046A900`, `Pop` `0x0046A940`, `Move` `0x0046A970` | exact (linked PE, 3/3) |
| `src/Rng.cpp` | `Rng::GetU16` `0x0046A3F0`, `GetU32` `0x0046A460`, `GetFloat`/`GetFloatSigned`/`GetAngle` `0x0046A5D0..0x0046A65E`, `GetFloat(float)` `0x0040D130` | exact (linked PE) |
| `src/SupervisorTime.cpp` | `GetElapsedSeconds` `0x00455160` | exact (linked PE, 1/1) |
| `src/SoundManager.cpp` | `SoundManager` / `SfxChannel` `0x00458410..0x0045921F` (17 functions) | exact (linked PE, 17/17) |
| `src/ThreadInf.cpp` | `ThreadInf` / `Supervisor` lock helpers `0x0046AC80..0x0046ADF3` (8 functions) | exact (linked PE, 8/8) |
| `src/Chain.cpp` | `Chain` / `ChainElem` list and registration helpers `0x00468130..0x004688EC` (13 exact) | mixed (13 exact, 5 `CutImpl`/caller frontiers) |
| `src/FileSystem.cpp` | `FileSystem` read/write/path helpers (7 exact) | mixed (7/8 exact, linked PE) |
| `src/GameErrorContext.cpp` | `GameErrorContext` logging helpers (2 exact) | exact (linked PE, 2/2) |
| `src/EngineUtil.cpp` | engine utility leaves (2 exact) | mixed (2/5 exact, linked PE) |
| `src/Input.cpp` | `InitJoypad`, `MapJoyButtonByte`/`Bit`, `UpdateInputSlot`, `PollKeyboardState`, `ClearKeyboardState` (6 exact) | mixed (6/8 exact, linked PE) |
| `src/CoreMath.cpp` | `Vec2LengthSq`, `ShortestAngleDifference2`, `SetGlobalMatrix4x4`, time-scale/chain leaves (11 exact) | mixed (11/14 exact, linked PE) |
| `src/CSoundManager.cpp` | `CSoundManager`, `CSoundView`, `CWaveFileView` DirectSound layer (25 exact) | mixed (25/29 exact, linked PE) |
| `src/EclResource.cpp` | `SptResourceInf::LoadIncludes` `0x00414680` (exact), `Load` `0x004703F0` (frontier) | normal COFF |
| `src/AnmManager.cpp` | `GetVmById` `0x00467420`, `FlushSpriteBatch` `0x0045EF60`, `CleanupVm` `0x00466F60`, `DrawVmGroup` `0x00466CE0`, `MarkResourceVmsForDelete` `0x00467710`, `AreAllAnmsLoaded`, `ReleaseAnm`, `IsPoolVm`, 23 `DrawLayer*` callbacks (31 exact) | mixed (31 exact linked PE + frontiers) |
| `src/AnmResource.cpp` | `AnmResource::AnmResource` `0x00468110`, `Release` `0x00465F60`, `AnmTextureEntry::Release` `0x00466060` | mixed (3 exact linked PE + `CreateVm` frontier) |
| `src/AnmHelpers.cpp` | `ResolveAnmVmId` `0x00467780` and 31 `AnmId`/VM/child/rotation/draw helpers `0x00467570..0x00468100` | mixed (32 exact linked PE + `ClearD3DTexture*` frontiers) |
| `src/AnmInterp.cpp` | `AnmInterp<T>` (`AnmColor3`/`float`/`int`/`ZunVec2`) methods + vector/angle/color helpers `0x0045D330..0x0045E76F` | mixed (39 exact linked PE + frontiers) |
| `src/AnmVmOperands.cpp` | four operand resolvers `0x00459900..0x00459CE0` | exact (linked PE, 4/4) |
| `src/AnmVmResource.cpp` | `NormalizeAnmCoordinates` `0x00459D50` | exact (linked PE) |
| `src/AnmVmEffects.cpp` | `StartAnmVmEffect06` `0x00412C80`, four empty effect-table slots `0x00412B30..0x00412E00`, plus `ApplyAnmVmRenderState` / world-matrix / draw paths | mixed (5 exact normal COFF + frontiers) |
| `src/AnmVmScript.cpp` | `EaseTransition` `0x0046B190`, `ResetTransitionTimer` `0x00406F80`, `ResetScalarTransitionTimer` `0x0040CEA0`, `ResetViewTransitionTimer` `0x0045DAD0`, `SetUpdateTimerCurrent` `0x00403280`; `AnmVm::Update` `0x00459D90` (13,604B) | mixed (5 exact linked PE; `Update` frontier) |
| `src/AnmVmTransitions.cpp` | `SetScaleTransition` `0x0045E190`, `SetSecondaryScaleTransition` `0x0045E080`, `SetSecondaryEffectState` `0x0045E240`; `SetColorTransition`/`SetEffectState` | mixed (3 exact linked PE + frontiers) |
