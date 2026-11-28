#pragma once

#include <windows.h>

namespace th128 {

struct GcwChainElem;

struct GcwChainNode {
    GcwChainElem *owner;
    GcwChainNode *next;
    GcwChainNode *prev;
};

struct GcwChainElem {
    int priority0;
    unsigned int flags4;
    void *cb8;
    int (__fastcall *onRegC)(void *);
    int (__fastcall *onCut10)(void *);
    GcwChainNode node14;
    void *arg20;

    GcwChainElem() {
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

extern unsigned int g_GcwSupFlags;
extern CRITICAL_SECTION g_GcwSupCs0;
extern unsigned char g_GcwSupCsDepth0;

void __fastcall GcwChainCutImpl(struct GcwChain *chain, GcwChainElem *elem);

struct GcwChain {
    GcwChainElem calcChain;
    GcwChainElem drawChain;
    GcwChainNode *current;
    int isShutdown;

    int RegisterCalc(GcwChainElem *elem, int priority);
    int RegisterDraw(GcwChainElem *elem, int priority);
    void Cut(GcwChainElem *elem) {
        if (elem != NULL) {
            if ((g_GcwSupFlags & 0x8000u) != 0) {
                EnterCriticalSection(&g_GcwSupCs0);
                ++g_GcwSupCsDepth0;
            }
            GcwChainCutImpl(this, elem);
            if ((g_GcwSupFlags & 0x8000u) != 0) {
                LeaveCriticalSection(&g_GcwSupCs0);
                --g_GcwSupCsDepth0;
            }
        }
    }
};

struct GcwAnmId {
    unsigned int id;
};

struct GcwAnmResource {
    unsigned char pad0[0x124];
    volatile int loading124;

    void Release();
};

struct GcwAnmVm;

struct GcwAnmNode {
    GcwAnmVm *vm;
    GcwAnmNode *next;
    GcwAnmNode *prev;
};

struct GcwAnmVm {
    GcwAnmId id0;
    GcwAnmNode mgrNode4;
    GcwAnmNode childNode10;       // +0x10 (+0x10 vm, +0x14 next, +0x18 prev)
    unsigned char pad1c[0x42c - 0x1c];
    GcwAnmResource *res42c;       // +0x42c
    unsigned char pad430[0x4b0 - 0x430];
    unsigned int flags4b0;        // +0x4b0
    unsigned char pad4b4[0x4e8 - 0x4b4];

    void ResetForReuse();
    void SetSprite(GcwAnmResource *res, int spriteId);
};

struct GcwAnmManager {
    unsigned char pad0[0x4e90e0];
    GcwAnmResource *slots4e90e0[0x26];
    unsigned char pad4e9178[0x8b9704 - 0x4e9178];
    GcwAnmNode *worldVms8b9704;
    GcwAnmNode *worldTail8b9708;
    GcwAnmNode *uiVms8b970c;

    GcwAnmResource *PreloadAnm(int slot, const char *name);
    GcwAnmResource *LoadAnm(int slot, const char *name);
    void StopVmById(GcwAnmId id, short interrupt);
};

struct GcwErrorCtx {
    char buffer[0x2000];
    char *cursor;
    int fatal;
    const char *__cdecl Log(const char *fmt, ...);
};

struct GcwAsciiInf {
    unsigned char pad0[0xc];
    GcwChainElem *calcC;
    GcwChainElem *draw10;
    GcwAnmVm vm14;                // +0x14 (size 0x4e8 -> +0x4fc)
    GcwAnmVm vm4fc;               // +0x4fc
    unsigned char pad9e4[0x1c894 - 0x9e4];
    GcwAnmResource *asciiAnm1c894;
    unsigned char pad1c898[8];
    GcwChainElem *draw1c8a0;
    GcwChainElem *draw1c8a4;

    int Init();
};

struct GcwStageBgMini {
    int dummy;
    ~GcwStageBgMini();
};

struct GcwEnemyFlagMgr {
    unsigned char pad0[8];
    GcwChainElem *calc8;
    GcwChainElem *drawC;
    unsigned char pad10[0x3c - 0x10];
    int flag3c;
    GcwAnmId vm40;
    GcwAnmId vm44;
    GcwAnmId vm48;
    unsigned char pad4c[0x4f8 - 0x4c];
    void *buf4f8;
    unsigned char pad4fc[0x534 - 0x4fc];
    void *buf534;
    GcwStageBgMini *bgMini538;
    unsigned char pad53c[0x554 - 0x53c];
    GcwChainElem *extra554;

    void Cleanup();
};

struct GcwPlayer {
    unsigned char pad0[0x37f8];
    unsigned int flags37f8;
};

struct GcwReplayMgr {
    unsigned char pad0[0x10];
    int mode10;
};

struct GcwScoreSpell {
    char name[0x80];
    int clearCount80;
    int noMissCount84;
    int seenCount88;
    int attempts8c;
    unsigned char pad90[8];
};

struct GcwScoreFile {
    unsigned char pad0[0x37a0];
    GcwScoreSpell spells37a0[256];
};

struct GcwPopupMgr {
    int dummy;
    void __stdcall Show(int kind, int value);
};

struct GcwSpellCard {
    unsigned char pad0[0x10];
    GcwAnmId vm10;
    GcwAnmId vm14;
    GcwAnmId vm18;
    GcwAnmId vm1c;
    GcwAnmId vm20;
    unsigned char pad24[0x78 - 0x24];
    int spellId78;
    unsigned int flags7c;
    int bonus80;

    static void Finish();
};

struct GcwEclFile {
    virtual void V00() = 0;
    virtual void V01() = 0;
    virtual int __stdcall LoadInclude(const char *name) = 0; // +0x08
    void *bufsC[0x20];   // +0x0c..+0x8b
    void *mainBuf8c;     // +0x8c

    int __stdcall LoadDeps(const unsigned int *header);
};

struct GcwEclConcrete {
    void *vtable0;
    unsigned char pad4[8];
    void *bufsC[0x20];
    void *mainBuf8c;

    ~GcwEclConcrete();
};

struct GcwEnemyMgr {
    unsigned char pad0[8];
    GcwChainElem *calc8;
    GcwChainElem *drawC;
    unsigned char pad10[0x84 - 0x10];
    GcwAnmResource *anms84[8];
    unsigned char padA4[0xb4 - 0xa4];
    GcwEclConcrete *eclB4;

    void Cleanup();
};

struct GcwGui {
    unsigned char pad0[8];
    GcwChainElem *calc8;
    unsigned char padC[0x5860 - 0x0c];
    GcwAnmId digitVms5860[10];     // +0x5860..+0x5884
    GcwAnmId vm5888;
    GcwAnmId vm588c;
    unsigned int pad5890[2];
    GcwAnmId vm5898;
    int vals589c[10];              // +0x589c..+0x58c0
    unsigned char pad58c4[0x5914 - 0x58c4];
    int field5914;
    unsigned char pad5918[0x5954 - 0x5918];
    int field5954;
    unsigned char pad5958[0x5968 - 0x5958];
    int field5968;
    unsigned char pad596c[0x59a8 - 0x596c];
    int field59a8;
    unsigned char pad59ac[0x59bc - 0x59ac];
    int field59bc;
    unsigned char pad59c0[0x59fc - 0x59c0];
    int field59fc;
    int pad5a00;
    unsigned int flags5a04;
    unsigned char pad5a08[0x5a1c - 0x5a08];
    void *msgObj5a1c;
    void *msgBuf5a20;

    void Reset();
};

extern GcwChain *g_GcwChain;
extern GcwAnmManager *g_GcwAnmMgr;
extern GcwErrorCtx g_GcwErrorCtx;
extern unsigned int g_GcwSupFlags;
extern CRITICAL_SECTION g_GcwSupCs0;
extern unsigned char g_GcwSupCsDepth0;
extern int g_GcwGrazeA;
extern int g_GcwGrazeB;
extern GcwEnemyFlagMgr *g_GcwEnemyFlagMgr;
extern GcwSpellCard *g_GcwSpellCard;
extern GcwPlayer *g_GcwPlayer;
extern GcwReplayMgr *g_GcwReplayMgr;
extern GcwScoreFile *g_GcwScoreFile;
extern GcwPopupMgr *g_GcwPopupMgr;
extern int g_GcwScore;
extern GcwEnemyMgr *g_GcwEnemyMgr;
extern unsigned int g_GcwGameFlags;
extern void *g_GcwMsgBufGlobal;

int GcwAsciiCalc(void *);
int GcwAsciiDraw0(void *);
int GcwAsciiDraw1(void *);
int GcwAsciiDraw2(void *);
void GcwDeleteVmByIdPtr(GcwAnmId *id);
int GcwAddGrazeOrPoints(int amount);
void GcwPlaySfx(int id);
void __cdecl GcwDebugLog(const char *fmt, ...);
void __stdcall GcwEnemyMgrResetList(GcwEnemyMgr *mgr);
void GcwDestroyMsgObj(void *obj);

} // namespace th128
