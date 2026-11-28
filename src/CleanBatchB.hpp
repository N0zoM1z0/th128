#pragma once

#include <windows.h>

struct CbbListNode {
    void *data;
    CbbListNode *next;
    CbbListNode *prev;
};

struct CbbAnmVm {
    int id;
    unsigned char pad004[0xc];
    CbbListNode listNode; // +0x10 (+0x10 data, +0x14 next, +0x18 prev)
    unsigned char pad01c[0x402];
    short scriptId;       // +0x41e
    unsigned char pad420[0xc];
    void *anmFile;        // +0x42c
    unsigned char pad430[0x80];
    unsigned int flags;   // +0x4b0
};

struct CbbAnmMgr {
    unsigned char pad000[0x4e90e0];
    void *enemyFiles[8];  // +0x4e90e0
    unsigned char pad4e9100[0x3d0604];
    CbbListNode *worldHead; // +0x8b9704
    int pad8b9708;
    CbbListNode *uiHead;    // +0x8b970c

    CbbAnmVm *FindVm(int id);
};

// 1. 0x00410ea0 (222B)
struct CbbEnemyItem {
    virtual void V0() {}
    virtual void V1() {}
    virtual void V2() {}
    virtual void V3() {}
    virtual void V4() {}
    virtual void Destroy(int flags);
};
struct CbbFlagHolder {
    int pad0;
    unsigned int flags;
};
struct CbbEnemyMgr {
    int pad00[2];
    CbbFlagHolder *holder8; // +0x08
    CbbFlagHolder *holderC; // +0x0c
    unsigned char pad10[0x4c];
    void *field5c;          // +0x5c
    unsigned char pad60[0x58];
    CbbListNode *listHead;  // +0xb8
    int padbc;
    int padc0;
    void *fieldc4;          // +0xc4
};

// 2. 0x0046dc90 (193B)
struct CbbWaveFile {
    int pad00[2];
    DWORD remBytes;       // +0x08
    unsigned char pad0c[0x70];
    int fromMemory;       // +0x7c
    unsigned char *memBase; // +0x80
    unsigned char *memCur;  // +0x84
    DWORD memSize;        // +0x88
    HANDLE fileHandle;    // +0x8c

    HRESULT Read(unsigned char *buf, DWORD sizeToRead, DWORD *sizeRead);
};

// 3. 0x0043c280 (162B)
struct CbbStageRec {
    signed char type;
    unsigned char pad01[0x23];
    int f24;
    int f28;
    int f2c;
    int f30;
}; // 0x34 bytes
struct CbbStageTableHdr {
    short pad00;
    unsigned short count; // +0x02
    unsigned char pad04[0x264];
    struct {
        unsigned int offset;
        int pad;
    } entries[64]; // +0x268, stride 8
};
struct CbbStageLoader {
    unsigned char pad000[0xf7c];
    CbbStageTableHdr *hdr;

    int Load(const char *path);
};

// 4, 6, 9: Constructors with MenuCursor + memset
struct CbbTimer {
    unsigned int flags;
    CbbTimer() { flags &= ~1U; }
};
struct CbbMenuBlock {
    int current;            // +0x00
    int pad04;              // +0x04
    int count;              // +0x08
    unsigned char pad0c[0x80];
    int f8c;                // +0x8c
    unsigned char pad90[0x40];
    int fd0;                // +0xd0
    int fd4;                // +0xd4
    CbbMenuBlock() {
        f8c = 0;
        current = 0;
        fd4 = 0;
        fd0 = 1;
        count = 999;
    }
};
struct CbbSub2c8 {
    int x;
    CbbSub2c8();
};
struct CbbBase10 {
    virtual void V0() {}
    int f04, f08, f0c, f10;
    CbbBase10() : f04(0), f08(0), f0c(0), f10(0) {}
};

// 4. 0x00410fa0 (155B)
struct CbbEndingMgr {
    unsigned int flags;     // +0x00
    unsigned char pad04[0xc];
    CbbBase10 base10;       // +0x10..+0x23
    unsigned char pad24[0x18];
    CbbMenuBlock cur0;      // +0x3c..+0x113
    CbbMenuBlock cur1;      // +0x114..+0x1eb
    CbbMenuBlock cur2;      // +0x1ec..+0x2c3
    int pad2c4;
    CbbSub2c8 sub2c8;       // +0x2c8
    unsigned char pad2cc[0x4f8]; // total 0x7c4

