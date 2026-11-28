#pragma once

#include <windows.h>

namespace th128 {

struct MpwChainElem;

struct MpwChainNode {
    MpwChainElem *owner;
    MpwChainNode *next;
    MpwChainNode *prev;
};

struct MpwChainElem {
    int priority0;
    unsigned int flags4;
    void *cb8;
    int (__fastcall *onRegC)(void *);
    int (__fastcall *onCut10)(void *);
    MpwChainNode node14;
    void *arg20;

    MpwChainElem() {
        flags4 &= ~1u;
        cb8 = NULL;
        onRegC = NULL;
        onCut10 = NULL;
        priority0 = 0;
        node14.owner = this;
        node14.next = NULL;
        node14.prev = NULL;
    }
};

struct MpwChain {
    MpwChainElem calcChain;
    MpwChainElem drawChain;
    MpwChainNode *current;
    int isShutdown;

    int RegisterCalc(MpwChainElem *elem, int priority);
    int RegisterDraw(MpwChainElem *elem, int priority);
};

extern MpwChain *g_MpwChain;

struct MpwThreadInf {
    void *vtable0;
    HANDLE handle4;
    unsigned int threadId8;
    int fieldC;
    int running10;
    int field14;
    unsigned int (__stdcall *proc18)(void *);

    void Stop();
};

struct MpwMother {
    unsigned char pad0[8];
    MpwChainElem *calc8;
    MpwChainElem *drawC;
    MpwThreadInf thread10;

    int __stdcall Init();
};

struct IMpwCom {
    virtual HRESULT __stdcall QueryInterface(const GUID &, void **) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                 // +0x04
    virtual ULONG __stdcall Release() = 0;                                // +0x08
    virtual HRESULT __stdcall V03() = 0;
    virtual HRESULT __stdcall V04() = 0;
    virtual HRESULT __stdcall V05() = 0;
    virtual HRESULT __stdcall V06() = 0;
    virtual HRESULT __stdcall V07() = 0;
    virtual HRESULT __stdcall Unacquire() = 0;                            // +0x20
};

struct IMpwDSBuf {
    virtual HRESULT __stdcall QueryInterface(const GUID &, void **) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall V03() = 0;
    virtual HRESULT __stdcall V04() = 0;
    virtual HRESULT __stdcall V05() = 0;
    virtual HRESULT __stdcall V06() = 0;
    virtual HRESULT __stdcall V07() = 0;
    virtual HRESULT __stdcall V08() = 0;
    virtual HRESULT __stdcall V09() = 0;
    virtual HRESULT __stdcall V10() = 0;
    virtual HRESULT __stdcall V11() = 0;
    virtual HRESULT __stdcall V12() = 0;
    virtual HRESULT __stdcall V13() = 0;
    virtual HRESULT __stdcall V14() = 0;
    virtual HRESULT __stdcall V15() = 0;
    virtual HRESULT __stdcall V16() = 0;
    virtual HRESULT __stdcall V17() = 0;
    virtual HRESULT __stdcall Stop() = 0; // +0x48
};

struct MpwStreamSound {
    void *vtable0;
    IMpwDSBuf **buffers4;
    unsigned char pad8[0xc];
    int fadeRem14;
    int fadeTotal18;
    int fadeMode1c;

    HRESULT SetVolume(int vol);
};

struct MpwSmallObj {
    int dummy;
    ~MpwSmallObj();
};

struct MpwBufObj {
    unsigned char pad0[0x4ac];
    void *buf4ac;

    ~MpwBufObj() {
        if (buf4ac != NULL) {
            free(buf4ac);
            buf4ac = NULL;
        }
    }
};

struct MpwConfig {
    unsigned int version0;     // +0x00 (0x128001)
    unsigned int keys4[4];     // +0x04..+0x13
    unsigned short key14;      // +0x14
    unsigned char pad16[4];
    unsigned char f1a;         // +0x1A (< 2)
    unsigned char f1b;         // +0x1B (< 3)
    unsigned char f1c;         // +0x1C (< 2)
    unsigned char f1d;         // +0x1D (< 4)
    unsigned char f1e;         // +0x1E (< 3)
    unsigned char f1f;         // +0x1F (< 3)
    unsigned char pad20[0x3c - 0x20];

    void ResetDefaults();
}; // 0x3C bytes

struct MpwVec3 {
    int x, y, z;
};

struct MpwAnmVm {
    unsigned char pad0[0x464];
    MpwVec3 pos464;
};

struct MpwAnmId {
    unsigned int id;
};

struct MpwAnmResource {
    int dummy;
    MpwAnmId CreateVm(const void *pos, int script, int layer, unsigned int flags);
};

struct MpwAnmManager {
    unsigned char pad0[0x4e9698];
    IMpwCom *res4e9698;

