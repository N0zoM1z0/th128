# TH128 reconstruction handoff

Prepared 2026-11-26 for a new agent. Last executable-code checkpoint:
**`c1419af` — five exact scratch-image/font lifetime owners**.
No reconstruction job remains running, and no push was performed.

## Resume

1. Read [AGENTS.md](../AGENTS.md), [workflow](RE_WORKFLOW.md),
   [Oracles](ORACLES.md), then the relevant source and ledger rows.
   Preserve dirty/untracked work; inspect `git status --short --branch`
   and `git log -5 --oneline` before editing.
2. Before target-dependent work in a new session, run once:

   ~~~bash
   scripts/repo-python scripts/verify-target.py
   scripts/repo-python scripts/validate-tracking.py --require-target
   scripts/repo-python scripts/verify-toolchain.py --check
   ~~~

3. Analyze through `scripts/repo-python scripts/ghidra.py`; each query
   attests the original disk target, loaded Program and complete mapped .text.
   Ghidra boundaries/types/decompilation remain hypotheses.
4. Use commit titles **`gpt-6.1-sol: <English summary>`**. Build and compare
   affected complete owners and their real consumers. Notes and comment-only
   cleanup need diff review, not a compiler, analyzer or broad replay.

Only use ignored `resources/th128.exe`: original Japanese v1.00a,
752,640 bytes, SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
Keep VC10 RTM and `/GL` ownership; do not substitute another game/compiler.
[Shared tools](SHARED_TOOLS.md) explains the isolated Factory runtime.

## Current state

[PROGRESS.md](PROGRESS.md) is generated from canonical ledgers. At the last
code checkpoint: **449 source mappings, 357 exact functions / 26,077B**;
authored exact is **348 functions / 25,717B**, with **92** authored source
frontiers. There are 3,516 candidates, 64 provisional boundaries and 21
pending origins. The inventory denominator is provisional. The native
Windows product has not been closed or runtime-validated.

`config/matches.csv` grants exact credit;
`config/match-units.toml` defines the reproducible owner context, complete
extent and exhaustive address fields. Successful compilation, source presence
and near matches grant no extra credit. Git is source provenance.

## Latest verified work

[TextImageLifetime.cpp](../src/TextImageLifetime.cpp) has five exact owners
in one three-source cold context with real Rng.cpp support:

| Owner | Target | Bytes / fields |
| --- | --- | ---: |
| DestroyTextImage | 0x004516B0 | 105 / 14 |
| InitializeDefaultTextImage | 0x00451720 | 22 / 2 |
| InitializeTextImage | 0x00451740 | 413 / 22 |
| InitializeTextFonts | 0x004525D0 | 516 / 32 |
| DestroyTextFonts | 0x004527E0 | 83 / 10 |

