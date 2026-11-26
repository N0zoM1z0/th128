# Reconstruction workflow

> Current workflow: follow the **Highest priority** section in `AGENTS.md`.

Read `AGENTS.md` and `RE_HANDOFF.md` and inspect dirty work before touching
it. Verify target and toolchain identity once on resume. Query the target
through `scripts/ghidra.py` only.

For each coherent subsystem:

1. Review its complete target control flow, incoming and interior references,
   sparse Ghidra ranges, adjacent bytes, jump tables and provisional ABI.
2. Record boundary and origin findings independently. Keep unproved facts
   unknown. TH10/TH11 similarities supply hypotheses only.
3. Write natural C/C++ and declare source mappings. Probe profiles with the
   pinned VC10 RTM compiler. Expect `/GL` for game code: Rich records 56 LTCG
   C++ objects. Never force equality with target bytes, padding, fake returns
   or incorrect calling conventions.
4. For a linked owner, configure a `linked-pe` unit with the complete public
   contribution and every reviewed field. For a normal-COFF owner, recover the
   decorated symbol's complete extent and exhaustive relocation list.
   Source candidates may have `frontier = true` recipes. Remove that flag
   and enter `matches.csv` only after the complete zero-difference replay.
5. Replay only affected canonical exact units and directly impacted
   dependencies; do not run a full cold replay for routine changes. Update
   ledgers and generated progress only when those inputs change, review the
   diff, and checkpoint substantive work as
   `gpt-6.1-sol: <English summary>`.

Static CRT and STL code (`libcmt.lib`, `libcpmt.lib`) is classified with
`scan-library-code.py` against the pinned VC10 archives rather than
reconstructed as game source.

Use `.analysis/` for probes and dumps, `build/` for artifacts, `.tools/` for
private tools and `ghidra-project/` for the private database. Do not refresh
the initial inventory over maintained mappings or exact rows; export later
candidates to a separate private location and reconcile.

Native product closure, broad runtime comparisons and portability are later
phases and do not block exact checkpoints.
