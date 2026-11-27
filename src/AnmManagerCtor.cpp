#include "AnmManagerCtor.hpp"
#include <string.h>

AmcVertex g_AmcQuadVerts[4];
AmcColorVertex g_AmcColorQuadVerts[4];
AmcD3dDevice *g_AmcD3dDevice;

static inline void AmcRegCalc(void *self, ChainCallback fn, int prio) {
    ChainElem *e = new ChainElem();
    e->flags |= 3;
    e->SetCallback(fn);
    e->arg = self;
    g_Chain->RegisterCalc(e, prio);
}

static inline void AmcRegDraw(void *self, ChainCallback fn, int prio) {
    ChainElem *e = new ChainElem();
    e->flags |= 3;
    e->SetCallback(fn);
    e->arg = self;
    g_Chain->RegisterDraw(e, prio);
}

AmcAnmManager::AmcAnmManager() {
    memset(this, 0, sizeof(AmcAnmManager));

    g_AmcQuadVerts[0].rhw = g_AmcQuadVerts[1].rhw = g_AmcQuadVerts[2].rhw = g_AmcQuadVerts[3].rhw = 1.0f;
    g_AmcQuadVerts[0].u = 0.0f;
    g_AmcQuadVerts[0].v = 0.0f;
    g_AmcQuadVerts[1].u = 1.0f;
    g_AmcQuadVerts[1].v = 0.0f;
    g_AmcQuadVerts[2].u = 0.0f;
    g_AmcQuadVerts[2].v = 1.0f;
    g_AmcQuadVerts[3].u = 1.0f;
    g_AmcQuadVerts[3].v = 1.0f;

    g_AmcColorQuadVerts[0].rhw = g_AmcColorQuadVerts[1].rhw = g_AmcColorQuadVerts[2].rhw = g_AmcColorQuadVerts[3].rhw = 1.0f;
    g_AmcColorQuadVerts[0].u = 0.0f;
    g_AmcColorQuadVerts[0].v = 0.0f;
    g_AmcColorQuadVerts[1].u = 1.0f;
    g_AmcColorQuadVerts[1].v = 0.0f;
    g_AmcColorQuadVerts[2].u = 0.0f;
    g_AmcColorQuadVerts[2].v = 1.0f;
    g_AmcColorQuadVerts[3].u = 1.0f;
    g_AmcColorQuadVerts[3].v = 1.0f;

    field4e9698 = 0;
    field4e9688 = 0;
    field4e968c = 0;
    field4e968d = 0;
    field4e9684 = 1;
    field4e968e = 0;
    field4e9690 = 0xff;
    field4e968f = 0;
    slot00 = -1;
    slot28 = -1;
    slot50 = -1;
    slot78 = -1;

    for (int i = 0; i < 0x1000; ++i) {
        vms[i].Reset();
    }

    AmcRegCalc(this, AmcCalc0, 30);
    AmcRegCalc(this, AmcCalc1, 8);
    AmcRegDraw(this, AmcDraw0, 4);
    AmcRegDraw(this, AmcDraw1, 6);
    AmcRegDraw(this, AmcDraw2, 8);
    AmcRegDraw(this, AmcDraw3, 9);
    AmcRegDraw(this, AmcDraw4, 11);
    AmcRegDraw(this, AmcDraw5, 13);
    AmcRegDraw(this, AmcDraw6, 16);
    AmcRegDraw(this, AmcDraw7, 17);
    AmcRegDraw(this, AmcDraw8, 18);
    AmcRegDraw(this, AmcDraw9, 19);
    AmcRegDraw(this, AmcDraw10, 20);
    AmcRegDraw(this, AmcDraw11, 23);
    AmcRegDraw(this, AmcDraw12, 26);
    AmcRegDraw(this, AmcDraw13, 27);
    AmcRegDraw(this, AmcDraw14, 29);
    AmcRegDraw(this, AmcDraw15, 31);
    AmcRegDraw(this, AmcDraw16, 36);
    AmcRegDraw(this, AmcDraw17, 38);
    AmcRegDraw(this, AmcDraw18, 40);
    AmcRegDraw(this, AmcDraw19, 43);
    AmcRegDraw(this, AmcDraw20, 44);
    AmcRegDraw(this, AmcDraw21, 56);
    AmcRegDraw(this, AmcDraw22, 68);
    AmcRegDraw(this, AmcDraw23, 69);
    AmcRegDraw(this, AmcDraw24, 54);
    AmcRegDraw(this, AmcDraw25, 53);
    AmcRegDraw(this, AmcDraw26, 57);
    AmcRegDraw(this, AmcDraw27, 71);
    AmcRegDraw(this, AmcDraw28, 70);

    g_AmcD3dDevice->vtbl->SetSamplerOrState(g_AmcD3dDevice, 0);
}
