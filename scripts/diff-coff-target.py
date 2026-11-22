#!/usr/bin/env python3
"""Side-by-side disassembly: object symbol vs target extent (diagnostic only)."""
import importlib.util, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from target_identity import pe_bytes_at
ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('cc', ROOT / 'scripts/compare-coff-function.py')
cc = importlib.util.module_from_spec(spec); spec.loader.exec_module(cc)
obj, sym, addr, size = sys.argv[1], sys.argv[2], int(sys.argv[3], 0), int(sys.argv[4], 0)
code, _ = cc.object_function(Path(obj), sym)
target = pe_bytes_at(open(ROOT / 'resources/th128.exe', 'rb').read(), addr, size)
md = Cs(CS_ARCH_X86, CS_MODE_32)
a = [f"{i.address:3x} {i.mnemonic} {i.op_str}" for i in md.disasm(bytes(code), 0)]
b = [f"{i.address:3x} {i.mnemonic} {i.op_str}" for i in md.disasm(target, 0)]
for i in range(max(len(a), len(b))):
    l = a[i] if i < len(a) else ''; r = b[i] if i < len(b) else ''
    print(f"{'  ' if l[4:]==r[4:] else '* '}{l:45s} | {r}")