All **1,139B / 80 fields** passed complete primary-PDB-owner comparison.
Primary flags are `/O2 /Ob1 /Oy- /GL`; this context's RNG support keeps
`/O2 /Oy- /GL` without `/Ob1` to reproduce the observed GetU16 inlining.
The established RNG source and primary context were not changed. Original
TU partition and compiler settings are not proved by this matched context.
[Finding and failed candidates](KNOWLEDGE_BASE.md#scratch-image-lifetime-and-font-setup-2026-11-26),
[final results](../.analysis/text-image-lifetime-final-results.json),
[scope](../.analysis/text-image-lifetime-final-scope.json),
[build log](../.analysis/text-image-lifetime-final-cold-build.log).

Keep physical DIB height **-129**, logical stored height **128**, width 1024
and byte count pitch * 128. Format lookup returns null only for requested -1;
other misses return the sentinel address. Font setup continues after both
format attempts fail. Font cleanup deletes default/2/4/6/1/3/5/7 without
clearing globals. The two unreferenced wrappers retain medium boundary
confidence despite exact codegen. The knowledge base has full layout, font
names, sizes, weights and target references.

## Next useful work

Inspect the four adjacent unreferenced color-conversion owners at
0x004518E0 (66B), 0x00451930 (55B), 0x00451970 (30B), 0x00451990 (42B).
Their source types may help explain the alternate text fill frontier:

~~~bash
scripts/repo-python scripts/ghidra.py query .analysis/text-color-converters-span.txt review_span 0x004518E0 0x004519B9
~~~

This query has not been run. Review complete instructions, delimiters and
references before declaring source or changing boundary confidence.

The current text/Card family frontiers are:

| Owner | Candidate / target bytes | Remaining issue |
| --- | ---: | --- |
| Text image RGB fill | 823 / 817 | Whole owner size |
| Text image alpha inversion | 166 / 166 | Wrong three-field layout |
| DrawTextToTexture | 696 / 696 | Ordinary +533/+580 after 42 fields replay |
| Alternate texture text | 592 / 584 | Fill-loop/owner layout |
| Card Start / Finish | 1504 / 1496; 374 / 398 | Whole owner size |
| Popup Show | 1020 / 1072 | Whole owner/private calling context |
| Sprite text | 214 / 210 | Whole owner/stack layout |
| Explicit-X / centered / half-center text | 142 / 132; 215 / 196; 201 / 193 | Whole owner size |

These are complete owner sizes, not byte-difference counts. AddScore remains
exact at 54B / three fields. Card Update's floating-add operand order and
Shutdown's CutImpl register context are separate frontiers.
[Prior full comparisons](../.analysis/texture-text-final-results.json).

Do not repeat the typed IDirect3DTexture9* hypothesis: it retained the same
two draw differences. GDI-only local scopes and taking backend addresses
also failed. The scoped-lock CreateVm hypothesis was falsified; the existing
body already writes its result before unlock. Do not restore inline assembly,
volatile tricks, fake callees, ABI shims or padding.

For ANM work, start from the relevant knowledge-base finding and
[archived ANM frontier detail](RE_CHECKPOINT_NOTES.md#anm-frontier-detail-archived-from-c1419af).
The older handoff mixed historical measurements with a superseded task list;
it is no longer the active restart guide. In particular, text backends and
font/image lifetime source already exist.

## Retained state and storage cleanup

The operator requested a second, more aggressive cleanup on 2026-11-26.
This pass freed **113.84 MiB**, in addition to the earlier approximately
37 MiB. `build/` is empty; `.build/` did not exist. All generated objects,
executables, MAP/PDB files and receipts can be recreated from the unit recipes.

Removed 3,006 old files from the `.analysis/` root, seven historical analysis
and experiment directories, Ghidra application/script logs and generated
compiled-script caches. Old compiler snapshots, one-off relink scripts,
probe matrices and historical result logs are no longer kept as required
evidence. Their useful findings and failed approaches remain in the knowledge
base and Git history.

Kept only 15 active text-component files: the latest lifetime final results,
scope/build summary/field inventory, the text-family final comparisons and
small target reviews needed by the current work. Also kept the architecture
index used by function-context.py and the isolated Ghidra user settings.
The original target/data, compiler payload, Wine prefix, Factory selectors
and Ghidra project are untouched.

Deleted historical private-file links in the knowledge base/archive now
appear as inline filenames. They identify past work and are not promises
that the artifact still exists. Do not restore old log trees merely to satisfy
those historical references. Regenerate a target query when it is relevant.

The last executable-code verification remains `c1419af`; source statements,
layouts, profiles and ledgers were not changed by cleanup. For a new strict
comparison, first rebuild the selected context. For example, to reproduce
only the five lifetime owners:

~~~bash
scripts/repo-python scripts/replay-exact-units.py \
  --unit text-image-destroy --unit text-image-initialize-default \
  --unit text-image-initialize --unit text-fonts-initialize \
  --unit text-fonts-destroy
~~~

No comparison receipts were rewritten and no compilation was run merely for
cleanup. Never use `git clean -fdx`.

Historical phase/checkpoint detail is in [RE_CHECKPOINT_NOTES.md](RE_CHECKPOINT_NOTES.md).
Current target/compiler conclusions and useful failures are in
[KNOWLEDGE_BASE.md](KNOWLEDGE_BASE.md). Remote state was not fetched or verified;
local commits are separate from remote publication.
