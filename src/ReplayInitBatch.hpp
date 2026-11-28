#ifndef TH128_REPLAY_INIT_BATCH_HPP
#define TH128_REPLAY_INIT_BATCH_HPP

#include "Chain.hpp"
#include <string.h>

struct RibFileHeader {
    unsigned int magic;             // +0x00
    unsigned short version;         // +0x04
    unsigned short pad06;           // +0x06
    unsigned int pad08;             // +0x08
    unsigned int userdataOffset;    // +0x0c
    unsigned int field10;           // +0x10
    unsigned int pad14;             // +0x14
    unsigned int pad18;             // +0x18
    unsigned int compSize;          // +0x1c
    unsigned int origSize;          // +0x20

    RibFileHeader() {
        memset(this, 0, sizeof(RibFileHeader));
        magic = 0x72383231;
        version = 2;
        field10 = 0x100;
    }
};

struct RibSubObj {
    unsigned char data[0x3c];
    RibSubObj();
};

struct RibDataHeader {
    char name[10];                  // +0x00
    unsigned char flagA : 1;        // +0x0a bit 0
    unsigned char pad0b[13];        // +0x0b..+0x17
    RibSubObj sub;                  // +0x18..+0x53 (0x3c bytes = 15 dwords)
    float slowRate;                 // +0x54
    int stageCount;                 // +0x58
    int route;                      // +0x5c
    int field60;                    // +0x60
    int rank;                       // +0x64
    int clearFlag;                  // +0x68
    int field6c;                    // +0x6c (total 0x70)

    RibDataHeader();
};

struct RibStageHeader {
    unsigned short stage;           // +0x00
    unsigned short seed;            // +0x02
    int frameCount;                 // +0x04
    int dataSize;                   // +0x08
    int score;                      // +0x0c
    short field10;                  // +0x10
    short pad12;                    // +0x12
    int field14;                    // +0x14
    int field18;                    // +0x18
    int field1c;                    // +0x1c
    int field20;                    // +0x20
    int pad24;                      // +0x24
    int field28;                    // +0x28
    int pad2c;                      // +0x2c
    unsigned int array30[20];       // +0x30..+0x7f
    int field80;                    // +0x80
    int field84;                    // +0x84
    float field88;                  // +0x88
    unsigned int flag8c : 1;        // +0x8c bit 0 (total 0x90)

    RibStageHeader();
};

struct RibPlaybackSlot {
    void *bufA;                     // +0x00 (+0x138)
    void *curA;                     // +0x04 (+0x13c)
    void *bufB;                     // +0x08 (+0x140)
    void *curB;                     // +0x0c (+0x144)
    RibStageHeader *stageHeader;    // +0x10 (+0x148)
    int state;                      // +0x14 (+0x14c)
    int pad18[3];                   // +0x18..+0x23 (total 0x24)
};

struct RibReplayMgr {
    unsigned char pad00[8];
    ChainElem *calcElem0;           // +0x08
    ChainElem *drawElem;            // +0x0c
    int mode;                       // +0x10
    int pad14;                      // +0x14
    RibFileHeader *fileHeader;      // +0x18
    RibDataHeader *dataHeader;      // +0x1c
    RibStageHeader *stageHeaders[17];// +0x20..+0x63
    unsigned char pad64[0xcc];      // +0x64..+0x12f
    void *curBlockNode;             // +0x130
    int pad134;                     // +0x134
    RibPlaybackSlot slots[17];      // +0x138..+0x39b (17 * 0x24 = 0x264)
    unsigned char pad39c[8];        // +0x39c..+0x3a3
    int field3a4;                   // +0x3a4
    ChainElem *calcElem1;           // +0x3a8
    int curStage;                   // +0x3ac

    void ResetStageBlocks(int stage);
    void *AllocStageBlock(int stage);
    int LoadReplay(const char *filename);
    int Initialize(const char *filename, int mode);
};

struct RibPlayerData {
    unsigned char pad00[0x24];
    RibSubObj sub;                  // +0x24..+0x5f (15 dwords)
};

struct RibGameState {
    int pad00;                      // +0x00 (0x4b4cc0)
    int score;                      // +0x04 (0x4b4cc4)
    int field08;                    // +0x08 (0x4b4cc8)
    unsigned char pad0c[0x10];
    int field1c;                    // +0x1c (0x4b4cdc)
    unsigned char pad20[0x18];
    int field38;                    // +0x38 (0x4b4cf8)
    unsigned char pad3c[0x10];
    int rank;                       // +0x4c (0x4b4d0c)
    int pad50;                      // +0x50
    int stageNum;                   // +0x54 (0x4b4d14)
    unsigned char pad58[0x10];
    int field68;                    // +0x68 (0x4b4d28)
    unsigned char pad6c[0x08];
    int maxVal;                     // +0x74 (0x4b4d34)
    int minVal;                     // +0x78 (0x4b4d38)
    int pad7c;                      // +0x7c
    int field80;                    // +0x80 (0x4b4d40)
    int route;                      // +0x84 (0x4b4d44)
    unsigned char pad88[0x08];
    unsigned int flags;             // +0x90 (0x4b4d50)
    unsigned char pad94[0x0c];
    float floatA0;                  // +0xa0 (0x4b4d60)
    int fieldA4;                    // +0xa4 (0x4b4d64)
    int fieldA8;                    // +0xa8 (0x4b4d68)
    int padAC;                      // +0xac
    int fieldB0;                    // +0xb0 (0x4b4d70)

    short GetVal();
};

extern RibReplayMgr *g_RibReplayMgr;    // 0x4b8a84
extern RibPlayerData *g_RibPlayerData;  // 0x4b8954
extern RibGameState g_RibGameState;     // 0x4b4cc0
extern unsigned short g_RibRngSeed;     // 0x4d2ae0
extern int g_RibRngCount;               // 0x4d2ae4
extern unsigned int g_RibFlag4d33cc;    // 0x4d33cc

int __fastcall RibCalc0(void *arg);     // 0x442010
int __fastcall RibCalc1(void *arg);     // 0x442020
int __fastcall RibCalc2(void *arg);     // 0x442030
int __fastcall RibDraw(void *arg);      // 0x442070

#endif
