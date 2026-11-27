#ifndef TH128_ZERO_CALL_MEDIUM9_HPP
#define TH128_ZERO_CALL_MEDIUM9_HPP

#include <stddef.h>

struct Zc9Float2 {
    float x;
    float y;
};

struct Zc9Float3 {
    float x;
    float y;
    float z;
};

struct Zc9ScoreEntry {
    int score;
    unsigned char route;
    unsigned char stage;
    char name[10];
    __int64 timestamp;
    float slowRate;
    int pad1c;
};

struct Zc9ScoreTable {
    unsigned char pad00[0x10];
    Zc9ScoreEntry entries[5][10];
};

#pragma pack(push, 4)
struct Zc9SlowRateObj {
    unsigned char pad00[0x24];
    double f24;
    double f2c;
};
#pragma pack(pop)

struct Zc9IceField {
    unsigned char pad00[0x65de4];
    unsigned int bitmap[448][12];
};

struct Zc9Enemy {
    unsigned char pad00[0x11fc];
    float posX;
    float posY;
    unsigned char pad1204[0x4fe8 - 0x1204];
    float hitW;
    float hitH;
    unsigned char pad4ff0[0x50ac - 0x4ff0];
    unsigned int flags;
};

struct Zc9EnemyNode {
    Zc9Enemy *enemy;
    Zc9EnemyNode *next;
};

struct Zc9EnemyMgr {
    unsigned char pad00[0xb8];
    Zc9EnemyNode *head;
};

struct Zc9SoundMgr {
    void *dsound;
    unsigned char pad04[0xc];
    void *primaryBuf;
    unsigned char pad14[0xc];
    int slots[12];
    unsigned char pad50[0x5454 - 0x50];
    int bgmVol;
    int bgmEnabled;
    int bgmAtten;
};

struct Zc9AnmNode {
    void *owner;
    Zc9AnmNode *next;
    Zc9AnmNode *prev;
};

struct Zc9AnmVm {
    int f00;
    Zc9AnmNode node1;
    Zc9AnmNode node2;
    unsigned char pad1c[4];
    int id;
    unsigned char pad24[0x40 - 0x24];
    float scaleX;
    float scaleY;
    float scale2X;
    float scale2Y;
    unsigned char pad50[8];
    float uvScaleX;
    float uvScaleY;
    unsigned char pad60[0x10];
    int prevTime;
    int curTime;
    float curTimeF;
    unsigned char pad7c[0xe8 - 0x7c];
    int interp0;
    unsigned char padec[0x134 - 0xec];
    int interp1;
    unsigned char pad138[0x160 - 0x138];
    int interp2;
    unsigned char pad164[0x1ac - 0x164];
    int interp3;
    unsigned char pad1b0[0x214 - 0x1b0];
    int interp4;
    unsigned char pad218[0x250 - 0x218];
    int interp5;
    unsigned char pad254[0x29c - 0x254];
    int interp6;
    unsigned char pad2a0[0x2c8 - 0x2a0];
    int interp7;
    unsigned char pad2cc[0x2f4 - 0x2cc];
    int interp8;
    unsigned char pad2f8[0x320 - 0x2f8];
    int interp9;
    unsigned char pad324[0x330 - 0x324];
    float mat[4][4];
    unsigned char pad370[0x3f0 - 0x370];
    int f3f0;
    unsigned char pad3f4[0x464 - 0x3f4];
    Zc9Float3 pos;
    unsigned char pad470[0x4b0 - 0x470];
    unsigned short flags0;
    unsigned short pad4b2;
    unsigned int flags2;
    unsigned char pad4b8[0x4e8 - 0x4b8];
};

extern int g_Zc9Difficulty;
extern int g_Zc9Score;
extern unsigned char g_Zc9Stage;
extern unsigned char g_Zc9Route;
extern Zc9SlowRateObj *g_Zc9SlowRateObj;
extern void *g_Zc9SoundThread1;
extern void *g_Zc9SoundThread2;
extern int g_Zc9SoundStopFlag;
extern signed char g_Zc9ConfigBgmVol;
extern signed char g_Zc9ConfigBgmOn;

int __stdcall Zc9InsertScoreEntry(Zc9ScoreTable *table);
void __stdcall Zc9IceBitmapClear(const Zc9Float2 *pos, Zc9IceField *field, const unsigned int *mask, int height, int width);
void __stdcall Zc9IceBitmapOr(const Zc9Float2 *pos, Zc9IceField *field, const unsigned int *mask, int height, int width);
int __stdcall Zc9MarkEnemiesInBox(const Zc9Float2 *pos, const Zc9Float2 *size, Zc9EnemyMgr *mgr);
int Zc9SoundResetAndVolume(Zc9SoundMgr *mgr);
void Zc9AnmVmReset(Zc9AnmVm *vm);

#endif
