#pragma once

#include <windows.h>
#include <mmsystem.h>

namespace th128 {

struct ISmwBuffer {
    virtual HRESULT __stdcall QueryInterface(const GUID &, void **) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                 // +0x04
    virtual ULONG __stdcall Release() = 0;                                // +0x08
    virtual HRESULT __stdcall V03() = 0;
    virtual HRESULT __stdcall V04() = 0;
    virtual HRESULT __stdcall V05() = 0;
    virtual HRESULT __stdcall V06() = 0;
    virtual HRESULT __stdcall V07() = 0;
    virtual HRESULT __stdcall V08() = 0;
    virtual HRESULT __stdcall V09() = 0;
    virtual HRESULT __stdcall V10() = 0;
    virtual HRESULT __stdcall Lock(DWORD off, DWORD bytes, void **p1, LPDWORD b1, void **p2, LPDWORD b2, DWORD flags) = 0; // +0x2C
    virtual HRESULT __stdcall Play(DWORD r1, DWORD prio, DWORD flags) = 0; // +0x30
    virtual HRESULT __stdcall V13() = 0;
    virtual HRESULT __stdcall V14() = 0;
    virtual HRESULT __stdcall V15() = 0;
    virtual HRESULT __stdcall V16() = 0;
    virtual HRESULT __stdcall V17() = 0;
    virtual HRESULT __stdcall Stop() = 0; // +0x48
    virtual HRESULT __stdcall Unlock(void *p1, DWORD b1, void *p2, DWORD b2) = 0; // +0x4C
};

struct SmwBufferDesc {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    WAVEFORMATEX *lpwfxFormat;
    GUID guid3DAlgorithm;
};

struct ISmwDirectSound8 {
    virtual HRESULT __stdcall QueryInterface(const GUID &, void **) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                 // +0x04
    virtual ULONG __stdcall Release() = 0;                                // +0x08
    virtual HRESULT __stdcall CreateSoundBuffer(const SmwBufferDesc *desc, ISmwBuffer **out, void *outer) = 0; // +0x0C
    virtual HRESULT __stdcall V04() = 0;
    virtual HRESULT __stdcall V05() = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND hwnd, DWORD level) = 0; // +0x18
};

struct SmwTrackInfo {
    char name[16];          // +0x00
    DWORD dataOffset;       // +0x10
    DWORD preloadSize;      // +0x14
    DWORD loopStart;        // +0x18
    DWORD dataSize;         // +0x1C
    WAVEFORMATEX format;    // +0x20 (+0x24 = nSamplesPerSec, +0x2C = nBlockAlign)
    unsigned short pad32;
}; // total 0x34 bytes

struct SmwStreamingSound {
    virtual ~SmwStreamingSound() {}
};

struct SmwCSoundManager {
    ISmwDirectSound8 *ds;

    SmwCSoundManager() : ds(NULL) {}
    ~SmwCSoundManager() {
        if (ds != NULL) {
            ds->Release();
            ds = NULL;
        }
    }
    HRESULT SetPrimaryBufferFormat();
    HRESULT CreateStreamingFromFile(
        SmwStreamingSound **out, const char *path, DWORD flags, GUID alg,
        DWORD notifyCount, DWORD notifySize, HANDLE ev, SmwTrackInfo *info);
    HRESULT CreateStreamingFromMemory(
        SmwStreamingSound **out, unsigned char *data, DWORD dataSize, SmwTrackInfo *info,
        DWORD flags, GUID alg, DWORD notifyCount, DWORD notifySize, HANDLE ev);
};

struct SmwSfxTableEntry {
    int id;
    int wavIdx;
    int f8;
    int fc;
    int f10;
}; // 0x14 bytes

struct SmwSfxSlot {
    ISmwBuffer *buf0;       // +0x00
    int field4;             // +0x04
    SmwSfxTableEntry *tbl8; // +0x08
    int indexC;             // +0x0C
    int f10;
    int f14;
}; // 0x18 bytes

