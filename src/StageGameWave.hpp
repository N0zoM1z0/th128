#pragma once

#include <windows.h>

namespace th128 {

struct SgwChainElem {
    unsigned int pad0;
    unsigned int flags4;
};

struct SgwChainMgr {
    int dummy;
    void Cut(SgwChainElem *elem);
};

struct SgwAnmVm;

struct SgwAnmNode {
    SgwAnmVm *vm;
    SgwAnmNode *next;
    SgwAnmNode *prev;
};

struct SgwAnmVm {
    unsigned int id0;
    SgwAnmNode mgrNode4;
    SgwAnmNode childNode10;       // +0x10 (+0x10 vm, +0x14 next, +0x18 prev)
    unsigned char pad1c[0x424 - 0x1c];
    int active424;                // +0x424
    unsigned char pad428[4];
    void *res42c;                 // +0x42c
    unsigned char pad430[0x4ac - 0x430];
    void *vbuf4ac;                // +0x4ac
    unsigned int flags4b0;        // +0x4b0
    unsigned char pad4b4[0x4d0 - 0x4b4];
    unsigned short word4d0;       // +0x4d0
    unsigned char pad4d2[0x4e8 - 0x4d2];

    SgwAnmVm();
    void ResetForReuse();
    void Update();
}; // 0x4E8

struct SgwAnmResource {
    int dummy;
    void BindVmScript(SgwAnmVm *vm, int scriptId);
};

struct SgwAnmMgr {
    unsigned char pad0[0x4e90e0];
    void *slots4e90e0[8];
    unsigned char pad4e9100[0x8b9704 - 0x4e9100];
    SgwAnmNode *world8b9704;
    SgwAnmNode *worldTail8b9708;
    SgwAnmNode *ui8b970c;

    SgwAnmVm *GetVmById(unsigned int id);
    void StopVmById(unsigned int id, short intr);
    void MarkResourceVmsForDelete(void *res);
};

struct SgwMeshQuad {
    short type0;
    short stride2;
    short script4;
    short vmIdx6;
};

struct SgwMeshEntry {
    unsigned char pad0[3];
    unsigned char flags3;
    unsigned char pad4[0x18];
    SgwMeshQuad firstQuad1c;
};

struct SgwMeshHeader {
    short count0;
};

struct SgwStage {
    unsigned char pad0[8];
    SgwChainElem *chain8;
    SgwChainElem *chainC;
    SgwMeshHeader *meshHdr10;
    SgwMeshEntry **meshes14;
    int pad18;
    int scriptStart1c;
    unsigned char pad20[0x4c - 0x20];
    int scriptCur4c;
    unsigned char pad50[0x1c4 - 0x50];
    SgwAnmResource *bgAnm1c4;
    SgwAnmVm *bgVms1c8;
    unsigned char pad1cc[0x2910 - 0x1cc];
    int field2910;
    unsigned char pad2914[0x3834 - 0x2914];
    unsigned int vm3834;
    unsigned int vm3838;
    SgwChainElem *chain383c;

    int UpdateBgMeshes();
    void ClearBgVms();
    static void EnableAndResetMeshes();
};

struct SgwTimer {
    int prev;
    int current;
    float currentF;
    unsigned int flags;
    int init;

    SgwTimer() { flags &= ~1u; }
    void Increment();
};

struct SgwEnemyFlagMgr {
    unsigned int flags0;
    unsigned char pad4[0x14];
    SgwTimer timer18;
    SgwTimer timer2c;
    unsigned char pad40[0xc];
    SgwAnmVm vm4c;
    unsigned char pad534[0x558 - 0x534];

    SgwEnemyFlagMgr();
    int __stdcall Register();
    void Cleanup();
    static SgwEnemyFlagMgr *Create();
};

struct SgwBullet {
    unsigned char pad0[0x11b8];
    ~SgwBullet();
};

struct SgwBulletMgr {
    unsigned char pad0[8];
    SgwChainElem *chain8;
    SgwChainElem *chainC;
    unsigned char pad10[0x64 - 0x10];
    SgwBullet bullets64[0x7d1];
    void *bulletAnm8a7f9c;

    ~SgwBulletMgr();
};

struct SgwMotionState {
    unsigned char pad0[0x38];
    void UpdateVelocity();
    void UpdatePos();
};

struct SgwEffect {
    int active0;
    SgwMotionState motion4;
    SgwTimer timer3c;
    unsigned int vm50;

