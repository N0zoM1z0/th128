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
| `TimeScale`, `ZunTimer`, `AnmVmSetters` | linked PE, `/O2 /Ob1 /Oy- /GL` behind probe contexts | 12 exact |
| `PbgFile`, `PbgMemFile`, `AnmVm` constructor | normal COFF, `/O2 /Oy-` | 19 exact; `MemFile::Seek` frontier |

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

1. **AsciiInf** (`0x00401010..0x00402A55`, vtable `0x004A1F3C`). The
   constructor uses an `ESI` receiver, so it is a `/GL` owner. Its
   0x141-iteration, 0x164-stride bit-clearing loop starting at +0xB44 does not
   fit a single array inside the 0x1C8A8-byte object; resolve that layout
   before writing the constructor. The init function `0x004010B0` registers
   four 0x24-byte `UpdateFunc` chain nodes (`0x004681D0`, `0x00468270`).
2. **Pbg archive** (`0x0044F5F0..0x00450D70`): THA1 header decrypt
   (`0x00469A00`, key table `0x004B1200`), directory parse `0x0044FB20`,
   entry read `0x0044F7D0`, LZSS decode `0x00450860` and an LZSS encoder.
   TH11's decoder never matched (177 differences), so start with the
   directory and entry code.
3. `MemFile::Seek` frontier: the only remaining difference is ESI
   shrink-wrapping on the FILE_BEGIN path.
4. Many 3–20 byte leaves have no direct callers (function-pointer tables);
   find their tables before naming them.
5. Product closure needs the August 2009 DirectX SDK (`d3dx9_42`).
