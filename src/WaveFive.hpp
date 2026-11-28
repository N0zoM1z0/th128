#pragma once

#include <windows.h>

namespace th128 {

struct WfLaserEntry {
    unsigned char pad00[0x10];
    int value10;
    unsigned char pad14[0x0c];
    int type20;
    int flag24;
    unsigned char pad28[0x04];
};

struct WfLaserInfinite {
    unsigned char pad00[0x0c];
    int state0c;
    unsigned char pad10[0x584];
    int index594;
    int blocked598;
    unsigned char pad59c[0x18];
    int param5b4;
    unsigned char pad5b8[0x64];
    WfLaserEntry entries[0x12];
    unsigned char pad934[0x4b4];
    unsigned int flagsDe8;

    void Slot01();
};

struct WfGui {
    unsigned char pad00[0xa8];
    int encodedSpellTime;
};

struct WfSoundStream {
    void *buf;
    virtual void *Destroy(int flag) = 0;
    void Stop(int flag);
};

struct WfBgmEntry {
    char name[0x10];
    int offset;
    int size;
    unsigned char pad18[0x1c];
};

struct WfSoundManager {
    unsigned char pad00[0x14];
    DWORD threadId;
    HANDLE threadHandle;
    unsigned char pad1c[0x1968];
    WfBgmEntry *bgmTable;
    unsigned char pad1988[0x3aa4];
    WfSoundStream *stream;
    HANDLE notifyEvent;

    void StopStreamingBgm();
    int FindBgmIndex(const char *path);
};

struct WfCritSupervisor {
    void UnlockCritSec(int idx);
};

extern unsigned int g_WfFlags;
extern unsigned char g_WfSoundFlag;
extern int g_WfSoundInited;
extern char g_WfBgmPath[256];
extern CRITICAL_SECTION g_WfSoundCs;
extern unsigned char g_WfSoundCsDepth;
extern WfCritSupervisor g_WfCritSup;
extern WfSoundManager g_WfSoundMgr;
extern WfBgmEntry *g_WfPreloadEntries[16];
extern void *g_WfPreloadBufs[16];
extern void *g_WfPreloadCurs[16];
extern int g_WfPreloadSizes[16];
extern char g_WfPreloadNames[16][256];

void WfSoundDebugLog(const char *fmt, ...);
int WfPreLoadBgm(const char *path, int slot);

struct WfD3dDevice9 {
    virtual HRESULT __stdcall S00() = 0;
    virtual HRESULT __stdcall S01() = 0;
    virtual HRESULT __stdcall S02() = 0;
    virtual HRESULT __stdcall S03() = 0;
    virtual HRESULT __stdcall S04() = 0;
    virtual HRESULT __stdcall S05() = 0;
    virtual HRESULT __stdcall S06() = 0;
    virtual HRESULT __stdcall S07() = 0;
    virtual HRESULT __stdcall S08() = 0;
    virtual HRESULT __stdcall S09() = 0;
    virtual HRESULT __stdcall S10() = 0;
    virtual HRESULT __stdcall S11() = 0;
    virtual HRESULT __stdcall S12() = 0;
    virtual HRESULT __stdcall S13() = 0;
    virtual HRESULT __stdcall S14() = 0;
    virtual HRESULT __stdcall S15() = 0;
    virtual HRESULT __stdcall S16() = 0;
    virtual HRESULT __stdcall S17() = 0;
    virtual HRESULT __stdcall S18() = 0;
    virtual HRESULT __stdcall S19() = 0;
    virtual HRESULT __stdcall S20() = 0;
    virtual HRESULT __stdcall S21() = 0;
    virtual HRESULT __stdcall S22() = 0;
    virtual HRESULT __stdcall S23() = 0;
    virtual HRESULT __stdcall S24() = 0;
    virtual HRESULT __stdcall S25() = 0;
    virtual HRESULT __stdcall S26() = 0;
    virtual HRESULT __stdcall S27() = 0;
    virtual HRESULT __stdcall S28() = 0;
    virtual HRESULT __stdcall S29() = 0;
    virtual HRESULT __stdcall S30() = 0;
    virtual HRESULT __stdcall S31() = 0;
    virtual HRESULT __stdcall S32() = 0;
    virtual HRESULT __stdcall S33() = 0;
    virtual HRESULT __stdcall S34() = 0;
    virtual HRESULT __stdcall S35() = 0;
    virtual HRESULT __stdcall S36() = 0;
    virtual HRESULT __stdcall S37() = 0;
    virtual HRESULT __stdcall S38() = 0;
    virtual HRESULT __stdcall S39() = 0;
    virtual HRESULT __stdcall S40() = 0;
    virtual HRESULT __stdcall S41() = 0;
    virtual HRESULT __stdcall S42() = 0;
    virtual HRESULT __stdcall S43() = 0;
    virtual HRESULT __stdcall S44() = 0;
    virtual HRESULT __stdcall S45() = 0;
    virtual HRESULT __stdcall S46() = 0;
    virtual HRESULT __stdcall S47() = 0;
    virtual HRESULT __stdcall S48() = 0;
    virtual HRESULT __stdcall S49() = 0;
    virtual HRESULT __stdcall S50() = 0;
    virtual HRESULT __stdcall S51() = 0;
    virtual HRESULT __stdcall S52() = 0;
    virtual HRESULT __stdcall S53() = 0;
    virtual HRESULT __stdcall S54() = 0;
    virtual HRESULT __stdcall S55() = 0;
    virtual HRESULT __stdcall S56() = 0;
    virtual HRESULT __stdcall S57() = 0;
    virtual HRESULT __stdcall S58() = 0;
    virtual HRESULT __stdcall S59() = 0;
    virtual HRESULT __stdcall S60() = 0;
    virtual HRESULT __stdcall S61() = 0;
    virtual HRESULT __stdcall S62() = 0;
    virtual HRESULT __stdcall S63() = 0;
    virtual HRESULT __stdcall S64() = 0;
    virtual HRESULT __stdcall SetTexture(DWORD stage, void *tex) = 0; // 65 = 0x104
};

struct WfAnmSpriteEntry {
    unsigned char pad00[0x08];
    void **texSlot;
};

struct WfAnmVm {
    unsigned char pad00[0x3f3];
    unsigned char alpha;
    unsigned char pad3f4[0x34];
    WfAnmSpriteEntry *sprite;
    unsigned char pad42c[0x84];
    unsigned int flags;
};

struct WfAnmManager {
    unsigned char pad00[0x4e9688];
    void **curTexSlot;
    unsigned char pad4e968c[0x02];
    unsigned char fvfState;

    void FlushSpriteBatch();
    void ApplyState(WfAnmVm *vm);
    void AppendSprite(const void *verts);
    int DrawSprite2D(WfAnmVm *vm, const void *verts);
};

extern int g_WfTextFmt;
extern int g_WfTextStride;
extern unsigned char *g_WfTextBits;
extern WfGui *g_WfGui;
extern WfD3dDevice9 *g_WfDevice;

bool __stdcall WfInvertTextImageAlpha(int height);
void __stdcall WfGuiGetSpellTime(int *outSec, int *outCs);

} // namespace th128