struct SmwSoundMgr {
    ISmwDirectSound8 *ds0;              // +0x00 (0x4D5A68)
    int pad4;                           // +0x04
    ISmwBuffer *silentBuf8;             // +0x08 (0x4D5A70)
    HWND hwndC;                         // +0x0C (0x4D5A74)
    SmwCSoundManager *csoundMgr10;      // +0x10 (0x4D5A78)
    DWORD bgmThreadId14;                // +0x14 (0x4D5A7C)
    HANDLE bgmThread18;                 // +0x18 (0x4D5A80)
    int pad1c;
    int playQueue20[12];                // +0x20..+0x4F
    unsigned char pad50[0x1880 - 0x50];
    SmwTrackInfo *preloadTrack1880[16]; // +0x1880 (0x4D72E8)
    unsigned char *preloadBuf18c0[16];  // +0x18C0 (0x4D7328)
    unsigned char *preloadData1900[16]; // +0x1900 (0x4D7368)
    DWORD preloadSize1940[16];          // +0x1940 (0x4D73A8)
    int curPreloadSlot1980;             // +0x1980 (0x4D73E8)
    SmwTrackInfo *trackTable1984;       // +0x1984 (0x4D73EC)
    SmwSfxSlot sfxSlots1988[0x4c];      // +0x1988 (0x4D73F0..0x4D7B0F)
    void *wavBufs20a8[65];              // +0x20A8 (0x4D7B10..0x4D7C13)
    unsigned char pad21ac[0x432c - 0x21ac];
    char preloadNames432c[16][256];     // +0x432C (0x4D9D94)
    char bgmFileName532c[256];          // +0x532C (0x4DAD94)
    SmwStreamingSound *stream542c;      // +0x542C (0x4DAE94)
    HANDLE notifyEvent5430;             // +0x5430 (0x4DAE98)
    unsigned char pad5434[0x5454 - 0x5434];
    int bgmVol5454;                     // +0x5454
    int sfxVol5458;                     // +0x5458

    int Initialize(HWND hwnd);
    int Shutdown();
    int FindTrackIndex(const char *path);
    int StartStreamingBgm();
    int PreloadBgm(const char *name, int slot);
    int LoadPreloadedBgm(int slot);
    void StopAndDestroyBgm();
    int LoadWav(int slotIdx, const char *wavName);
    int OpenBgmByName(const char *name);
};

struct SmwErrorContext {
    int dummy;
    void __cdecl Log(const char *fmt, ...);
};

struct SmwSupervisor {
    unsigned char pad0[0x1e3];
    unsigned char soundEnabled1e3; // 0x4D304B (0x4D2E68 + 0x1E3)
    unsigned char pad1e4[0x200 - 0x1e4];
    unsigned int flags200;         // 0x4D3068
    unsigned char pad204[0x590 - 0x204];
    unsigned int gameFlags590;     // 0x4D33F8
    unsigned char pad594[0x810 - 0x594];
    CRITICAL_SECTION critSecs810[4]; // +0x810 ([2] = 0x4D36A8)
    unsigned char pad870[0x930 - 0x870];
    unsigned char lockDepth930[4]; // +0x930 ([2] = 0x4D379A)

    void UnlockCritSec(int idx);
};

extern SmwSoundMgr g_SmwSoundMgr;
extern SmwSfxTableEntry g_SmwSfxTable[0x4c];
extern const char *g_SmwWavNames[65];
extern SmwErrorContext g_SmwErrorCtx;
extern SmwSupervisor g_SmwSupervisor;
extern int g_SmwAudioMode;
extern void *g_SmwThreadParam;
extern const GUID g_SmwGuidNull;

extern HRESULT (__stdcall *g_SmwDirectSoundCreate8)(const GUID *, ISmwDirectSound8 **, void *);
DWORD __stdcall SmwBgmThreadProc(LPVOID param);
void __cdecl SmwSoundDebugLog(const char *fmt, ...);

} // namespace th128