    int CheckExpire();
    int Update();
};

struct SgwScoreState {
    unsigned char pad0[0x6b200];
    int val6b200;
};

struct SgwEnemy {
    virtual void V00() = 0;
    virtual void V01() = 0;
    virtual void V02() = 0;
    virtual void V03() = 0;
    virtual void V04() = 0;
    virtual void *__thiscall ScalarDtor(int flags) = 0; // +0x14
};

struct SgwEnemyNode {
    SgwEnemy *enemy;
    SgwEnemyNode *next;
};

struct SgwEnemyMgr {
    unsigned char pad0[8];
    SgwChainElem *chain8;
    SgwChainElem *chainC;
    unsigned char pad10[0x5c - 0x10];
    void *boss5c;
    unsigned char pad60[0xb4 - 0x60];
    void *eclB4;
    SgwEnemyNode *listB8;
    unsigned char padBc[8];
    int countC4;

    void __stdcall ResetList();
};

struct SgwMenuCursor {
    int cur;
    int prev;
    int count;
    unsigned char padC[0xc8 - 0x0c];
    int flagC8;
    int flagCC;
    int flagD0;
    int padD4;

    SgwMenuCursor() {
        cur = 0;
        prev = 0;
        count = 999;
        flagC8 = 0;
        flagCC = 1;
        flagD0 = 0;
    }
    int Move(int delta);
};

struct SgwThreadBase {
    virtual void Dummy() {}
    int a4, a8, aC, a10;
    SgwThreadBase() : a4(0), a8(0), aC(0), a10(0) {}
};

struct SgwGameMgr {
    unsigned int flags0;
    unsigned char pad4[0xc];
    SgwThreadBase thread10;
    unsigned char pad24[0xc];
    int ready30;
    const char **eclNames34;
    int pad38;
    SgwMenuCursor cursor3c;       // +0x3c..+0x110
    SgwMenuCursor cursor114;      // +0x114..+0x1e8
    SgwMenuCursor cursor1ec;      // +0x1ec..+0x2c0
    unsigned char pad2c4[4];
    SgwAnmVm vm2c8;
    unsigned char pad7b0[0x7c4 - 0x7b0];

    SgwGameMgr();
    static int InitSubsystems();
};

struct SgwItemMgr {
    unsigned char pad0[8];
    SgwChainElem *chain8;
    SgwChainElem *chainC;
    void *itemAnm10;

    ~SgwItemMgr();
};

struct SgwVertex28 {
    unsigned int d[7];
};

struct SgwMeshStrip {
    int rows0;
    int cols4;
    unsigned int *vmIds8;
    SgwAnmVm **vmsC;
    SgwVertex28 *verts10;

    void CopyVertices();
};

struct SgwEndingInner {
    int pad0;
    SgwTimer timer4;
    unsigned char pad18[0x74 - 0x18];
    unsigned int flags74;

    int __stdcall CheckDone();
};

struct SgwEnding {
    unsigned char pad0[0x18];
    SgwEndingInner *inner18;
    int pad1c;
    unsigned int flags20;
    int frames24;

    int Step();
};

extern unsigned int g_SgwSupFlags;
extern CRITICAL_SECTION g_SgwSupCs0;
extern unsigned char g_SgwSupCsDepth0;
extern SgwChainMgr *g_SgwChainMgr;
extern SgwAnmMgr *g_SgwAnmMgr;
extern SgwStage *g_SgwStage;
extern SgwEnemyFlagMgr *g_SgwEnemyFlagMgr;
extern SgwBulletMgr *g_SgwBulletMgr;
extern SgwScoreState *g_SgwScoreState;
extern SgwGameMgr *g_SgwGameMgr;
extern SgwEnemyMgr *g_SgwEnemyMgr;
extern SgwItemMgr *g_SgwItemMgr;
extern unsigned int g_SgwInputHeld;
extern int g_SgwSceneNext;

void __fastcall SgwChainCutImpl(SgwChainMgr *mgr, SgwChainElem *elem);
void SgwPlaySfx(int id);
void SgwCreatePlayer();
void SgwCreateBulletMgr();
void SgwCreateLaserMgr();
void SgwCreateSub443b40();
void __stdcall SgwCreateEnemyMgr(const char *eclName);
void SgwCreateGui();

} // namespace th128
