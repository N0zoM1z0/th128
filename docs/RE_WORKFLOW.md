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
   decorated symbol's complete extent and exhaustive relocation list. Add a
   canonical unit only when its complete zero-difference replay reproduces.
5. Cold-replay affected canonical units, update ledgers and generated
   progress, check the diff, and make a concise `gpt-web: ...` checkpoint.

Static CRT and STL code (`libcmt.lib`, `libcpmt.lib`) is classified with
`scan-library-code.py` against the pinned VC10 archives rather than
reconstructed as game source.

Use `.analysis/` for probes and dumps, `build/` for artifacts, `.tools/` for
private tools and `ghidra-project/` for the private database. Do not refresh
the initial inventory over maintained mappings or exact rows; export later
candidates to a separate private location and reconcile.

Native product closure, broad runtime comparisons and portability are later
phases and do not block exact checkpoints.
