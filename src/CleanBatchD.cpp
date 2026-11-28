#include "CleanBatchD.hpp"

CbdChain g_CbdChain;
CbdBgmPlayer *g_CbdBgm;
HANDLE g_CbdBgmEvent;
unsigned int g_CbdSupFlags;
int g_CbdFrameSkip;
CbdLoadingObj *g_CbdLoading;
CbdAnmMgr *g_CbdAnmMgr;
CbdChainOwner *g_CbdTitleObj;
int g_CbdFlag4d59e8;

static inline CbdChainElem *CbdMakeElemFlags3(void *cb, void *arg) {
    CbdChainElem *e = new CbdChainElem();
    e->flags |= 3;
    e->cb = cb;
    e->onReg = 0;
    e->onCut = 0;
    e->arg = arg;
    return e;
}

static inline CbdChainElem *CbdMakeElemFlags1(void *cb, void *arg) {
    CbdChainElem *e = new CbdChainElem();
    e->cb = cb;
    e->onReg = 0;
    e->onCut = 0;
    e->arg = arg;
    e->flags = (e->flags & ~2U) | 1U;
    return e;
}

// 1. 0x00407490 (180B)
int __stdcall CbdReg407490(CbdChainOwner *obj) {
    CbdChainElem *c = CbdMakeElemFlags3((void *)&CbdCb407790, obj);
    g_CbdChain.RegisterCalc(c, 0x14);
    obj->calcElem = c;
    CbdChainElem *d = CbdMakeElemFlags3((void *)&CbdCb4077e0, obj);
    g_CbdChain.RegisterDraw(d, 0x25);
    obj->drawElem = d;
    return 0;
}

// 2. 0x00411040 (180B)
int __stdcall CbdReg411040(CbdChainOwner *obj) {
    CbdChainElem *c = CbdMakeElemFlags3((void *)&CbdCb411bb0, obj);
    g_CbdChain.RegisterCalc(c, 5);
    obj->calcElem = c;
    CbdChainElem *d = CbdMakeElemFlags3((void *)&CbdCb411bc0, obj);
    g_CbdChain.RegisterDraw(d, 0x30);
    obj->drawElem = d;
    return 0;
}

// 3. 0x00443430 (180B)
int __stdcall CbdReg443430(CbdChainOwner *obj) {
    CbdChainElem *c = CbdMakeElemFlags3((void *)&CbdCb4438f0, obj);
    g_CbdChain.RegisterCalc(c, 7);
    obj->calcElem = c;
    CbdChainElem *d = CbdMakeElemFlags3((void *)&CbdCb443900, obj);
    g_CbdChain.RegisterDraw(d, 0x49);
    obj->drawElem = d;
    return 0;
}

// 4. 0x00444240 (180B)
int __stdcall CbdReg444240(CbdChainOwner *obj) {
    CbdChainElem *c = CbdMakeElemFlags3((void *)&CbdCb444c90, obj);
    g_CbdChain.RegisterCalc(c, 5);
    obj->calcElem = c;
    CbdChainElem *d = CbdMakeElemFlags3((void *)&CbdCb444ca0, obj);
    g_CbdChain.RegisterDraw(d, 0x30);
    obj->drawElem = d;
    return 0;
}

// 5. 0x00428360 (196B)
int __stdcall CbdReg428360(CbdChainOwner *obj) {
    CbdChainElem *c = CbdMakeElemFlags1((void *)&CbdCb429360, obj);
    g_CbdChain.RegisterCalc(c, 0x19);
    obj->calcElem = c;
    CbdChainElem *d = CbdMakeElemFlags1((void *)&CbdCb4293a0, obj);
    g_CbdChain.RegisterDraw(d, 0x1c);
    obj->drawElem = d;
    return 0;
}

// 6. 0x0041fc80 (207B)
void __stdcall CbdUpdateMeterA(CbdMeterOwner *obj, int fullCount, int partial) {
    int i;
    for (i = 0; i < fullCount; ++i) {
        if (obj->vmsA[i].cb != 0) {
            obj->vmsA[i].cb(&obj->vmsA[i], 2);
        }
        obj->vmsA[i].state3f8 = 2;
    }
    if ((unsigned int)i < 8) {
        CbdAnmVm *vm = &obj->vmsA[i];
        short s = (short)(partial + 7);
        if (vm->cb != 0) {
            vm->cb(vm, s);
        }
        vm->state3f8 = s;
        ++i;
    }
    for (; (unsigned int)i < 8; ++i) {
        if (obj->vmsA[i].cb != 0) {
            obj->vmsA[i].cb(&obj->vmsA[i], 3);
        }
        obj->vmsA[i].state3f8 = 3;
    }
}

// 7. 0x0041fd50 (208B)
void __stdcall CbdUpdateMeterB(CbdMeterOwner *obj, int fullCount, int partial) {
    int i;
    for (i = 0; i < fullCount; ++i) {
        if (obj->vmsB[i].cb != 0) {
            obj->vmsB[i].cb(&obj->vmsB[i], 2);
        }
        obj->vmsB[i].state3f8 = 2;
    }
    if ((unsigned int)i < 8) {
        CbdAnmVm *vm = &obj->vmsB[i];
        short s = (short)(partial + 7);
        if (vm->cb != 0) {
            vm->cb(vm, s);
        }
        vm->state3f8 = s;
        ++i;
    }
    for (; (unsigned int)i < 8; ++i) {
        if (obj->vmsB[i].cb != 0) {
            obj->vmsB[i].cb(&obj->vmsB[i], 3);
        }
        obj->vmsB[i].state3f8 = 3;
    }
}

// 8. 0x00458f30 (181B)
DWORD __stdcall CbdBgmThreadProc(void *param) {
    int done = 0;
    MSG msg;
    while (!done) {
        DWORD res = MsgWaitForMultipleObjects(1, &g_CbdBgmEvent, FALSE, INFINITE, QS_ALLEVENTS);
        if (g_CbdBgm == 0) {
            done = 1;
        }
        switch (res) {
        case WAIT_OBJECT_0:
            if (g_CbdBgm != 0 && g_CbdBgm->active30 != 0) {
                g_CbdBgm->busy78 = 1;
                CbdBgmFillBuffer(g_CbdBgm);
                g_CbdBgm->busy78 = 0;
            }
            break;
        case WAIT_OBJECT_0 + 1:
            while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    done = 1;
                }
            }
            break;
        }
    }
    return 0;
}

// 9. 0x00445280 (167B)
int CbdTitleInitWait() {
    if (CbdTitleCheck() != 0) {
        g_CbdFrameSkip = (g_CbdSupFlags & 0x2000) ? 2 : 3;
        return 0;
    }
    if (g_CbdLoading != 0) {
        while (g_CbdLoading->frames528 < 180 && (g_CbdSupFlags & 0x180) == 0) {
            Sleep(16);
        }
        if (g_CbdAnmMgr->loadingSlot != 0) {
            g_CbdAnmMgr->loadingSlot->~CbdAnmSlot();
            ::operator delete(g_CbdAnmMgr->loadingSlot);
            g_CbdAnmMgr->loadingSlot = 0;
        }
    }
    g_CbdTitleObj->drawElem->flags |= 2;
    g_CbdFlag4d59e8 = 1;
    return 0;
}
