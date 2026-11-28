#pragma once

#include <windows.h>

namespace th128 {

struct CsbThreadInf {
    unsigned char pad00[0x04];
    HANDLE handle;
    unsigned int threadId;
    int f0c;
    int f10;
    unsigned char pad14[0x04];
    void *proc;

    void Stop();
};

struct CsbTitleMgr {
    unsigned char pad00[0x5bd0];
    CsbThreadInf thread;

    CsbTitleMgr();
};

struct CsbLaserBase {
    unsigned char data[0x5c0];
    CsbLaserBase();
};

struct CsbLaserMgr {
    unsigned char pad00[0x10];
    CsbLaserBase base;
    unsigned char pad5d0[0x08];
    int f5d8;
    unsigned char pad5dc[0x24];

    CsbLaserMgr();
    ~CsbLaserMgr();
};

struct CsbAnmRes {
    void Release();
};

struct CsbAnmMgr {
    unsigned char pad00[0x4e90d4];
    CsbAnmRes *slot5;
    CsbAnmRes *slot6;
};

struct CsbEnemyNode {
    struct CsbEnemy *enemy;
    CsbEnemyNode *next;
    CsbEnemyNode *prev;
};

struct CsbEnemy {
    unsigned char pad00[0x145c];
    CsbEnemyNode node;
};

struct CsbEnemyMgr {
    unsigned char pad00[0xb8];
    CsbEnemyNode *head;
    CsbEnemyNode *tail;
    int count;
};

struct CsbChainElem {
    CsbChainElem *prev;
    CsbChainElem *next;
    unsigned int flags;
};

struct CsbChain {
    CsbChainElem *head;
    CsbChainElem *tail;
};

struct CsbFogMgr {
    unsigned char pad00[0x0c];
    CsbChainElem *drawElem;

    ~CsbFogMgr();
};

struct CsbItemMgr2 {
    unsigned int flags;
    unsigned char pad04[0x20];

    CsbItemMgr2();
    ~CsbItemMgr2();
};

struct CsbGuiSubObj {
    unsigned char pad00[0x40];
    int vmIds[10];

    CsbGuiSubObj *Destroy();
};

extern int g_CsbTitleFlag;
extern CsbLaserMgr *g_CsbLaserMgr;
extern CsbAnmMgr *g_CsbAnmMgr;
extern CsbEnemyMgr *g_CsbEnemyMgr;
extern int g_CsbFreezeGauge;
extern CsbChain *g_CsbChain;
extern unsigned int g_CsbFlags;
extern CRITICAL_SECTION g_CsbChainCs;
extern unsigned char g_CsbChainCsDepth;
extern CsbFogMgr *g_CsbFogMgr;
extern CsbItemMgr2 *g_CsbItemMgr2;

unsigned __stdcall CsbTitleWorker(void *arg);
int __stdcall CsbInitLaserMgr(CsbLaserMgr *mgr);
int __stdcall CsbInitItemMgr2(CsbItemMgr2 *mgr);
void CsbPlaySfx(int id);
void __fastcall CsbChainCut(CsbChain *chain, CsbChainElem *elem);
void CsbDeleteVmByIdPtr(int *idPtr);

CsbTitleMgr *CsbCreateTitleMgr();
CsbLaserMgr *CsbCreateLaserMgr();
int CsbReleaseEndingAnms();
const char *CsbReadShiftJisLine(const char *src, char *dst, int maxLen);
void CsbUnlinkEnemy(CsbEnemy *enemy);
void __stdcall CsbAddFreezeGauge(int delta);
CsbItemMgr2 *CsbCreateItemMgr2();
char *__stdcall CsbReadTextLine(char *src, char *dst, int *remaining);

} // namespace th128