    CbbEndingMgr();
};

// 5. 0x00427260 (152B)
struct CbbGui {
    int dummy;
    void ShowBanner(int type, int a, int b);
};

// 6. 0x004433a0 (142B)
struct CbbScoreMgr {
    unsigned int flags;     // +0x00
    unsigned char pad04[0xc];
    CbbBase10 base10;       // +0x10..+0x23
    unsigned char pad24[0x10];
    CbbMenuBlock cur0;      // +0x34..+0x10b
    CbbMenuBlock cur1;      // +0x10c..+0x1e3
    CbbMenuBlock cur2;      // +0x1e4..+0x2bb

    CbbScoreMgr();
};

// 7. 0x00442630 (137B)
struct CbbReplayBlock {
    unsigned char inputs[0x1518]; // +0x0000
    unsigned char *inputCursor;   // +0x1518
    unsigned char extra[0x384];   // +0x151c
    unsigned char *extraCursor;   // +0x18a0
    CbbListNode node;             // +0x18a4..+0x18af (total 0x18b0)
    CbbReplayBlock() {
        memset(this, 0, sizeof(CbbReplayBlock));
        extraCursor = extra;
        inputCursor = inputs;
        node.data = this;
        node.next = 0;
        node.prev = 0;
    }
};
struct CbbReplayList {
    CbbListNode *dummy;
    CbbListNode *head;
    CbbListNode *tail;
};
struct CbbReplayMgr {
    unsigned char pad00[0x64];
    CbbReplayList lists[17]; // +0x64, stride 12 -> lists[i].head at +0x68
    unsigned char pad130[4];
    int blockCount;          // +0x134

    CbbListNode *AllocStageBlock(int stage);
};

// 8. 0x004277c0 (119B)
struct CbbSubObj {
    int version;           // +0x00
    unsigned char cfg[18]; // +0x04..+0x15
    short w16;             // +0x16
    short w18;             // +0x18
    unsigned char b1a;     // +0x1a
    unsigned char b1b;     // +0x1b
    unsigned char b1c;     // +0x1c
    unsigned char b1d;     // +0x1d
    unsigned char b1e;     // +0x1e
    unsigned char b1f;     // +0x1f
    unsigned char b20;     // +0x20
    unsigned char b21;     // +0x21
    short w22;             // +0x22
    int f24;               // +0x24
    unsigned int f28;      // +0x28
    unsigned int f2c;      // +0x2c
    int f30;               // +0x30
    int f34;               // +0x34
    unsigned int flags38;  // +0x38

    void Init();
};

// 9. 0x00436400 (104B)
struct CbbReplayMenu {
    unsigned int flags;     // +0x00
    unsigned char pad04[0x1c];
    CbbTimer t20;           // +0x20
    unsigned char pad24[0x10];
    CbbTimer t34;           // +0x34
    CbbMenuBlock cur0;      // +0x38..+0x10f
    CbbMenuBlock cur1;      // +0x110..+0x1e7
    unsigned char pad1e8[0xf8]; // total 0x2e0

    CbbReplayMenu();
};

// 10 & 11. 0x00445010 (99B) & 0x00444fa0 (99B)
struct CbbTitleScreen {
    unsigned char pad000[0x2c8];
    int anmIds[210];        // +0x2c8
    int anmId610;           // +0x610

    void ClearChildFlag(int slot, int scriptId);
    void SetChildFlag(int scriptId);
};

// Globals & externs
extern CbbAnmMgr *g_CbbAnmMgr;
extern int g_CbbTblA[16];
extern int g_CbbTblB[16];
extern int g_CbbTblC[16];
extern int g_CbbTblD[16];
extern CbbEndingMgr *g_CbbEndingMgr;
extern int g_CbbMotiv;
extern int g_CbbMotivLocked;
extern CbbGui *g_CbbGui;
extern CbbScoreMgr *g_CbbScoreMgr;
extern unsigned char g_CbbDefaultCfg[18];
extern CbbReplayMenu *g_CbbReplayMenu;

void __stdcall CbbCleanupEnemies(CbbEnemyMgr *mgr);
void *__stdcall CbbOpenFile(const char *path, int *outSize, int isBinary);
void CbbPlaySfx(int id);
void __stdcall CbbAddMotivation(int delta);
