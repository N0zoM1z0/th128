# TH128 reconstruction handoff

## Start here

Run `git status --short --branch` and `git log -5 --oneline`; dirty work is
ours to continue. Work against Japanese TH12.8 v1.00a only: private
`resources/th128.exe`, 752,640 bytes, SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
The code-generation target is **VC10 RTM build 30319**; keep `/GL` in LTCG
owner contexts.

```bash
scripts/repo-python scripts/verify-target.py
scripts/repo-python scripts/validate-tracking.py --require-target
scripts/repo-python scripts/verify-toolchain.py --check
scripts/repo-python scripts/replay-exact-units.py --source src/ZunTimer.cpp
```

## Current state

Bootstrap is complete and every lane has been exercised once on the real
target:

- **Target** attested (hashes, PE, Rich stream, complete mapped `.text`); the
  Chinese-patched `th128_CN.exe` is rejected.
- **Toolchain**: archaic-msvc/msvc1000 (cl 16.00.30319.01) runs headlessly in
  `.tools/wineprefix-msvc1000`; `verify-toolchain.py --execute` passes normal
  C/C++, `/GL`, RC, cvtres and `/LTCG` PE link with MAP/PDB.
- **Ghidra 12.1.4** project `ghidra-project/TH128` imported once and
  re-attested read-only. Initial inventory: 2,221 candidates.
- **Library origin**: `scan-library-code.py --apply` matched 841 candidates
  (141,942 bytes) uniquely to VC10 `libcmt.lib`/`libcpmt.lib`; 713 have
  archive-equal extents and reviewed boundaries.
- **Oracles**: synthetic suites pass (8 normal-COFF, 27 linked-PE). The first
  real linked unit, `SetTimeScale` at `0x00407310` (16 bytes), is exact with
  `/TP /O2 /Oy- /Gy /MT /GL`; removing `/Oy-` is rejected. The second,
  `ZunTimerModulo` at `0x00402B20`, reproduces an LTCG private `EAX`
  convention from a `__stdcall` declaration (see the knowledge base).

Counts are in [PROGRESS](PROGRESS.md); findings in
[KNOWLEDGE_BASE](KNOWLEDGE_BASE.md).

## Next useful work

1. Review the remaining 1,380 origin-pending candidates and the `.text` gaps
   (for example the uninventoried `FLD [EAX]; RET` at `0x00407320`).
2. Reconstruct small authored leaves around `0x00402B20..0x00407400`; many use
   LTCG custom conventions (receiver in `EAX`/`ECX`, `RET n`), which need
   linked contexts whose callers establish the same convention.
3. Confirm whether `/Oy-` holds across the game code, then identify the
   subsystems (Ghidra `architecture` export, string references).
4. Product closure needs the August 2009 DirectX SDK (`d3dx9_42`); it is not
   provisioned yet.
