#ifndef TH128_ZERO_CALL_MEDIUM13_HPP
#define TH128_ZERO_CALL_MEDIUM13_HPP

struct Zc13GuiCounter {
    unsigned char pad00[0xa8];
    int encoded;
};

struct Zc13RankObj {
    unsigned char pad00[0xa0];
    float val;
};

struct Zc13Rng {
    unsigned short state;
    unsigned short pad02;
    unsigned int count;
};

struct Zc13IceNode {
    int f00;
    Zc13IceNode *next;
    int f08;
    int f0c;
    float x;
    float y;
    float z;
    float radius;
    int f20;
    int f24;
    unsigned char pad28[0x38 - 0x28];
    int f38;
};

struct Zc13IceCell {
    Zc13IceNode *head;
    int pad04[2];
};

struct Zc13IceGrid {
    unsigned char pad00[0x65a20];
    Zc13IceCell cells[8][10];
};

struct Zc13Float2 {
    float x;
    float y;
};

struct Zc13Timer {
    unsigned int flags;
    Zc13Timer() { flags &= ~1u; }
};

struct Zc13BossSlot {
    unsigned char pad00[0x374];
    int f374;
    unsigned char pad378[8];

    Zc13BossSlot();
};

struct Zc13BossStruct {
    unsigned char pad00[0x27c];
    Zc13Timer t0;
    unsigned char pad280[0x290 - 0x280];
    Zc13Timer t1;
    unsigned char pad294[0x2e0 - 0x294];
    Zc13Timer t2;
    unsigned char pad2e4[0x32c - 0x2e4];
    Zc13Timer t3;
    unsigned char pad330[0x368 - 0x330];
    Zc13Timer t4;
    unsigned char pad36c[0x3a4 - 0x36c];
    Zc13Timer t5;
    unsigned char pad3a8[0x3e0 - 0x3a8];
    Zc13Timer t6;
    unsigned char pad3e4[0x41c - 0x3e4];
    Zc13Timer t7;
    unsigned char pad420[0x458 - 0x420];
    Zc13Timer t8;
    unsigned char pad45c[0x494 - 0x45c];
    Zc13Timer t9;
    unsigned char pad498[0x4a0 - 0x498];
    Zc13BossSlot slots[16];
    unsigned char pad3ca0[0x3eb0 - (0x4a0 + 16 * 0x380)];
    Zc13Timer t10;
    unsigned char pad3eb4[0x3ec4 - 0x3eb4];
    Zc13Timer t11;
    unsigned char pad3ec8[0x3edc - 0x3ec8];
    Zc13Timer t12;
    unsigned char pad3ee0[0x20];

    Zc13BossStruct();
};

struct Zc13LzssNode {
    int parent;
    int left;
    int right;
};

struct Zc13InterpObj {
    unsigned char pad00[0x13c];
    int f13c;
    int f140;
    int f144;
    int f148;
    int timerPrev;
    int timerCur;
    float timerCurF;
    float *timerScale;
    unsigned int timerFlags;
    int f160;
    int f164;
};

struct Zc13StageInfo {
    unsigned char pad00[0x148];
    int defVal;
    unsigned char pad14c[0x16c - 0x14c];
    int arr0[36];
    int arr1[36];
    int arr2[36];
    int arr3[36];
    int f388;

    int GetField(int idx, int mode);
};

extern Zc13GuiCounter *g_Zc13Gui;
extern int g_Zc13Diff;
extern int g_Zc13RankThresholds[2][15];
extern unsigned int g_Zc13CritFlags;
extern unsigned char g_Zc13CritSec[0x3a];
extern unsigned char g_Zc13CritDepth;
extern Zc13LzssNode g_Zc13LzssTree[4096];
extern float g_Zc13TimeScale;

void __stdcall Zc13DecodeScoreCounter(int *outA, int *outB);
int Zc13GetRankIndex(const Zc13RankObj *obj);
int __stdcall Zc13AabbOverlap(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh);
unsigned int Zc13RngNextU32(Zc13Rng *rng);
Zc13IceNode *__stdcall Zc13FindCollidingIce(unsigned int cx, unsigned int cy, Zc13IceGrid *grid, const Zc13Float2 *pos, float radius);
Zc13BossStruct *Zc13ResetBossStruct(Zc13BossStruct *boss);
void Zc13LzssReplaceNode(int oldIdx, int newIdx);
void __stdcall Zc13InitInterpTimer(Zc13InterpObj *obj, int f160, unsigned char f164, unsigned char f13c, unsigned char f140);

#endif
