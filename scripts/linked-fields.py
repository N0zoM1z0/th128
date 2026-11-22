#!/usr/bin/env python3
"""List the decoded linked fields of one owner in a cold linked-PE build (diagnostic).

Prints offset, type and candidate symbols so a unit's `linkages` can be
written; each target address still needs independent review.
"""
import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from linked_image import PEImage, linked_code_fields  # noqa: E402

spec = importlib.util.spec_from_file_location("compare_linked", ROOT / "scripts/compare-linked-function.py")
compare = importlib.util.module_from_spec(spec)
spec.loader.exec_module(compare)

directory, symbol, size = Path(sys.argv[1]).resolve(), sys.argv[2], int(sys.argv[3], 0)
image = PEImage(directory / "source.exe")
publics = compare.map_publics(directory / "source.map")
owner = compare.owner(image, directory / "source.map", directory / "source.pdb", symbol, size)
for field in linked_code_fields(image, publics, int(owner["address"], 0), size)["fields"]:
    names = [n for n in field.get("candidate_symbols", []) if "_ltcg_probe_anchor_" not in n]
    print(field["offset"], field["type"], " ".join(names))
