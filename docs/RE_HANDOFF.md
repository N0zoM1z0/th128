# TH128 reconstruction handoff

Prepared 2026-11-28 for the next agent. Last executable-code checkpoint:
**`22eec6f` — `CleanBatchN` with exact `CbnEnemy::~CbnEnemy` (`0x00415EC0`, +344B)**.
No background job remains running. Local `main` is 2 commits ahead of
`origin/main` (`2354b2d`); no push was performed.

## Resume

1. Read [AGENTS.md](../AGENTS.md), [workflow](RE_WORKFLOW.md),
   [Oracles](ORACLES.md), and the relevant source/ledger rows.
   Preserve dirty/untracked work; inspect `git status --short --branch`
   and `git log -10 --oneline` before editing.
2. Before target-dependent work in a new session, run once:

   ~~~bash
   scripts/repo-python scripts/verify-target.py
   scripts/repo-python scripts/validate-tracking.py --require-target
   scripts/repo-python scripts/verify-toolchain.py --check
   ~~~

3. Analyze through `scripts/repo-python scripts/ghidra.py` (or direct PE/Capstone
   inspection of `resources/th128.exe`). Ghidra boundaries/types/decompilation
   remain provisional hypotheses, never exactness evidence.
4. Keep **VC10 RTM** (`cl 16.00.30319.01`, `link 10.00.30319.01`) and `/GL`
   ownership. Run **only one Wine/VC10 build at a time** on
   `.tools/wineprefix-msvc1000`. Documentation/handoff edits need diff review
   only (`git diff --check`), not a compiler or replay.

Only use ignored `resources/th128.exe`: original Japanese v1.00a,
752,640 bytes, SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
[Shared tools](SHARED_TOOLS.md) explains the isolated Factory runtime.

## Current state

[PROGRESS.md](PROGRESS.md) is generated from canonical ledgers (`scripts/repo-python scripts/progress.py`).
At `22eec6f`:

| Measure | Count |
| --- | ---: |
| Tracked 1.00a function candidates | 3,516 |
| Boundary reviewed / provisional | 3,452 / 64 |
| Origin review pending | 21 |
| Confirmed authored functions / bytes | 2,226 / 435,812B |
| Source-present mappings (authored / pending) | 1,387 (1,351 / 16) |
| Authored source-present exact backlog | 92 |
| **Canonical exact functions / bytes** | **1,294 / 84,111B** |
| **Canonical exact authored functions / bytes** | **1,259 / 82,716B (18.98%)** |

Remaining non-exact, non-library authored functions by size bucket:

- `1–49B`: 705 funcs (12,099B)
- `50–99B`: 142 funcs (10,099B)
- `100–299B`: 236 funcs (42,723B)
- `300–999B`: 203 funcs (103,341B)
- `1000+B`: 81 funcs (192,016B)

`config/matches.csv` grants exact credit; `config/match-units.toml` defines
each reproducible owner context, complete extent and exhaustive address fields.
The native Windows product has not been closed or runtime-validated.

## Latest verified work

Recent commits advanced exact reconstruction across fresh LTCG batches, `/EHsc`
destructors/factories, and VC10 `<string>` STL instantiations:

| Commit | Batch | Exact units landed | Bytes |
| --- | --- | --- | ---: |
| `22eec6f` | `CleanBatchN` | `CbnEnemy::~CbnEnemy` (`0x00415EC0`) | +344B |
| `9ba52b1` | `StlStringBatch` | 10 VC10 STL `std::string`/`bad_alloc`/`allocator<char>` funcs (`0x00452D80..0x00453000`) | +324B |
| `2354b2d` | `CleanBatchM` | `CbmSumU32` (`0x0046AC00`), `std::string::erase` (`0x00452DE0`) | +227B |
| `984057c` | `CleanBatchL` | `CblSwapKey` (`0x004496F0`), `CblScoreTable::GetStageValue` (`0x0044F430`) | +355B |
| `6ade83b` | `CleanBatchK` | `CbkEndingSub::~CbkEndingSub` (`0x00412FC0`), `CbkGui::Draw` (`0x004225F0`) | +316B |
| `732e90f` | `CleanBatchJ` | `CbjCountIceBits` (`0x00410960`), `CbjEnumJoyAxisCb` (`0x00456590`) | +385B |
| `a0cccc3` | `EhBatchF` | `EbfPlayer::~EbfPlayer` (`0x0043ACD0`), `EbfAnmMgr::~EbfAnmMgr` (`0x00464240`) | +686B |
| `0b27adf` | `EhBatchC` | `EbcMainMgr::~EbcMainMgr`, `EbcScoreMgr::~EbcScoreMgr` | +842B |

