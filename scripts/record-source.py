#!/usr/bin/env python3
"""Record a maintained source mapping, reviewed boundary and optional exact claim.

The canonical unit in config/match-units.toml supplies name, extent and source.
Exact rows are written only with --exact, after the unit's comparator passed.
"""
from __future__ import annotations

import argparse
import csv
from pathlib import Path
import tomllib

ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "config"


def load(name):
    with (CONFIG / name).open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        return reader.fieldnames, list(reader)


def save(name, fields, rows):
    with (CONFIG / name).open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("unit")
    parser.add_argument("--module", required=True)
    parser.add_argument("--convention", required=True)
    parser.add_argument("--signature", required=True)
    parser.add_argument("--notes", required=True)
    parser.add_argument("--boundary", required=True, help="boundary review note")
    parser.add_argument("--boundary-confidence", default="high", choices=["high", "medium"])
    parser.add_argument("--exact", metavar="EVIDENCE", help="exact evidence text")
    args = parser.parse_args()
    with (CONFIG / "match-units.toml").open("rb") as stream:
        unit = tomllib.load(stream)["units"][args.unit]
    address, size, name = int(unit["target_address"]), int(unit["size"]), unit["functions"][0]
    key = f"0x{address:08X}"
    source = unit.get("pdb_source", unit["source"])

    fields, rows = load("functions.csv")
    row = next(r for r in rows if r["address"] == key)
    if int(row["size"]) != size:
        raise SystemExit(f"inventory size {row['size']} differs from unit size {size}")
    row.update(proposed_name=name, module=args.module, calling_convention=args.convention,
               signature=args.signature, source_file=source,
               evidence="Direct hash-attested target extent, neighbor and reference review.",
               notes=args.notes + f" Canonical unit {args.unit}.")
    if args.exact:
        row.update(status="exact", match_percent="100.00", evidence=row["evidence"] + " " + args.exact)
    save("functions.csv", fields, rows)

    fields, rows = load("function-boundaries.csv")
    row = next(r for r in rows if r["address"] == key)
    row.update(state="reviewed", confidence=args.boundary_confidence,
               evidence_id=f"{args.unit}-boundary", notes=args.boundary)
    save("function-boundaries.csv", fields, rows)

    fields, rows = load("reccmp-functions.csv")
    rows = [r for r in rows if r["address"] != key] + [dict(name=name, address=key, type="function")]
    save("reccmp-functions.csv", fields, sorted(rows, key=lambda r: int(r["address"], 0)))
    names = sorted({r["name"] for r in rows})
    (CONFIG / "implemented.csv").write_text("".join(n + "\n" for n in names))

    if args.exact:
        fields, rows = load("matches.csv")
        rows = [r for r in rows if r["address"] != key] + [dict(
            address=key, name=name, size=str(size), status="exact", match_percent="100.00",
            unit=args.unit, evidence=args.exact)]
        save("matches.csv", fields, sorted(rows, key=lambda r: int(r["address"], 0)))
    print(f"recorded {args.unit}: {name} at {key}{' (exact)' if args.exact else ''}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
