#pragma once

#include "ZunMath.hpp"
#include "ZunTimer.hpp"

namespace th128 {

struct Zc4ScoreEntry {
    int score;              // +0x00
    char stage;             // +0x04
    char cont;              // +0x05
    char name[10];          // +0x06
    unsigned int timestampLo; // +0x10
    unsigned int timestampHi; // +0x14
    float slowRate;         // +0x18
    int pad1c;              // +0x1c
};

struct Zc4RouteScore {
    int magic;              // +0x00
    int pad04;              // +0x04
    int size;               // +0x08
    int pad0c;              // +0x0c
    Zc4ScoreEntry entries[5][10]; // +0x10..+0x64f
    unsigned char tail[0x66c - 0x650];
};

struct Zc4LzssNode {
    int parent;
    int left;
    int right;
};

struct Zc4Player {
    unsigned char pad00[0xecc];
    float posX;             // +0xecc
    float posY;             // +0xed0
    unsigned char padEd4[0xf34 - 0xed4];
    float hitHalfW;         // +0xf34
    float hitHalfH;         // +0xf38
};

struct Zc4LaserDesc {
    ZunVec3 pos;            // +0x00..+0x0b
    unsigned int words[0xd5 - 3]; // +0x0c..+0x353
};

struct Zc4AnmVmRaw {
    unsigned int unk00;     // +0x00
    void *node1Self;        // +0x04
    void *node1Next;        // +0x08
    void *node1Prev;        // +0x0c
    void *node2Self;        // +0x10
    void *node2Next;        // +0x14
    void *node2Prev;        // +0x18
    unsigned int pad1c;     // +0x1c
    unsigned int vmId;      // +0x20
    unsigned char pad24[0x40 - 0x24];
    float scaleX;           // +0x40
    float scaleY;           // +0x44
    float scale2X;          // +0x48
    float scale2Y;          // +0x4c
    unsigned char pad50[0x58 - 0x50];
    float uvScaleX;         // +0x58
    float uvScaleY;         // +0x5c
    unsigned char pad60[0x70 - 0x60];
    int timerPrev;          // +0x70
    int timerCur;           // +0x74
    float timerCurF;        // +0x78
    unsigned char pad7c[0xe8 - 0x7c];
    int interp0;            // +0xe8
    unsigned char padEc[0x134 - 0xec];
    int interp1;            // +0x134
    unsigned char pad138[0x160 - 0x138];
    int interp2;            // +0x160
    unsigned char pad164[0x1ac - 0x164];
    int interp3;            // +0x1ac
    unsigned char pad1b0[0x214 - 0x1b0];
    int interp4;            // +0x214
    unsigned char pad218[0x250 - 0x218];
    int interp5;            // +0x250
    unsigned char pad254[0x29c - 0x254];
    int interp6;            // +0x29c
    unsigned char pad2a0[0x2c8 - 0x2a0];
    int interp7;            // +0x2c8
    unsigned char pad2cc[0x2f4 - 0x2cc];
    int interp8;            // +0x2f4
    unsigned char pad2f8[0x320 - 0x2f8];
    int interp9;            // +0x320
    unsigned char pad324[0x330 - 0x324];
    float mat[16];          // +0x330..+0x36f
    unsigned char pad370[0x3f0 - 0x370];
    unsigned int color;     // +0x3f0
    unsigned char pad3f4[0x464 - 0x3f4];
    ZunVec3 pos;            // +0x464..+0x46f
    unsigned char pad470[0x4b0 - 0x470];
    union {
        unsigned int flags;
        struct {
            unsigned short flagsLo;
            unsigned short flagsHi;
        };
    };                      // +0x4b0
    unsigned int stateFlags;// +0x4b4
    unsigned char pad4b8[0x4e8 - 0x4b8];

    void ResetForReuse();
};

struct Zc4LaserLine {
    unsigned char pad00[0x0c];
    int unk0c;              // +0x0c
    int unk10;              // +0x10
    unsigned char pad14[0x50 - 0x14];
    ZunVec3 pos;            // +0x50..+0x5b
    unsigned char pad5c[0x68 - 0x5c];
    float unk68;            // +0x68
    float width;            // +0x6c
    float unk70;            // +0x70
    unsigned char pad74[0x80 - 0x74];
    int unk80;              // +0x80
    unsigned char pad84[0x5bc - 0x84];
    int unk5bc;             // +0x5bc
    Zc4LaserDesc desc;      // +0x5c0..+0x913
    Zc4AnmVmRaw vm;         // +0x914..+0xdfb
    float unkDfc;           // +0xdfc
    float widths[512];      // +0xe00..+0x15ff

    int Init(const Zc4LaserDesc *d);
};

struct Zc4EnemyInterrupt {
    int hpThreshold;        // +0x00
    int timeLimit;          // +0x04
    int subIndex;           // +0x08
    int unk0c;              // +0x0c
};

struct Zc4Enemy {
    unsigned char pad00[0x1434];
    ZunTimer timer;         // +0x1434..+0x1447
    unsigned char pad1448[0x5004 - 0x1448];
    int hp;                 // +0x5004
    int pad5008;            // +0x5008
    int hpRemaining;        // +0x500c
    int pad5010;            // +0x5010
    int hpFloor;            // +0x5014
    unsigned char pad5018[0x50ac - 0x5018];
    unsigned int flags;     // +0x50ac
    unsigned char pad50b0[0x50c4 - 0x50b0];
    Zc4EnemyInterrupt interrupts[8]; // +0x50c4..+0x5143

    int CheckSpellInterrupts();
};

struct Zc4Gui {
    unsigned char pad00[0x5a24];
    int timerSec;           // +0x5a24
    int timerCs;            // +0x5a28
};

struct Zc4CardInf {
    unsigned char pad00[0x28];
    int timerCur;           // +0x28
    unsigned char pad2c[0x7c - 0x2c];
    unsigned int flags;     // +0x7c
    int bonus;              // +0x80
};

struct Zc4Gate {
    unsigned char pad00[0x3c];
    int active;             // +0x3c
};

struct Zc4EnemyMgr {
    unsigned char pad00[0x18];
    int field18;            // +0x18
};

extern int g_Zc4LzssRoot;
extern unsigned char g_Zc4LzssBuf[0x2000];
extern Zc4LzssNode g_Zc4LzssTree[0x2002];
extern Zc4Player *g_Zc4Player;
extern Zc4Gui *g_Zc4Gui;
extern Zc4CardInf *g_Zc4CardInf;
extern Zc4Gate *g_Zc4Gate;
extern Zc4EnemyMgr *g_Zc4EnemyMgr;

void Zc4InitRouteScores(Zc4RouteScore *route);
int __stdcall Zc4LzssInsertNode(int node, int *outMatchPos);
int __stdcall Zc4PlayerCheckLaserHit(const ZunVec3 *laserPos, float angle, float width, float length);

} // namespace th128
