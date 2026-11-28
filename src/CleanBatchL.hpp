#pragma once

#include <windows.h>

namespace th128 {

struct CblStageHeader {
    int pad0;
    int frameCount;
};

struct CblReplayStage {
    unsigned short* inputPtr;  // +0x00 (total +0x13c)
    int pad4;                  // +0x04 (+0x140)
    unsigned char* slowPtr;    // +0x08 (+0x144)
    CblStageHeader* header;    // +0x0c (+0x148)
    int frame;                 // +0x10 (+0x14c)
    int pad14[4];              // +0x14..+0x20
};

struct CblReplay {
    unsigned char pad0[0x13c];
    CblReplayStage stages[16];
    unsigned char pad37c[0x3a0 - 0x37c];
    unsigned char slowByte3a0;
    unsigned char pad3a1[3];
    int totalFrames3a4;
    int pad3a8;
    int curStage3ac;

    int StepInput();
};

struct CblInputObj {
    int dummy;
    void Update();
};

struct CblScoreEntry {
    unsigned char data[0x66c];
};

struct CblScoreMgr {
    unsigned char pad0[8];
    CblScoreEntry entries[8];
    unsigned char pad3368[0x3374 - (8 + 8 * 0x66c)];
    char lastName3374[16];
};

struct CblPauseMenu {
    unsigned char pad0[0x38];
    int field38;
    int pad3c;
    int field40;
    unsigned char pad44[0x108 - 0x44];
    int field108;
    int pad10c;
    int field110;
    int pad114;
    int field118;
    unsigned char pad11c[0x1e0 - 0x11c];
    int field1e0;
    unsigned char pad1e4[0x1f0 - 0x1e4];
    int nameLen1f0;
    int field1f4;
    int field1f8;
    unsigned char pad1fc[0x2cc - 0x1fc];
    char name2cc[16];

    void SetupNameEntry();
};

struct CblChainNode {
    int pad0;
    unsigned int flags4;
};

struct CblBgHeader {
    short count;
};

struct CblBgObj {
    unsigned char pad0[3];
    unsigned char flag3;
};

struct CblBgQuad {
    short type;      // +0
    short size2;     // +2
    short scriptId4; // +4
    short vmIdx6;    // +6
};

struct CblBgVm {
    unsigned char pad0[0x4d0];
    unsigned short w4d0;
    unsigned char pad4d2[0x4e8 - 0x4d2];
};

struct CblStageBg {
    unsigned char pad0[8];
    CblChainNode* chain8;
    CblChainNode* chainC;
    CblBgHeader* header10;
    CblBgObj** objs14;
    int pad18;
    int field1c;
    unsigned char pad20[0x4c - 0x20];
    int field4c;
    unsigned char pad50[0x1c4 - 0x50];
    void* anm1c4;
    CblBgVm* vms1c8;
    unsigned char pad1cc[0x383c - 0x1cc];
    CblChainNode* chain383c;
};

struct CblAnmVm {
    unsigned char pad0[0x42c];
    unsigned int tag42c;
    unsigned char pad430[0x4b0 - 0x430];
    unsigned int flags4b0;
};

struct CblAnmNode {
    CblAnmVm* vm;
    CblAnmNode* next;
};

struct CblAnmMgr {
    unsigned char pad0[0x4e90e0];
    unsigned int slotIds[8];
    unsigned char pad4e9100[0x8b9704 - 0x4e9100];
    CblAnmNode* listA;
    int pad8b9708;
    CblAnmNode* listB;
};

struct CblEnemyBase {
    virtual void V0() = 0;
    virtual void V1() = 0;
    virtual void V2() = 0;
    virtual void V3() = 0;
    virtual void V4() = 0;
    virtual void V5(int flag) = 0; // +0x14
};

struct CblEnemyNode {
    CblEnemyBase* obj;
    CblEnemyNode* next;
};

struct CblEnemyMgr {
    unsigned char pad0[8];
    CblChainNode* chain8;
    CblChainNode* chainC;
    unsigned char pad10[0x5c - 0x10];
    int field5c;
    unsigned char pad60[0xb8 - 0x60];
    CblEnemyNode* enemiesB8;
    unsigned char padBc[0xc4 - 0xbc];
    int fieldC4;
};

struct CblKeyConfig {
    unsigned char pad0[0x5a1c];
    short keys[5];
};

struct CblGame2 {
    unsigned char pad0[0xa8];
    int valA8;
};

struct CblStageScoreSlot {
    int value;
    int pad[8];
};

struct CblScoreTable {
    unsigned char pad0[0x148];
    int field148;
    unsigned char pad14c[0x16c - 0x14c];
    CblStageScoreSlot arr16c[5];
    CblStageScoreSlot arr220[5];
    CblStageScoreSlot arr2d4[5];
    int field388;

    int GetStageValue(int mode, int idx);
};

extern int g_CblReplayActive;
extern unsigned int g_CblInput0;
extern unsigned int g_CblInput4;
extern unsigned int g_CblInputC;
extern unsigned int g_CblInput10;
extern CblInputObj g_CblInputObj;
extern int g_CblMode;
extern int g_CblMode2;
extern int g_CblDiff;
extern void* g_CblModePtr;
extern int g_CblData1;
extern int g_CblData2;
extern CblScoreMgr* g_CblScoreMgr;
extern CblStageBg* g_CblStageBg;
extern CblAnmMgr* g_CblAnmMgr;
extern CblGame2* g_CblGame2;

void CblOnReplayEnd();
int __stdcall CblFindScoreRank(void* entry);
void __stdcall CblPlaySe(int* timer, int id);
void CblInitVm(void* anm, CblBgVm* vm, int scriptId);
void CblUpdateVm(CblBgVm* vm);
struct CblSeMgr {
    int dummy;
    void PlaySe(int id);
};
extern CblSeMgr g_CblSeMgr;

void CblSaveConfig(CblKeyConfig* cfg);

void CblStageBgInit();
void __stdcall CblEnemyMgrClearAll(CblEnemyMgr* self);
void CblSwapKey(int slot, int newKey, CblKeyConfig* cfg);
void __stdcall CblDecodeFreezePercent(int* outWhole, int* outFrac);

} // namespace th128
