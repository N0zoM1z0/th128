#pragma once

#include <stddef.h>

namespace th128 {

struct SaSoundMgr {
    unsigned char pad00[0x1984];
    void *bgmTable;
    unsigned char pad1988[0x5460 - 0x1988];
};

struct SaBgmStream {
    unsigned char pad00[0x30];
    int isPlaying;
    unsigned char pad34[0x44];
    int inNotify;
};

struct SaD3DSurfaceDesc {
    unsigned int pad00[7];
    unsigned int height;
};

struct SaD3DLockedRect {
    int pitch;
    void *pBits;
};

struct SaD3DSurfaceVtbl {
    void *pad00[2];
    unsigned long (__stdcall *Release)(void *self);
    void *pad0c[9];
    long (__stdcall *GetDesc)(void *self, SaD3DSurfaceDesc *desc);
    long (__stdcall *LockRect)(void *self, SaD3DLockedRect *rect, const void *r, unsigned long flags);
    long (__stdcall *UnlockRect)(void *self);
};

struct SaD3DSurface {
    SaD3DSurfaceVtbl *vtbl;
};

struct SaD3DTextureVtbl {
    void *pad00[18];
    long (__stdcall *GetSurfaceLevel)(void *self, unsigned int level, SaD3DSurface **ppSurface);
};

struct SaD3DTexture {
    SaD3DTextureVtbl *vtbl;
};

struct SaStreamObj {
    virtual ~SaStreamObj();
    void Cleanup();
};

struct SaInputPad {
    unsigned int curInput;
    unsigned int prevInput;
    unsigned int repeatMask;
    unsigned int pressedMask;
    unsigned int releasedMask;
    unsigned int repeatTimers[32];
    unsigned int pad94[32];
    unsigned int heldTimers[32];
    unsigned int pad194[38];
    unsigned int heldMask;
    unsigned int pad230;
    short keyConfig[9];

    SaInputPad();
    void UpdateRepeatTimers();
};

struct SaErrorLog {
    char buf[0x2000];
    char *ptr;
    char flag;
    SaErrorLog();
};

extern SaSoundMgr g_SaSoundMgr;
extern void *g_SaSoundThread;
extern void *g_SaSfxThread;
extern unsigned long g_SaSoundThreadId;
extern int g_SaSoundState;
extern void *g_SaSoundArg;
extern const char g_SaBgmTableName[];
extern void *g_SaBgmEvent;
extern SaBgmStream *g_SaBgmStream;
extern SaErrorLog g_SaErrorLog;

unsigned long __stdcall SaSoundThreadProc(void *param);
void *__stdcall SaLoadFile(const char *path, int a, int b);
void __stdcall SaHandleStreamNotify(SaBgmStream *stream);
void SaSfxLoadThread();

void SaNoOp4575C0();
int __stdcall SaInitSoundThread(void *arg);
int SaRequestSoundStop();
int SaShutdownSoundThreads();
void SaStartSfxThread();
int __stdcall SaLoadBgmTable(SaSoundMgr *mgr);
unsigned long __stdcall SaBgmThreadProc(void *param);
float __stdcall SaSinF(float x);
void __stdcall SaClearD3DTexture(SaD3DTexture **ppTex);
int __stdcall SaHitTestOrientedRect(float cx, float cy, float w, float h, float angle, float px, float py);
int __stdcall SaHitTestQuad(float x1, float y1, float x2, float y2, float x3, float y3, float px, float py);
int __stdcall SaHitTestOrientedRectMargin(float cx, float cy, float w, float h, float angle, float px, float py, float margin);

} // namespace th128
