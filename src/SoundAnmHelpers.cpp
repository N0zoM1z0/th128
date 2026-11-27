#include "SoundAnmHelpers.hpp"

#include <math.h>
#include <string.h>

extern "C" {
typedef unsigned long (__stdcall *SaThreadStartRoutine)(void *);

struct SaWinMsg {
    void *hwnd;
    unsigned int message;
    unsigned int wParam;
    long lParam;
    unsigned long time;
    long ptX;
    long ptY;
};

__declspec(dllimport) void *__stdcall CreateThread(
    void *lpThreadAttributes,
    unsigned long dwStackSize,
    SaThreadStartRoutine lpStartAddress,
    void *lpParameter,
    unsigned long dwCreationFlags,
    unsigned long *lpThreadId);
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);
__declspec(dllimport) void __stdcall Sleep(unsigned long dwMilliseconds);
__declspec(dllimport) int __stdcall CloseHandle(void *hObject);
__declspec(dllimport) unsigned long __stdcall MsgWaitForMultipleObjects(
    unsigned long nCount,
    void *const *pHandles,
    int fWaitAll,
    unsigned long dwMilliseconds,
    unsigned long dwWakeMask);
__declspec(dllimport) int __stdcall PeekMessageA(
    SaWinMsg *lpMsg,
    void *hWnd,
    unsigned int wMsgFilterMin,
    unsigned int wMsgFilterMax,
    unsigned int wRemoveMsg);
}

namespace th128 {

void SaNoOp4575C0() {
}

int __stdcall SaInitSoundThread(void *arg) {
    memset(&g_SaSoundMgr, 0, sizeof(g_SaSoundMgr));
    g_SaSoundArg = arg;
    g_SaSoundThread = CreateThread(0, 0, SaSoundThreadProc, &g_SaSoundMgr, 0, &g_SaSoundThreadId);
    return 0;
}

int SaRequestSoundStop() {
    g_SaSoundState = 2;
    return 0;
}

int SaShutdownSoundThreads() {
    if (g_SaSoundThread != 0) {
        if (g_SaSoundState == 0) {
            g_SaSoundState = 1;
        }
        while (WaitForSingleObject(g_SaSoundThread, 100) == 0x102) {
            Sleep(1);
        }
        while (WaitForSingleObject(g_SaSfxThread, 100) == 0x102) {
            Sleep(1);
        }
        CloseHandle(g_SaSoundThread);
        CloseHandle(g_SaSfxThread);
        g_SaSoundThread = 0;
        g_SaSfxThread = 0;
    }
    return 0;
}

void SaStartSfxThread() {
    unsigned long tid;
    g_SaSfxThread = CreateThread(0, 0, (SaThreadStartRoutine)SaSfxLoadThread, &g_SaSoundMgr, 0, &tid);
}

int __stdcall SaLoadBgmTable(SaSoundMgr *mgr) {
    void *p = SaLoadFile(g_SaBgmTableName, 0, 0);
    mgr->bgmTable = p;
    return p != 0 ? 0 : -1;
}

unsigned long __stdcall SaBgmThreadProc(void *param) {
    (void)param;
    int done = 0;
    do {
        unsigned long r = MsgWaitForMultipleObjects(1, &g_SaBgmEvent, 0, 0xffffffff, 0x4bf);
        if (g_SaBgmStream == 0) {
            done = 1;
        }
        switch (r) {
        case 0:
            if (g_SaBgmStream != 0 && g_SaBgmStream->isPlaying != 0) {
                g_SaBgmStream->inNotify = 1;
                SaHandleStreamNotify(g_SaBgmStream);
                g_SaBgmStream->inNotify = 0;
            }
            break;
        case 1: {
            SaWinMsg msg;
            while (PeekMessageA(&msg, 0, 0, 0, 1)) {
                if (msg.message == 0x12) {
                    done = 1;
                }
            }
            break;
        }
        }
    } while (!done);
    return 0;
}

float __stdcall SaSinF(float x) {
    return (float)sin(x);
}

void __stdcall SaClearD3DTexture(SaD3DTexture **ppTex) {
    SaD3DSurface *surf;
    SaD3DSurfaceDesc desc;
    SaD3DLockedRect rect;
    (*ppTex)->vtbl->GetSurfaceLevel(*ppTex, 0, &surf);
    surf->vtbl->GetDesc(surf, &desc);
    surf->vtbl->LockRect(surf, &rect, 0, 0);
    memset(rect.pBits, 0, rect.pitch * desc.height);
    surf->vtbl->UnlockRect(surf);
    if (surf != 0) {
        surf->vtbl->Release(surf);
    }
}

SaStreamObj::~SaStreamObj() {
    Cleanup();
}

int __stdcall SaHitTestOrientedRect(float cx, float cy, float w, float h, float angle, float px, float py) {
    float dx = px - cx;
    float dy = py - cy;
    float negAngle = -angle;
    float s = (float)sin(negAngle);
    float c = (float)cos(negAngle);
    if ((float)fabs(c * dx - s * dy) <= w * 0.5) {
        if ((float)fabs(s * dx + c * dy) <= h * 0.5) {
            return 1;
        }
    }
    return 0;
}

int __stdcall SaHitTestQuad(float x1, float y1, float x2, float y2, float x3, float y3, float px, float py) {
    float ax = x1 - x2;
    float ay = y1 - y2;
    float bx = x3 - x2;
    float by = y3 - y2;
    float dx = px - x2;
    float dy = py - y2;
    float u = (dx * ax + dy * ay) / (ax * ax + ay * ay);
    float v = (dx * bx + dy * by) / (bx * bx + by * by);
    if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f) {
        return 1;
    }
    return 0;
}

int __stdcall SaHitTestOrientedRectMargin(float cx, float cy, float w, float h, float angle, float px, float py, float margin) {
    float dx = px - cx;
    float dy = py - cy;
    float negAngle = -angle;
    float s = (float)sin(negAngle);
    float c = (float)cos(negAngle);
    if ((float)fabs(c * dx - s * dy) <= w * 0.5 - margin) {
        if ((float)fabs(s * dx + c * dy) <= h * 0.5 - margin) {
            return 1;
        }
    }
    return 0;
}

SaErrorLog::SaErrorLog() {
    buf[0] = '\0';
    flag = 0;
    ptr = buf;
}

void SaInputPad::UpdateRepeatTimers() {
    unsigned int cur = curInput;
    repeatMask = 0;
    heldMask = 0;
    unsigned int bit = 1;
    for (int i = 0; i < 32; ++i) {
        if (cur & 1) {
            repeatTimers[i]++;
            heldTimers[i]++;
            if (repeatTimers[i] >= 8) {
                heldMask |= bit;
            }
            if (repeatTimers[i] >= 26) {
                repeatMask |= bit;
                repeatTimers[i] -= 8;
            }
        } else {
            repeatTimers[i] = 0;
            heldTimers[i] = 0;
        }
        cur >>= 1;
        bit <<= 1;
    }
    unsigned int changed = curInput ^ prevInput;
    pressedMask = changed & curInput;
    releasedMask = changed & ~curInput;
}

SaInputPad::SaInputPad() {
    keyConfig[0] = 0;
    keyConfig[1] = 1;
    keyConfig[2] = 3;
    keyConfig[3] = 2;
    keyConfig[4] = 4;
    keyConfig[5] = -1;
    keyConfig[6] = -1;
    keyConfig[7] = -1;
    keyConfig[8] = -1;
}

} // namespace th128
