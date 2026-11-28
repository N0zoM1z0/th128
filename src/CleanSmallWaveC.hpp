#pragma once

#include <windows.h>

namespace th128 {

struct CscGuiSubObj {
    unsigned char pad00[0x40];
    int vmIds[10];

    ~CscGuiSubObj();
};

struct CscLineArray {
    unsigned char pad00[0x34];
    void **lines;
    int count;

    void FreeLines();
};

struct CscChainElem {
    int f00;
    unsigned int flags;
    void *cb;
    void *f0c;
    void *f10;
    CscChainElem *self14;
    void *f18;
    void *f1c;
    void *arg20;

    CscChainElem() {
        flags &= ~1u;
        cb = NULL;
        f0c = NULL;
        f10 = NULL;
        f00 = 0;
        self14 = this;
        f18 = NULL;
        f1c = NULL;
    }
};

struct CscFogMgr {
    unsigned char pad00[0x0c];
    CscChainElem *drawElem;

    int RegisterDrawChain();
};

struct CscDsBuffer {
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
    virtual HRESULT __stdcall Play(DWORD reserved, DWORD priority, DWORD flags) = 0; // 12 = 0x30
};

struct CscWaveFile {
    unsigned char pad00[0x90];
    void *desc;
    unsigned char pad94[0x04];
    HANDLE handle;

    void __stdcall ReopenTrack(void *desc, HANDLE h);
};

struct CscStreamingSound {
    unsigned char pad00[0x04];
    CscDsBuffer **buffers;
    unsigned char pad08[0x04];
    CscWaveFile *waveFile;
    unsigned char pad10[0x10];
    DWORD playPriority;
    DWORD playFlags;
    unsigned char pad28[0x08];
    int playing;
    int paused;

    HRESULT Resume();
};

struct CscEndingInner {
    unsigned char pad00[0x70];
    const char *anmPath;
    unsigned int flags74;
    unsigned char pad78[0x08];
    void *anms[8];
    unsigned char padA0[0x4c];
    int slotIdx;
};

struct CscEndingMgr {
    unsigned char pad00[0x18];
    CscEndingInner *inner;
};

struct CscAsciiMgr {
    unsigned char pad00[0x1c89c];
    int loadingVmId;
};

struct CscScoreData {
    unsigned char pad00[0x338e];
    unsigned char bgmUnlocked[32];
};

extern CscEndingMgr *g_CscEndingMgr;
extern CscAsciiMgr *g_CscAsciiMgr;
extern unsigned char g_CscSoundFlag;
extern int g_CscSoundMgr;
extern CscScoreData *g_CscScoreData;

void CscDeleteVmByIdPtr(int *idPtr);
void CscRegisterDraw(CscChainElem *elem, int prio);
void CscFogDrawCb();
void *CscPreloadAnm(int slot, const char *path);
void __stdcall CscStopVmById(int id, int flag);
void __stdcall CscEnqueueBgmCommand(void *mgr, int cmd, int arg, const char *path);

struct CscScoreBuf {
    unsigned char data[16];
    unsigned int CalcChecksum(int totalLen);
};
int CscLoadEndingAnmWorkerA();
int CscLoadEndingAnmWorkerB();
int __stdcall CscPlayPreloadedBgm(int slot, int bgmIdx);

} // namespace th128
