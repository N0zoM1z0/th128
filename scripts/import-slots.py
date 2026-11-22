#!/usr/bin/env python3
"""Print the DLL and function name of target import-address-table slots."""
import struct,sys
from pathlib import Path
d=open(Path(__file__).resolve().parents[1] / 'resources/th128.exe','rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]; opt=pe+24
imp_rva,_=struct.unpack_from('<II',d,opt+96+8)
sh=opt+struct.unpack_from('<H',d,pe+20)[0]
secs=[struct.unpack_from('<8sIIII',d,sh+40*i)[1:4]+(struct.unpack_from('<I',d,sh+40*i+20)[0],) for i in range(4)]
def off(r):
  for vs,va,rs,ro in secs:
    if va<=r<va+vs: return r-va+ro
o=off(imp_rva); m={}
while True:
  ilt,ts,fc,name,iat=struct.unpack_from('<IIIII',d,o)
  if not name: break
  dll=d[off(name):d.index(b'\0',off(name))].decode(); t=ilt or iat; i=0
  while True:
    e=struct.unpack_from('<I',d,off(t)+4*i)[0]
    if not e: break
    fn = f"#{e&0xffff}" if e&0x80000000 else d[off(e)+2:d.index(b'\0',off(e)+2)].decode()
    m[0x400000+iat+4*i]=(dll,fn); i+=1
  o+=20
for a in sys.argv[1:]: print(a, m.get(int(a,16)))
