#include "CleanSmallWaveD.hpp"

#include <process.h>
#include <stdlib.h>
#include <string.h>

extern "C" __declspec(dllimport) BOOL __stdcall WINNLSEnableIME(HWND hwnd, BOOL bFlag);

namespace th128 {

// 1. 0x00442DF0 (76B)
void CsdDestroyScoreGlobal() {
    CsdScoreGlobal *s = g_CsdScoreGlobal;
    if (s != NULL) {
        if (s->buf00 != NULL) {
            free(s->buf00);
            s->buf00 = NULL;
        }
        if (s->buf04 != NULL) {
            free(s->buf04);
            s->buf04 = NULL;
        }
        operator delete(s);
    }
    g_CsdScoreGlobal = NULL;
}

// 2. 0x00470780 (74B)
int CsdEclStack::Push(const void *valPtr, char tag) {
    if (this->sp + 4 >= 0x1000) {
        return -1;
    }
    if (tag != 0) {
        this->buf[this->sp] = (unsigned char)tag;
        this->sp += 4;
    }
    *(unsigned int *)(&this->buf[this->sp]) = *(const unsigned int *)valPtr;
    this->sp += 4;
    return 0;
}

// 3. 0x0042A500 (73B)
CsdLaserLine::CsdLaserLine() {
    memset(this->pad5c0, 0, sizeof(this->pad5c0));
}

// 4. 0x00451600 (72B)
void CsdFontHolder::RecreateFont() {
    if (this->hFont != NULL) {
        DeleteObject(this->hFont);
        this->hFont = NULL;
    }
    const char *face = this->cap >= 16 ? this->name.ptr : this->name.buf;
    this->hFont = CreateFontA(this->height, this->width, 0, 0, this->weight,
                              0, 0, 0, 1, 0, 0, 0, 0x30, face);
}

// 5. 0x004512F0 (70B)
void CsdThreadObj::StartWorker() {
    this->flags20 &= ~7u;
    this->f4c = 0;
    this->Stop();
    this->proc = (void *)CsdWorkerProc;
    this->f10 = 1;
    this->f0c = 0;
    this->handle = (HANDLE)_beginthreadex(NULL, 0, CsdWorkerProc, NULL, 0, &this->threadId);
}

// 6. 0x0040EA10 (69B)
CsdCardEffectMgr *CsdCreateCardEffectMgr() {
    CsdCardEffectMgr *mgr = new CsdCardEffectMgr();
    if (CsdInitCardEffectMgr(mgr) != 0) {
        if (mgr != NULL) {
            mgr->Shutdown();
            operator delete(mgr);
        }
        return NULL;
    }
    return mgr;
}

// 7. 0x00443600 (68B)
CsdScoreMgr *CsdCreateScoreMgr() {
    CsdScoreMgr *mgr = new CsdScoreMgr();
    if (CsdRegScoreMgr(mgr) != 0) {
        if (mgr != NULL) {
            delete mgr;
        }
        return NULL;
    }
    return mgr;
}

// 8. 0x00456540 (67B)
int __stdcall CsdEnumJoysticksCb(const CsdDeviceInstance *inst, void *ref) {
    (void)ref;
    for (int i = 0; i < 1; ++i) {
        if (g_CsdPads[i] == NULL) {
            g_CsdDirectInput->CreateDevice(&inst->guidInstance, &g_CsdPads[i], NULL);
            return 1;
        }
    }
    return 0;
}

// 9. 0x004518E0 (66B)
void CsdFillTextImageRgb444(unsigned int rgb) {
    unsigned short color = (unsigned short)(((((rgb >> 20) & 0xf) << 4) | ((rgb >> 12) & 0xf)) << 4 | ((rgb >> 4) & 0xf));
    unsigned short *p = (unsigned short *)g_CsdTextBits;
    for (int i = 0; i < g_CsdTextByteLen; i += 2, ++p) {
        *p = color;
    }
}

// 10. 0x00408170 (63B)
CsdBullet::~CsdBullet() {
    if (this->buf99c != NULL) {
        free(this->buf99c);
    }
    this->buf99c = NULL;
    if (this->buf4b4 != NULL) {
        free(this->buf4b4);
    }
    this->buf4b4 = NULL;
}

// 11. 0x004425F0 (63B)
CsdReplayStage::CsdReplayStage() {
    memset(this, 0, sizeof(CsdReplayStage));
    this->ptr18a0 = this->data151c;
    this->self1518 = this;
    this->self18a4 = this;
    this->f18a8 = 0;
    this->f18ac = 0;
}

// 12. 0x00454770 (63B)
void CsdRestoreSystemSettings() {
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, g_CsdSavedScreenSave, NULL, SPIF_SENDCHANGE);
    SystemParametersInfoA(SPI_SETLOWPOWERACTIVE, g_CsdSavedLowPower, NULL, SPIF_SENDCHANGE);
    SystemParametersInfoA(SPI_SETPOWEROFFACTIVE, g_CsdSavedPowerOff, NULL, SPIF_SENDCHANGE);
    WINNLSEnableIME(NULL, TRUE);
}

} // namespace th128
