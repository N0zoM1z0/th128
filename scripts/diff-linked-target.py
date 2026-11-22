#!/usr/bin/env python3
"""Side-by-side disassembly of one linked-PE owner against the target (diagnostic only).

Usage: diff-linked-target.py BUILD_DIR SYMBOL ADDRESS SIZE
"""
import sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
from linked_image import linked_functions
from target_identity import pe_bytes_at
from capstone import *
d,sym,addr,size=sys.argv[1],sys.argv[2],int(sys.argv[3],0),int(sys.argv[4],0)
img=open(d+'/source.exe','rb').read(); t=open(ROOT / 'resources/th128.exe','rb').read(); md=Cs(CS_ARCH_X86,CS_MODE_32)
f=[f for f in linked_functions(d+'/source.exe',d+'/source.map',d+'/source.pdb')['functions'] if f['symbol']==sym][0]
a=int(f['address'],16); off=a-0x401000+0x400; c=img[off:off+f['size']]
A=[f"{i.mnemonic} {i.op_str}" for i in md.disasm(c,0)]; B=[f"{i.mnemonic} {i.op_str}" for i in md.disasm(pe_bytes_at(t,addr,size),0)]
print('size',f['size'],'vs',size)
for i in range(max(len(A),len(B))):
    l=A[i] if i<len(A) else ''; r=B[i] if i<len(B) else ''
    print(('  ' if l==r else '* ')+f"{l:42s} | {r}")