See [KNOWLEDGE_BASE.md](KNOWLEDGE_BASE.md#vc10-ltcg-batch-reconstruction-findings-2026-11-28)
for the reusable VC10 LTCG codegen patterns and pitfalls from these batches.

## Batch / sequential reconstruction workflow (recommended for next agent)

To maximize exact bytes/minute while building reusable types for later functions,
work in **ordered 2-TU `/GL` batches** (`src/<Batch>.hpp`, `src/<Batch>.cpp`,
`probes/<batch>-context.cpp`, ~8–12s per cold link) instead of hopping between
unrelated frontiers:

### 1. Ordered target waves

1. **Wave A — Fresh 0-FPU non-EH functions (100–900B, `<=6` calls):**
   Query `config/functions.csv` for `owner != 'library'`, empty `source_file`,
   not in `config/matches.csv`. Group 5–8 functions per batch.
   - Ready pre-dumped in `.analysis/clean-batch-n-disasm.txt`:
     `0x0044CEF0` (851B, spell history refresh), `0x00450560` (762B, LZSS compress),
     `0x00452310` (645B, surface blit), `0x004580C0` (276B, PlayBgmSlot),
     `0x00469B20` (274B).
   - Next un-dumped 0-FPU `<=6`-call 250..400B pool:
     `0x00420A70` (381B), `0x00443690` (378B), `0x004154B0` (341B),
     `0x00468460` (284B), `0x00412430` (274B), `0x00434EB0` (267B),
     `0x00434D40` (267B), `0x00455F50` (256B).
2. **Wave B — Fresh `/EHsc` constructors, destructors, and factories:**
   Ready pre-dumped in `.analysis/eh-batch-f-disasm.txt`:
   - `0x00413730` (714B, `EndingSub::EndingSub`)
   - `0x0046C5B0` (613B, `DSound::CreateStreamingFromFile`)
   - `0x0046C850` (589B, `DSound::CreateStreamingFromMem`)
   (Total 1,916B in 3 functions.)
3. **Wave C — Remaining VC10 STL instantiations & small/medium pools:**
   Any non-library STL template instantiations (`std::string`, `std::vector`,
   `std::list`) match automatically when exercised under `/Ob1 /Oy- /EHsc`.
   Note: `compare-linked-function.py` currently requires a single PDB section
   contribution of the exact declared size, so multi-contribution `__catch$`
   functions (e.g., `std::string::_Copy` at `0x00453050`, 354B = 177+131+46)
   and `noreturn` trailing-`int3` wrappers (`_Xran`/`_Xlen` 10B vs 11B) need
   either a small `compare-linked-function.py` enhancement or separate handling.
4. **Wave D — Subsystem sweeps (300–999B and 1000+B) & frontier closure:**
   Group functions by subsystem (Stage, Enemy, Bullet, Laser, Player, Gui,
   Title, Replay, Sound, Ending, Anm, Ecl) so shared struct layouts carry over
   between consecutive batches. Revisit the 92 authored source frontiers (e.g.,
   `AnmVmScript`, `AnmDraw`, `AnmTextureLoading`, `TextTexture`) once the
   fresh low-hanging pools are exhausted.

### 2. Fast per-batch iteration & banking loop

1. **Probe build:** Call `ltcg_link.cold_link` with
   `profile=[*LTCG_PROFILE, '/Ob1', '/Oy-']` (add `'/EHsc'` for EH batches;
   test without `'/Oy-'` only if target omits frame pointer) and
   `entry='<Batch>Probe@8'`.
2. **Diff & time-box:** Compare candidate `.text` bytes against `resources/th128.exe`
   via Capstone. **Bank every function that reaches `diff=+0, bad_ins=0`
   immediately.** Do not spend more than 2–3 turns chasing a 1–3 byte
   register-tie or SIB-order quirk; park near-matches and move to fresh targets.
3. **Exact verification & commit sequence (run in exact order):**
   1. Append `[units.<unit_id>]` to `config/match-units.toml` from `source.exe`
      + `source.map` (skip `_th128_ltcg_probe_anchor_*`; mark `__ehhandler$` /
      `__unwindfunclet$` with `eh_static = true`; mark intra-function switch
      jump-table `DIR32` entries with `local = true`).
   2. Record non-exact source mapping first:
      ~~~bash
      scripts/repo-python scripts/record-source.py <unit_id> \
        --module <Batch> --convention ltcg --signature "<sig>" \
        --boundary "0xSTART..0xEND" --notes "<notes>"
      ~~~
   3. Run targeted cold replay (never full cold replay):
      ~~~bash
      scripts/repo-python scripts/replay-exact-units.py --unit <unit_id> [...]
      ~~~
   4. After `difference_count: 0`, re-run `record-source.py` with
      `--exact "compare-linked-function.py --unit <unit_id>"`.
   5. Regenerate progress and validate tracking:
      ~~~bash
      scripts/repo-python scripts/progress.py
      scripts/repo-python scripts/validate-tracking.py --require-target
      ~~~
   6. **Always stage all 6 tracking files together**:
      `config/match-units.toml`, `config/function-boundaries.csv`,
      `config/functions.csv`, `config/implemented.csv`, `config/matches.csv`,
      `config/reccmp-functions.csv`, plus `docs/PROGRESS.md`,
      `resources/progress.svg`, and the batch `src/` + `probes/` files.
      Run `git diff --cached --check` and commit.

## Preserved dirty / untracked WIP on disk

All dirty and untracked WIP files were preserved per `AGENTS.md` (do not run
`git clean -fdx`):

- Tracked modified WIP (pre-existing): `src/EclVmRun.hpp`, `src/EclVmRun.cpp`,
  `probes/ecl-vm-run-context.cpp`.
- Notable near-exact untracked WIP batches (parked to avoid diminishing returns):
  - `src/EhBatchH.*` + `probes/eh-batch-h-context.cpp`: `EbhEnding::Init`
    (`0x004131D0`, 521B) is same-size 521B (`bad=7`, register/SIB swap).
  - `src/EhBatchD.*` + `probes/eh-batch-d-context.cpp`: `EbdReadFile`
    (`0x00450380`, 208B, 2-byte diff) and `EbdGetFileSize` (`0x00450450`, 251B,
    same-size `bad=3`).
  - `src/EhBatchG.*`, `src/EhBatchE.*`, `src/CleanBatchI.*`, plus earlier
    untracked scratch batches.
- Pre-dumped disassemblies ready in `.analysis/`:
  `eh-batch-f-disasm.txt`, `clean-batch-n-disasm.txt`, `clean-batch-l-disasm.txt`,
  `clean-batch-m-disasm.txt`, `clean-batch-k-disasm.txt`.

## References

- Historical checkpoint archive: [RE_CHECKPOINT_NOTES.md](RE_CHECKPOINT_NOTES.md)
- Compiler/target findings and falsified hypotheses: [KNOWLEDGE_BASE.md](KNOWLEDGE_BASE.md)
- Linked Oracle contract: [LINKED_ORACLE.md](LINKED_ORACLE.md)
