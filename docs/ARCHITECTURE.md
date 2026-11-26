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

The current source mappings and signatures are in
[functions.csv](../config/functions.csv), exact acceptance is in
[matches.csv](../config/matches.csv), and build/comparison recipes are in
[match-units.toml](../config/match-units.toml). These ledgers supersede older
per-file totals. [PROGRESS.md](PROGRESS.md) is the generated aggregate.
The handoff carries the current work and next command; the knowledge base
retains target/compiler findings and failed hypotheses.

The active families at the 2026-11-26 handoff are:

| Source family | Current evidence |
| --- | --- |
| `TextImageLifetime.cpp` | Five exact image/font creation and destruction owners, 1,139B / 80 fields |
| `TextTexture.cpp`, `AnmText.cpp` | Four scratch-image/text backend candidates and four VM-text candidates; all remain frontiers |
| `CardInf.cpp`, `CardInfActions.cpp`, `AsciiInf.cpp`, `PopupNotifications.cpp` | Mixed lifecycle/drawing/score exact owners and activation/notification frontiers; see current handoff |
| `SupervisorFrame.cpp`, `SupervisorPresent.cpp`, screenshot/surface/device owners | Mixed exact helpers and complete frame/camera/Present frontiers; retained findings are in the knowledge base |
| `AnmVmScript.cpp`, `AnmManager.cpp`, ANM helper/resource/draw owners | Exact leaves and callbacks alongside larger unresolved owners; archived ANM frontier detail is in RE_CHECKPOINT_NOTES.md |

This is a navigation map. Source presence and an exact subset do not prove
native product closure or runtime behavior. Historical phase totals and
older per-owner measurements remain in
[RE_CHECKPOINT_NOTES.md](RE_CHECKPOINT_NOTES.md); use current ledgers before
turning them into a present-tense status claim.
