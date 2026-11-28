#pragma once

#include <windows.h>

// 1. 0x00420020 (165B)
struct ZcaGaugeObj {
    unsigned char pad00[0xa8];
    int valA8;
};

// 2. 0x00450d70 (134B)
struct ZcaLzssNode {
    int parent;
    int left;
    int right;
};

// 3. 0x00412700 (131B)
struct ZcaVertex {
    int d[7]; // 28 = 0x1c bytes
};
struct ZcaMeshStrip {
    unsigned char pad000[0x4ac];
    ZcaVertex *verts;
};
struct ZcaMesh {
    int rows;             // +0x00
    int cols;             // +0x04
    int pad08;            // +0x08
    ZcaMeshStrip **strips; // +0x0c
    ZcaVertex *src;       // +0x10
};

// 4. 0x004273a0 (120B)
struct ZcaStageMgr {
    unsigned char pad00[0x74];
    int flag74;
};
struct ZcaStageInfo {
    unsigned char data[0x28];
};

// 5. 0x004142e0 (116B)
struct ZcaEnemySlot {
    unsigned int flags; // +0x00
    int val;            // +0x04
    unsigned char pad08[0x24]; // total 0x2c
};
struct ZcaEnemySub {
    unsigned char pad0000[0x1008];
    int f1008;
    int f100c;
    unsigned char pad1010[0x30];
    ZcaEnemySlot slots[8]; // +0x1040, stride 0x2c
};

// 7. 0x00415270 (97B)
struct ZcaListNode {
    void *data;
    ZcaListNode *next;
    ZcaListNode *prev;
};
struct ZcaEnemyMgr {
    unsigned char pad00[0xb8];
    ZcaListNode *head; // +0xb8
    ZcaListNode *tail; // +0xbc
    int count;         // +0xc0
};
struct ZcaEnemy {
    unsigned char pad0000[0x145c];
    ZcaListNode node;  // +0x145c
};

// 11. 0x00468190 (57B)
struct ZcaSubList {
    int f00;
    unsigned int flags; // +0x04
    int f08;
    int f0c;
    int f10;
    ZcaSubList *self;   // +0x14
    int f18;
    int f1c;
    int f20;            // total 0x24
};
struct ZcaTwoLists {
    ZcaSubList a;       // +0x00
    ZcaSubList b;       // +0x24
    int f48;
    int f4c;
};

// 12. 0x0041de50 (56B)
struct ZcaDropEntry {
    int type;
    int a;
    int b;
    int c;
};
struct ZcaDropOwner {
    unsigned char pad0000[0x50c4];
    ZcaDropEntry drops[8]; // +0x50c4, stride 0x10

    void SetDrop(int idx, int type, int a, int b, int c);
};

// 13 & 15. 0x0040ccb0 (56B) & 0x0042abd0 (54B)
struct ZcaBulletInner {
    unsigned char pad0000[0x11b0];
    short s11b0;
    short s11b2;
};
struct ZcaBullet {
    unsigned char pad000[0x4e4];
    ZcaBulletInner *inner;
};
struct ZcaLaserInner {
    unsigned char pad000[0x5b8];
    int i5b8;
    int i5bc;
};
struct ZcaLaser {
    unsigned char pad000[0x4e4];
    ZcaLaserInner *inner;
};

struct ZcaReplayBuf {
    unsigned char bytes[1];
    int ChecksumBytes(int len);
};

// 14. 0x0045dfd0 (54B)
struct ZcaAnmSlot {
    unsigned char pad000[0x120];
    int field120;
};
struct ZcaAnmMgr {
    unsigned char pad000[0x4e90c0];
    ZcaAnmSlot *slots[256];
};
struct ZcaAnmVm {
    unsigned char pad000[0x428];
    int *ptr428;
};

// 16. 0x004427b0 (51B)
struct ZcaReplaySlot {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    int f14;
    ZcaReplaySlot *self;
    int f1c;
    int f20;

    ZcaReplaySlot();
};

// Globals
extern ZcaGaugeObj *g_ZcaGaugeObj;
extern ZcaLzssNode g_ZcaLzssTree[4097];
extern ZcaStageMgr *g_ZcaStageMgr;
extern int g_ZcaStage;
extern int g_ZcaRoute;
extern ZcaStageInfo g_ZcaStageTable[32];
extern ZcaStageInfo *g_ZcaStagePtr;
extern ZcaEnemyMgr *g_ZcaEnemyMgr;
extern int g_ZcaTextBytes;
extern unsigned short *g_ZcaTextPixels;
extern int g_ZcaBulletTable[32][18][4];
extern ZcaAnmMgr *g_ZcaAnmMgr;

// Functions
void __stdcall ZcaDecodeGauge(int *outA, int *outB);
void ZcaLzssReplaceNode(int oldNode, int newNode);
void ZcaCopyMeshStrip(ZcaMesh *m);
void __stdcall ZcaAdvanceStage(int branch);
void ZcaResetEnemySub(ZcaEnemySub *s);
int ZcaChecksumDwords(const int *data, int byteLen);
void ZcaUnlinkEnemy(ZcaEnemy *e);
char *__stdcall ZcaReadLine(char *src, int *rem, char *dst);
void ZcaFillText4444(unsigned int color);
void ZcaResetTwoLists(ZcaTwoLists *p);
int __fastcall ZcaGetBulletParam16(ZcaBullet *b, int idx);
int ZcaIsAnmLoaded(ZcaAnmVm *vm);
int __fastcall ZcaGetBulletParam32(ZcaLaser *l, int idx);
