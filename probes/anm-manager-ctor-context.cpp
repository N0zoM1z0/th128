#include "AnmManagerCtor.hpp"
#include <windows.h>

volatile int g_AmcSink = 0;

__declspec(noinline) AmcAnmVm::AmcAnmVm() {
    data[0] = 0;
    g_AmcSink += 1;
}

__declspec(noinline) AmcAnmVm::~AmcAnmVm() {
    g_AmcSink -= 1;
}

__declspec(noinline) void AmcAnmVm::Reset() {
    DWORD t = GetTickCount();
    data[0] = (unsigned char)t;
    data[1] = (unsigned char)(t >> 8);
    g_AmcSink ^= (int)this + (int)t;
}

#define DEF_CB(name, n) __declspec(noinline) int __fastcall name(void *p) { g_AmcSink += n + (int)p; return n; }
DEF_CB(AmcCalc0, 1)
DEF_CB(AmcCalc1, 2)
DEF_CB(AmcDraw0, 3)
DEF_CB(AmcDraw1, 4)
DEF_CB(AmcDraw2, 5)
DEF_CB(AmcDraw3, 6)
DEF_CB(AmcDraw4, 7)
DEF_CB(AmcDraw5, 8)
DEF_CB(AmcDraw6, 9)
DEF_CB(AmcDraw7, 10)
DEF_CB(AmcDraw8, 11)
DEF_CB(AmcDraw9, 12)
DEF_CB(AmcDraw10, 13)
DEF_CB(AmcDraw11, 14)
DEF_CB(AmcDraw12, 15)
DEF_CB(AmcDraw13, 16)
DEF_CB(AmcDraw14, 17)
DEF_CB(AmcDraw15, 18)
DEF_CB(AmcDraw16, 19)
DEF_CB(AmcDraw17, 20)
DEF_CB(AmcDraw18, 21)
DEF_CB(AmcDraw19, 22)
DEF_CB(AmcDraw20, 23)
DEF_CB(AmcDraw21, 24)
DEF_CB(AmcDraw22, 25)
DEF_CB(AmcDraw23, 26)
DEF_CB(AmcDraw24, 27)
DEF_CB(AmcDraw25, 28)
DEF_CB(AmcDraw26, 29)
DEF_CB(AmcDraw27, 30)
DEF_CB(AmcDraw28, 31)

extern "C" int __stdcall AnmManagerCtorProbe(AmcD3dDevice *dev, Chain *ch, int mode) {
    g_AmcD3dDevice = dev;
    g_Chain = ch;
    AmcAnmManager *mgr = new AmcAnmManager();
    if (mode & 1) {
        mgr->vms[0].Reset();
    }
    return (int)mgr + g_AmcSink + (int)g_AmcQuadVerts[0].rhw + (int)g_AmcColorQuadVerts[0].rhw;
}
