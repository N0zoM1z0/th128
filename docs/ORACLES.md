# Oracle policy

| Claim | Required falsification mechanism |
| --- | --- |
| Target identity | Exact size, SHA-256/SHA-1/MD5, PE sections, complete mapped `.text` SHA-256 and Rich record stream against `config/target.toml` |
| Ghidra routing | Hash-pinned Ghidra/JDK, original module metadata, image base/entry, distributed mapped samples and the complete mapped `.text` SHA-256 |
| Boundary and origin | Complete target flow and all reviewed references, tables and adjacent bytes; provisional while unresolved |
| Compiler candidate | TH128 PE/Rich evidence, pinned component hashes and banners, and the headless toolchain smoke |
| Source presence | Maintained source mapping; grants no byte credit |
| Normal-COFF exactness | Cold VC10 object, source/tool/object receipt, full owned extent, exhaustive relocation replay, zero byte differences |
| LTCG exactness | Cold VC10 linked family receipt, matching PE/MAP/PDB identity, complete public primary-TU contribution, exhaustive linked-field replay, zero differences |

The original `.text` occupies inclusive addresses `0x00401000..0x00499C14`
(625,685 bytes) with SHA-256
`5cc95be82e3a29600990c2c6fd915aa3a9b6f9f010d2f992c1d986660c96e9f8`.
No target instruction dump is committed. Ghidra check and query commands
compare that entire range with the loaded database in addition to six
distributed mapping probes.

## Normal COFF

`compare-coff-function.py OBJECT SYMBOL ADDRESS SIZE --json` is a diagnostic:
a relocation-masked similarity score has no acceptance authority.
`--unit NAME` requires the declared canonical context in
`config/match-units.toml` and a current artifact receipt. The extent comes
from a COFF function-definition auxiliary record or one complete
single-function COMDAT section, never a caller-chosen slice. Every COFF
relocation must match offset, type and symbol and resolve to its separately
reviewed target value; all bytes must agree after replay. A normal-COFF match
does not establish the original TU partition or `/GL` ownership.

## Linked PE (LTCG)

TH128's Rich header records 56 LTCG C++ objects, so most game code is
expected to live in `/GL` owners whose machine code exists only after
link-time code generation. `compare-linked-function.py --unit NAME` is the
acceptance lane for those owners; see [LINKED_ORACLE.md](LINKED_ORACLE.md).

## Controls

`scripts/test-exact-oracle.py` (8 cases) and `scripts/test-linked-oracle.py`
(27 cases) build synthetic COFF and PE32/MSF7/PDB fixtures with no game bytes
or historical compiler. They reject wrong final bytes, truncated or escaping
extents, undeclared or wrong relocations, wrong owners and addends, stale
receipts and changed tools. Run them when the comparison, receipt or build
infrastructure changes; they are not a per-function requirement. Only private
target-bound cold replays provide actual exactness evidence.
