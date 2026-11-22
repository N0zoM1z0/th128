#!/usr/bin/env python3
"""Offline context for a function from the Ghidra architecture export (diagnostic)."""
import csv, sys, collections
from pathlib import Path
A = str(Path(__file__).resolve().parents[1] / '.analysis/architecture') + '/'
edges = list(csv.DictReader(open(A + 'call-edges.csv')))
strings = collections.defaultdict(list)
for r in csv.DictReader(open(A + 'string-refs.csv')): strings[int(r['function'], 16)].append(r['value'])
globs = collections.defaultdict(list)
for r in csv.DictReader(open(A + 'global-refs.csv')): globs[int(r['function'], 16)].append(r['symbol'])
callers = collections.defaultdict(set); callees = collections.defaultdict(set)
for e in edges:
    if not e['callee'].startswith('0x'): continue
    callers[int(e['callee'], 16)].add(int(e['caller'], 16)); callees[int(e['caller'], 16)].add(int(e['callee'], 16))
def show(a, depth):
    print(f"{a:#x}: callers={[hex(c) for c in sorted(callers[a])][:12]}")
    for c in sorted(callers[a])[:8]:
        s = strings.get(c, [])[:6]; g = [x for x in globs.get(c, []) if 'vftable' in x][:3]
        print(f"   caller {c:#x}: strings={s} vtables={g}")
        if depth > 1:
            for cc in sorted(callers[c])[:4]:
                print(f"      <- {cc:#x}: {strings.get(cc, [])[:5]}")
for a in sys.argv[1:]: show(int(a, 16), 2)
