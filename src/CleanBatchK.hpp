#pragma once

#include <windows.h>

namespace th128 {

struct CbkGui {
    unsigned char pad0[0x10];
    unsigned char vmsA[8][0x4e8];
    unsigned char vmsB[8][0x4e8];
    unsigned char pad4e90[0x5378 - 0x4e90];
    unsigned char vm5378[0x4e8];
    unsigned char pad5860[0x58fc - 0x5860];
    int displayScore;
    int scoreStep;
    unsigned char pad5904[0x5a04 - 0x5904];
    unsigned char flags5a04;

    int Draw();
};

struct CbkEnemyBoss {
    unsigned char pad0[0x50ac];
    unsigned int flags50ac;
};

struct CbkEnemyMgr {
    unsigned char pad0[0x5c];
    CbkEnemyBoss* boss;
};

struct CbkThreadBase {
    virtual ~CbkThreadBase() { Reset(); }
    void Reset();
};

struct CbkAnmVm {
    unsigned int id;
    unsigned char pad4[0x10];
    struct CbkAnmListNode* children14;
    void* parent18;
    unsigned char pad1c[0x4b0 - 0x1c];
    unsigned int flags4b0;
};

struct CbkAnmListNode {
    CbkAnmVm* vm;
    CbkAnmListNode* next;
};

struct CbkAnmMgr {
    unsigned char pad0[0x8b9704];
    CbkAnmListNode* listA;
    int pad8b9708;
    CbkAnmListNode* listB;

    CbkAnmVm* FindVmInline(unsigned int id) {
        if (id == 0) return 0;
        for (CbkAnmListNode* n = listA; n != 0; n = n->next) {
            if (n->vm->id == id) return n->vm;
        }
        for (CbkAnmListNode* n = listB; n != 0; n = n->next) {
            if (n->vm->id == id) return n->vm;
        }
        return 0;
    }

    CbkAnmVm* FindVm(unsigned int id);
    void DrawVm(void* vm);
};

struct CbkEndingSub {
    unsigned char pad0[0x40];
    unsigned int anmIds[5];
    unsigned char pad54[0xd0 - 0x54];
    CbkThreadBase thread;

    ~CbkEndingSub();
};

struct CbkStageClear {
    unsigned char pad0[0x2910];
    int field2910;
    unsigned char pad2914[0x3834 - 0x2914];
    unsigned int vmId3834;
    unsigned int vmId3838;

    void ClearVms();
};

struct CbkScoreMgr {
    unsigned char pad0[0x338e];
    unsigned char musicUnlocked[32];
};

struct CbkSoundMgr {
    int dummy;
    void PlayBgm(const char* path, int a, int b);
};

#pragma pack(push, 1)
struct CbkConfig {
    unsigned int version;      // +0x00
    unsigned char keyMap[18];  // +0x04
    unsigned short w16;        // +0x16
    unsigned short w18;        // +0x18
    unsigned char b1a;         // +0x1a
    unsigned char b1b;         // +0x1b
    unsigned char b1c;         // +0x1c
    unsigned char b1d;         // +0x1d
    unsigned char b1e;         // +0x1e
    unsigned char b1f;         // +0x1f
    unsigned char b20;         // +0x20
    unsigned char b21;         // +0x21
    unsigned short w22;        // +0x22
    unsigned int pad24;        // +0x24
    unsigned int u28;          // +0x28
    unsigned int u2c;          // +0x2c
    unsigned int pad30;        // +0x30
    unsigned int pad34;        // +0x34
    unsigned int flags38;      // +0x38
};
#pragma pack(pop)

struct CbkEndingSubObj {
    int pad0;
    int timer4;
    unsigned char pad8[0x74 - 8];
    unsigned int flags74;
};

struct CbkEndingObj {
    unsigned char pad0[0x18];
    CbkEndingSubObj* sub18;
    int pad1c;
    unsigned int flags20;
    int frame24;

    int OnCalc();
};

extern int g_CbkHighScore;
extern int g_CbkScore;
extern int g_CbkDifficulty;
extern int g_CbkStage;
extern int g_CbkHighScoreStage;
extern int g_CbkExtendIdx;
extern unsigned int g_CbkStageFlags;
extern int g_CbkNormalExtends[16];
extern int g_CbkExtraExtends[16];
extern CbkGui* g_CbkGui;
extern CbkEnemyMgr* g_CbkEnemyMgr;
extern CbkAnmMgr* g_CbkAnmMgr;
extern CbkScoreMgr* g_CbkScoreMgr;
extern CbkSoundMgr g_CbkSoundMgr;
extern unsigned char g_CbkDefaultKeys[18];
extern unsigned int g_CbkInput;
extern unsigned int g_CbkSupFlags;
extern int g_CbkNextMode;

void CbkShowPopup(CbkGui* gui, int a, int b, int c);
void CbkOnExtend(int* highScorePtr);
void __stdcall CbkDrawVm(void* vm, CbkAnmMgr* mgr);
int __stdcall CbkUpdateEndingSub(CbkEndingSubObj* sub);
void CbkTickTimer(int* timer);

void CbkUpdateScoreDisplay();
int __stdcall CbkPlayMusicTrack(int idx, const char* path);
void CbkInitConfig(CbkConfig* cfg);

} // namespace th128
