# TH128 reconstruction agent rules

## Highest priority: lightweight exact reconstruction and useful knowledge

These are the operator's current workflow requirements. They override any
older instruction that mandates broad CI, repeated preflights, document or
source-pin refreshes, per-checkpoint audits or extra workflow machinery. The
exactness, target identity, natural-source and state-isolation rules below
still apply.

- Prioritize exact reconstruction and reusable knowledge. Use the shortest
  useful loop: inspect relevant state, analyze/edit, compile and compare the
  affected scope, record what was learned, and create a coherent checkpoint.
- Use existing repository scripts and composable Bash. Add only the smallest
  change needed for an observed problem. Do not introduce a framework, schema,
  registry, state machine, generalized validator, test family or reporting
  process for routine reconstruction.
- Documentation, knowledge and handoff edits need diff review only: no
  compiler, analyzer, full CI, cold replay or receipt refresh. Check syntax
  only for machine-readable inputs actually changed.
- Source changes need the complete affected exact comparison, including owned
  extents and relocations, plus known consumers of changed shared inputs.
  Tool changes need relevant positive/negative cases and a bounded real smoke.
  Run a broad suite only on explicit request or for an impact that cannot be
  narrowed, and state that reason first.
- Do not add tests or freshness gates for prose hashes, prompt wording,
  progress totals, handoff tables or ordinary source spelling. Bind exact
  evidence to executable inputs and comparison settings; Git is provenance.
  Never edit a historical result to manufacture a pass.
- Plain Markdown is sufficient for knowledge. Lead with the finding, name the
  target/compiler/scope, link evidence, and keep useful failed approaches and
  open questions. State a real uncertainty once, where it changes a decision.
- Keep the handoff focused on current work, blockers and the next useful
  command. Generate progress from canonical ledgers.
- Preserve dirty/untracked work, the target, game-specific Wine prefix and
  analyzer project. Knowledge and successful commands confer no exact credit.

## Target

Only reconstruct the original Japanese TH12.8 (妖精大戦争) v1.00a executable,
752,640 bytes, SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
The canonical private input is ignored `resources/th128.exe`. Never substitute
`th128_CN.exe` (Chinese patch), `custom.exe`, the English patch, the Steam
build or any patched bytes. `TH128_TARGET_PATH` is only a deliberate local
override.

## Session recovery

1. Read [docs/RE_HANDOFF.md](docs/RE_HANDOFF.md), then
   [workflow](docs/RE_WORKFLOW.md), [Oracles](docs/ORACLES.md) and the
   affected source and ledger rows.
2. Run `git status --short --branch` and inspect dirty work. Interrupted work
   is ours to continue, not something to route around.
3. Before target-dependent work, verify once per session:

   ```bash
   scripts/repo-python scripts/verify-target.py
   scripts/repo-python scripts/validate-tracking.py --require-target
   scripts/repo-python scripts/verify-toolchain.py --check
   ```

Use `scripts/repo-python` for repository Python: it selects an interpreter
with the hash-pinned Capstone decoder. Analyze through direct local Ghidra
with `scripts/ghidra.py`; it attests the target, the loaded program and the
complete mapped `.text` before every query. Ghidra names, types, boundaries
and decompilation are provisional hypotheses, never exactness evidence.

## Toolchain

TH128-local PE linker 10.0 and Rich build-30319 records select **VC10 RTM
(cl 16.00.30319.01, link 10.00.30319.01)**. Rich records show 56 LTCG C++
objects next to normal C/C++ objects, so game code is expected to need
`/GL` linked contexts; retain `/GL` in an LTCG owner and never remove it to
fit the normal-COFF Oracle. Compiler flags, TU partition, per-unit LTCG
ownership, library selection, resources and link order are unknown until
target evidence settles them. Never use the TH10/TH11 compilers or profiles
as TH128 evidence.

## Exact evidence

Keep target observations, Ghidra hypotheses, compiler observations, source
presence, exact codegen, native product closure, runtime behavior and port
status separate. Only a complete, reproducible, target-bound VC10 comparison
may enter `config/matches.csv`; near matches stay frontier candidates. A
comparison covers the whole owned extent, physical tails/tables and every
relocation's offset, type, target, addend and call identity.

Use natural C/C++. Never embed target bytes, add inert padding, patch code,
fake returns, misdeclare the ABI, or add inline assembly, register hints,
barriers, dummy clobbers or compiler-profile roulette to force a match.
Linked/LTCG acceptance goes only through `compare-linked-function.py` under
[LINKED_ORACLE.md](docs/LINKED_ORACLE.md).

## Adjacent-game hypothesis sources

These local checkouts are read-only references:

- `/home/pentester/coding/codex_ida/th11-reconstruction/th11` (VC9 RTM, LTCG)
- `/home/pentester/coding/codex_ida/th10-reconstruction/th10` (VC7.1 SP1)
- `/home/pentester/coding/codex_ida/th09-reconstruction/th09` (VC7.1)

Their names, layouts and source shapes are hypotheses, never TH128 address,
byte, layout, ownership or behavior evidence. Accept TH128 conclusions only
from TH128-local target, xref, ABI, compiler, build or runtime evidence.

## Phase order and scope

Exact reconstruction first; then faithful Windows i386 product closure; then
semantic reconstruction against two runtime Oracles (the original and the
native reconstructed product); portability last. Later phases are not
prerequisites for exact checkpoints, and exact functions do not prove the
executable builds or runs. Do not add broad behavioral harnesses,
allocation-failure/null-pointer matrices or fuzzing as routine work.

## Retention and checkpoints

Experiments, logs and dumps go in `.analysis/`, build artifacts in `build/`,
tools in `.tools/`, the private database in `ghidra-project/`. Never commit
game executables/data, compiler payloads, Ghidra databases, decompiler
output, credentials or private endpoints. Do not run `git clean -fdx`: it
deletes the ignored target, prefix and project.

Create concise local commits after substantive advances using the required
format `gpt-web: <English summary>`. Cold-replay only changed exact units and
directly affected dependencies; never run a full cold replay for routine
source work. Refresh tracking/progress only when the relevant ledgers change,
and finish with `git diff --check`. Report local checkpoints separately from
any verified remote state.

## Shared Factory runtime

Follow [docs/SHARED_TOOLS.md](docs/SHARED_TOOLS.md). Ghidra/JDK and Wine
executables are shared through Factory and work without MCP. Keep this game's
prefix (`.tools/wineprefix-msvc1000`), analyzer project, Java home/cache,
target and compiler profile isolated. Do not install duplicate tools or
inherit another game's `WINEPREFIX`.