    MpwAnmVm *GetVmById(MpwAnmId id);
};

struct MpwSupervisor {
    unsigned char pad0[0xc];
    IMpwCom *dinputC;          // +0x0C
    unsigned char pad10[0x10];
    IMpwCom *kbDev20;          // +0x20
    IMpwCom *joyDev24;         // +0x24
    unsigned char pad28[0x114 - 0x28];
    int flag114;               // +0x114
    unsigned char pad118[0x200 - 0x118];
    unsigned int flags200;     // +0x200
    unsigned char pad204[0x57c - 0x204];
    int flag57c;               // +0x57C
    unsigned char pad580[0x58c - 0x580];
    MpwAnmResource *loadAnm58c;// +0x58C
    unsigned char pad590[0x93c - 0x590];
    int loadingActive93c;      // +0x93C

    int Shutdown();
    int LoadConfig();
    void SpawnLoadingVmsA(const MpwVec3 *pos);
    void SpawnLoadingVmsB(const MpwVec3 *pos);
};

struct MpwMenuCursor {
    int cur;
    int prev;
    int count;
    unsigned char padC[0xd0 - 0x0c];
    int flagD0;

    void Move(int delta);
};

struct MpwPauseMenu {
    unsigned char pad0[0x38];
    MpwMenuCursor scoreCursor38; // +0x38 (count at +0x40, flagD0 at +0x108)
    unsigned char pad10c[4];
    MpwMenuCursor charCursor110; // +0x110 (count at +0x118, flagD0 at +0x1E0)
    unsigned char pad1e4[0x1f0 - 0x1e4];
    int nameLen1f0;              // +0x1F0
    int isPractice1f4;           // +0x1F4
    int noHighScore1f8;          // +0x1F8
    unsigned char pad1fc[0x2cc - 0x1fc];
    char nameBuf2cc[16];         // +0x2CC

    void InitScoreNameEntry();
};

struct MpwErrorCtx {
    char buffer[0x2000];
    char *cursor;
    int fatal;
    const char *__cdecl Log(const char *fmt, ...);
    const char *__cdecl Fatal(const char *fmt, ...);
};

struct MpwArchive {
    int dummy;
    void Close();
};

struct MpwScoreFile {
    unsigned char pad0[8];
    unsigned char routes8[8][0x66c];
    unsigned char pad3368[0xc];
    char lastName3374[16];
};

extern int g_MpwAudioMode;
extern MpwThreadInf g_MpwSupThread;
extern void *g_MpwCfgAlloc;
extern MpwSmallObj *g_MpwGlobalObj;
extern MpwAnmManager *g_MpwAnmMgr;
extern void *g_MpwSoundMgrPtr;
extern HGDIOBJ g_MpwGdiObjs[8];
extern MpwArchive g_MpwArchive;
extern MpwBufObj *g_MpwBufObjs[3];
extern int g_MpwSceneCur;
extern int g_MpwSceneNext;
extern int g_MpwSceneFlag;
extern MpwSupervisor g_MpwSupervisor;
extern MpwConfig g_MpwConfig;
extern char g_MpwAppDataDir[0x1000];
extern unsigned int g_MpwInputKeys[4];
extern unsigned short g_MpwInputKey10;
extern int g_MpwSupFlag3e4;
extern MpwErrorCtx g_MpwErrorCtx;
extern MpwAnmId g_MpwLoadVmIds[3];
extern MpwStreamSound *g_MpwStreamSound;
extern int g_MpwStageNum;
extern int g_MpwStageNumCopy;
extern int g_MpwRouteIdx;
extern void *g_MpwStageTablePtr;
extern unsigned char g_MpwStageTableA[0x118];
extern unsigned char g_MpwStageTableB[0x118];
extern MpwScoreFile *g_MpwScoreFile;

int MpwChainRegisterCalc(MpwChainElem *elem, int prio);
int MpwChainRegisterDraw(MpwChainElem *elem, int prio);
int MpwMotherCalc(void *);
int MpwMotherDraw(void *);
unsigned int __stdcall MpwMotherThread(void *);
int MpwWaitSoundReady();
void MpwCleanupSub();
void __stdcall MpwEnqueueBgmCommand(void *mgr, int cmd, int arg, const char *label);
void MpwDestroyTextImage();
int MpwSupCalc(void *);
int MpwSupOnReg(void *);
int MpwSupDraw0(void *);
int MpwSupDraw1(void *);
int MpwSupDraw2(void *);
int MpwSupDraw3(void *);
int MpwSupDraw4(void *);
int MpwSupDraw5(void *);
int MpwSupDraw6(void *);
int MpwSupDraw7(void *);
void *__stdcall MpwLoadFile(const char *path, DWORD *outSize, int fromDisk);
int __stdcall MpwWriteDataToFile(const char *path, const void *data, unsigned int size);
int __stdcall MpwInsertScoreEntry(void *route);
int MpwRegisterSupervisorChains();
void MpwUpdateBgmFade();

} // namespace th128
