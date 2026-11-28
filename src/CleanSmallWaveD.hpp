#pragma once

#include <windows.h>

namespace th128 {

struct CsdScoreGlobal {
    void *buf00;
    void *buf04;
};

struct CsdEclStack {
    unsigned char buf[0x1000];
    int sp;

    int Push(const void *valPtr, char tag);
};

struct CsdLaserBase {
    virtual ~CsdLaserBase();
    unsigned char pad04[0x5bc];
    CsdLaserBase();
};

struct CsdAnmVm {
    unsigned char data[0x4e8];
    CsdAnmVm();
};

struct CsdLaserLine : CsdLaserBase {
    unsigned char pad5c0[0x354];
    CsdAnmVm vm1;
    CsdAnmVm vm2;
    CsdAnmVm vm3;

    CsdLaserLine();
    virtual ~CsdLaserLine();
};

struct CsdFontHolder {
    HFONT hFont;
    int width;
    int height;
    union {
        char buf[16];
        char *ptr;
    } name;
    unsigned int len;
    unsigned int cap;
    int weight;

    void RecreateFont();
};

struct CsdThreadObj {
    unsigned char pad00[0x04];
    HANDLE handle;
    unsigned int threadId;
    int f0c;
    int f10;
    unsigned char pad14[0x04];
    void *proc;
    unsigned char pad1c[0x04];
    unsigned int flags20;
    unsigned char pad24[0x28];
    int f4c;

    void Stop();
    void StartWorker();
};

struct CsdCardEffectMgr {
    unsigned char data[0x6b21c];
    CsdCardEffectMgr();
    void Shutdown();
};

struct CsdScoreMgr {
    unsigned char data[0x2bc];
    CsdScoreMgr();
    ~CsdScoreMgr();
};

struct CsdDirectInput {
    virtual HRESULT __stdcall S0() = 0;
    virtual HRESULT __stdcall S1() = 0;
    virtual HRESULT __stdcall S2() = 0;
    virtual HRESULT __stdcall CreateDevice(const GUID *guid, void **dev, void *outer) = 0;
};

struct CsdDeviceInstance {
    DWORD dwSize;
    GUID guidInstance;
};

struct CsdBullet {
    unsigned char pad00[0x4b4];
    void *buf4b4;
    unsigned char pad4b8[0x4e4];
    void *buf99c;

    ~CsdBullet();
};

struct CsdReplayStage {
    unsigned char pad00[0x1518];
    CsdReplayStage *self1518;
    unsigned char data151c[0x384];
    void *ptr18a0;
    CsdReplayStage *self18a4;
    int f18a8;
    int f18ac;

    CsdReplayStage();
};

extern CsdScoreGlobal *g_CsdScoreGlobal;
extern CsdDirectInput *g_CsdDirectInput;
extern void *g_CsdPads[1];
extern unsigned char *g_CsdTextBits;
extern int g_CsdTextByteLen;
extern int g_CsdSavedScreenSave;
extern int g_CsdSavedLowPower;
extern int g_CsdSavedPowerOff;

unsigned __stdcall CsdWorkerProc(void *arg);
int __stdcall CsdInitCardEffectMgr(CsdCardEffectMgr *mgr);
int __stdcall CsdRegScoreMgr(CsdScoreMgr *mgr);

void CsdDestroyScoreGlobal();
CsdCardEffectMgr *CsdCreateCardEffectMgr();
CsdScoreMgr *CsdCreateScoreMgr();
int __stdcall CsdEnumJoysticksCb(const CsdDeviceInstance *inst, void *ref);
void CsdFillTextImageRgb444(unsigned int rgb);
void CsdRestoreSystemSettings();

} // namespace th128
