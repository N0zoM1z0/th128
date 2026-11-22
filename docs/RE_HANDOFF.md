# TH128 reconstruction handoff

## Start here

Run `git status --short --branch` and `git log -5 --oneline`; dirty work is
ours to continue. Work against Japanese TH12.8 v1.00a only: private
`resources/th128.exe`, 752,640 bytes, SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
The code-generation target is **VC10 RTM build 30319**.

```bash
scripts/repo-python scripts/verify-target.py
scripts/repo-python scripts/validate-tracking.py --require-target
scripts/repo-python scripts/verify-toolchain.py --check
scripts/repo-python scripts/replay-exact-units.py --source src/PbgFile.cpp
```

The remote is the public `N0zoM1z0/th128`; push after coherent checkpoints.

## Current state

Counts are generated in [PROGRESS](PROGRESS.md); the source map is in
[ARCHITECTURE](ARCHITECTURE.md) and findings in
[KNOWLEDGE_BASE](KNOWLEDGE_BASE.md).

| Area | Lane and profile | State |
| --- | --- | --- |
| `TimeScale`, `ZunTimer`, `AnmVmSetters` | linked PE, `/O2 /Ob1 /Oy- /GL` behind probe contexts | 14 exact |
| `PbgArchive` (`_ArcMngr.cpp`) | linked PE, `/O2 /Ob1 /Oy- /EHsc /GL`; `DebugLog.cpp` as normal support | 4 exact |
| `PbgFile`, `PbgMemFile`, `AnmVm` constructor, `DebugPrint` | normal COFF, `/O2 /Oy-` | 20 exact; `MemFile::Seek` frontier |

Origin review is applied for every candidate (34 rows stay open pending
VC10 compiles); boundary review and inventory gap recovery are running as a
separate pass that writes `.analysis/boundary-review/` for merging.

Two profiles are in use and both are hypotheses about the original TU split:
small game leaves with private `EAX` receivers need `/GL`; the virtual Pbg
classes and the AnmVm constructor reproduce as normal COFF. Default `/O2`
inlining is required where a destructor inlines `Close`; `/Ob1` contexts are
fine for leaves that inline nothing.

`scripts/record-source.py UNIT ... --exact EVIDENCE` updates the function,
boundary, mapping, implemented and match ledgers from a canonical unit after
its comparator passed. `scripts/diff-coff-target.py OBJ SYMBOL ADDR SIZE`
prints a side-by-side diff (relocated operands differ by design);
`scripts/function-context.py ADDR` shows callers and strings from the Ghidra
architecture export (`ghidra.py architecture`).

## Next useful work

1. **Pbg archive reader chain.** `ReadHeader`, `ReadEntry` and
   `DecryptBlocks` are real frontiers in the archive context; see the
   knowledge base for the remaining register/layout differences. Next is the
   LZSS decoder (`0x00450860`, `__stdcall`, four stack arguments, history at
   `0x004D0AB8`): replacing its placeholder may also shift the callers'
   register priorities. Replay `src/PbgArchive.cpp` after each change.
2. **AsciiInf** (`0x00401010..0x00402A55`, vtable `0x004A1F3C`): `/GL` owner
   with an `ESI` receiver. Resolve the 0x141-iteration timer loop before
   writing the constructor (see the knowledge base). The init function
   `0x004010B0` registers four 0x24-byte `UpdateFunc` chain nodes
   (`0x004681D0`, `0x00468270`).
3. Byte-identical unfolded copies (`0x00403280`, `0x00432C80`, the timer
   resets at `0x00406F80`/`0x0040CEA0`/`0x0041EDE0`/`0x0045DAD0`) belong to
   other types or names; identify their owners from callers.
4. `MemFile::Seek` frontier: only ESI shrink-wrapping on the FILE_BEGIN path.
5. Merge the boundary review when it lands: apply `boundaries.csv`, add
   `new-functions.csv` rows (then the archive entry constructor `0x0044FD20`
   is ready to match from the existing probe), and keep corrections explicit.
6. Product closure needs the August 2009 DirectX SDK (`d3dx9_42`).
