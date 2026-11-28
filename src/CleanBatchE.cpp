#include "CleanBatchE.hpp"
#include <process.h>
#include <string.h>

CbeChain g_CbeChain;
CbeLog g_CbeLog;
CbeObj41f8c0 *g_CbeObj41f8c0;
CbeObj445330 *g_CbeObj445330;
int g_CbeFlag4d2ad4;
int g_CbeTexFmt;
int g_CbeTexPitch;
unsigned char *g_CbeTexBuf;

static inline CbeChainElem *CbeMakeElemFlags3(void *cb, void *arg) {
    CbeChainElem *e = new CbeChainElem();
    e->flags |= 3;
    e->cb = cb;
    e->onReg = 0;
    e->onCut = 0;
    e->arg = arg;
    return e;
}

static inline CbeChainElem *CbeMakeElemFlags1(void *cb, void *arg) {
    CbeChainElem *e = new CbeChainElem();
    e->cb = cb;
    e->onReg = 0;
    e->onCut = 0;
    e->arg = arg;
    e->flags = (e->flags & ~2U) | 1U;
    return e;
}

inline CbeObj41f8c0::CbeObj41f8c0() {
    memset(this, 0, sizeof(CbeObj41f8c0));
    flags |= 2;
    g_CbeObj41f8c0 = this;
}

// 1. 0x0041f8c0 (137B)
CbeObj41f8c0 *CbeCreate41f8c0() {
    CbeObj41f8c0 *obj = new CbeObj41f8c0();
    CbeChainElem *d = CbeMakeElemFlags3((void *)&CbeCb41fb70, obj);
    g_CbeChain.RegisterDraw(d, 0x43);
    obj->drawElem = d;
    return obj;
}

// 2. 0x0040e820 (232B)
int __stdcall CbeInit40e820(CbeObj40e820 *obj) {
    obj->anm = CbeLoadAnm(0x24, "ice.anm");
    if (obj->anm == 0) {
        g_CbeLog.Print("\x83" "f\x81[\x83^\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x81" "B\x83" "f\x81[\x83^\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");
        return -1;
    }
    CbeChainElem *c = CbeMakeElemFlags3((void *)&CbeCb4101b0, obj);
    g_CbeChain.RegisterCalc(c, 0x12);
    obj->calcElem = c;
    CbeChainElem *d = CbeMakeElemFlags3((void *)&CbeCb4101c0, obj);
    g_CbeChain.RegisterDraw(d, 0x18);
    obj->drawElem = d;
    return 0;
}

// 3. 0x00411e50 (245B)
int __stdcall CbeInit411e50(CbeObj411e50 *obj) {
    obj->anm = CbeLoadAnm(6, "bullet.anm");
    if (obj->anm == 0) {
        g_CbeLog.Print("\x89\xe6\x96\xca\x8d\\\x90\xac\x83" "f\x81[\x83^\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x81" "B\x83" "f\x81[\x83^\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");
        return -1;
    }
    CbeChainElem *c = CbeMakeElemFlags1((void *)&CbeCb4120e0, obj);
    g_CbeChain.RegisterCalc(c, 0x1b);
    obj->calcElem = c;
    CbeChainElem *d = CbeMakeElemFlags1((void *)&CbeCb4120f0, obj);
    g_CbeChain.RegisterDraw(d, 0x23);
    obj->drawElem = d;
    return 0;
}

inline void CbeThreadObj::Start(unsigned int (__stdcall *fn)(void *), void *arg) {
    Reset();
    proc = (void *)fn;
    flag10 = 1;
    flag0c = 0;
    handle = _beginthreadex(0, 0, fn, arg, 0, &tid);
}

// 4. 0x00433110 (245B)
int __stdcall CbeInit433110(CbeObj433110 *obj) {
    CbeChainElem *c = CbeMakeElemFlags1((void *)&CbeCb4335b0, obj);
    g_CbeChain.RegisterCalc(c, 3);
    obj->calcElem = c;
    CbeChainElem *d = CbeMakeElemFlags1((void *)&CbeCb4335c0, obj);
    g_CbeChain.RegisterDraw(d, 0x3c);
    obj->drawElem = d;
    obj->thread.Start(&CbeThread432f50, obj);
    return 0;
}

// 5. 0x00429ea0 (257B)
int __stdcall CbeInit429ea0(CbeObj429ea0 *obj) {
    obj->anm = CbeLoadAnm(6, "bullet.anm");
    if (obj->anm == 0) {
        g_CbeLog.Print("\x93" "G\x92" "e\x83" "f\x81[\x83^\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x81" "B\x83" "f\x81[\x83^\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");
        return -1;
    }
    CbeChainElem *c = CbeMakeElemFlags1((void *)&CbeCb42a370, obj);
    g_CbeChain.RegisterCalc(c, 0x17);
    obj->calcElem = c;
    CbeChainElem *d = CbeMakeElemFlags1((void *)&CbeCb42a3e0, obj);
    g_CbeChain.RegisterDraw(d, 0x1e);
    obj->drawElem = d;
    obj->ptr5d0 = obj->arr10;
    return 0;
}

// 6. 0x00445330 (278B)
int CbeInit445330() {
    CbeObj445330 *obj = g_CbeObj445330;
    CbeChainElem *c = CbeMakeElemFlags1((void *)&CbeCb445c80, obj);
    g_CbeChain.RegisterCalc(c, 6);
    obj->calcElem = c;
    CbeChainElem *d = CbeMakeElemFlags1((void *)&CbeCb445c90, obj);
    g_CbeChain.RegisterDraw(d, 0x3d);
    obj->drawElem = d;
    obj->anm14 = CbeLoadAnm(0x18, "title.anm");
    if (obj->anm14 == 0) {
        g_CbeLog.Print("\x83" "f\x81[\x83^\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");
        return -1;
    }
    obj->anm18 = CbeLoadAnm(0x19, "title_v.anm");
    if (obj->anm18 == 0) {
        g_CbeLog.Print("\x83" "f\x81[\x83^\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");
        return -1;
    }
    obj->flagf8 = 1;
    g_CbeFlag4d2ad4 = 0;
    return 0;
}

// 7. 0x00451d00 (166B)
bool __stdcall CbeInvertAlpha(int height) {
    int bytes = g_CbeTexPitch * height;
    unsigned char *buf = g_CbeTexBuf;
    if (g_CbeTexFmt == 0x15) {
        for (int i = 3; i < bytes; i += 4) {
            buf[i] = ~buf[i];
        }
        return true;
    } else if (g_CbeTexFmt == 0x19) {
        CbePixel1555 *p = (CbePixel1555 *)buf;
        for (int i = 0; i < bytes; i += 2, ++p) {
            p->a ^= 1;
            if (p->a == 0) {
                p->rgb = 0;
            }
        }
        return true;
    } else if (g_CbeTexFmt == 0x1a) {
        for (int i = 1; i < bytes; i += 2) {
            buf[i] ^= 0xf0;
        }
        return true;
    }
    return false;
}
