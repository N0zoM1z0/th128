#pragma once

#include <stddef.h>

struct PrVec3
{
    float x;
    float y;
    float z;

    float DistSq2DA(const PrVec3 *other);
    float DistSq2DB(const PrVec3 *other);
};

struct PrIVec2
{
    int x;
    int y;

    void ToVec3Scaled(PrVec3 *out);
    PrIVec2 *AddAssign(PrIVec2 *out, const PrIVec2 *rhs);
    int IsNonZero();
    void SubTo(PrIVec2 *out, const PrIVec2 *rhs);
    void MulTo(PrIVec2 *out, int scale);
    void Div100To(PrIVec2 *out);
    void Set(int a, int b);
    void AddTo(PrIVec2 *out, const PrIVec2 *rhs);
    void CopyTo(PrIVec2 *out, const PrIVec2 *rhs);
};

struct PrTimer
{
    int previous;
    int current;
    int IsTickMultiple(int mod);
};

struct PrWord418
{
    unsigned char pad000[0x418];
    short field418;
    int Get418();
};

struct PrObj38 { unsigned char pad[0x38]; int field38; int Get38(); };
struct PrObj74 { unsigned char pad[0x74]; int field74; int Get74(); };

struct PrPlayerOpt
{
    unsigned char pad00[0x4C];
    float field4C;                              // +0x4C (0x8810 + 0x4C = 0x885C)
    int pad50;
    int field54;                                // +0x54 (0x8810 + 0x54 = 0x8864)
    unsigned char pad58[0x78 - 0x58];
    int flag78;                                 // +0x78 (0x8810 + 0x78 = 0x8888)
    unsigned char pad7C[0xE4 - 0x7C];
};
typedef char PrPlayerOptSizeCheck[(sizeof(PrPlayerOpt) == 0xE4) ? 1 : -1];

struct PrPlayerBullet
{
    unsigned char pad00[0x0C];
    int ownerId;                                // +0x0C -> wait: 0xFFC is offset of ownerId?
    unsigned char pad10[0x78 - 0x10];
};
typedef char PrPlayerBulletSizeCheck[(sizeof(PrPlayerBullet) == 0x78) ? 1 : -1];

struct PrPlayer
{
    unsigned char pad0000[0xECC];
    PrVec3 posECC;                              // +0xECC
    PrIVec2 fixedPosED8;                        // +0xED8
    unsigned char padEE0[0xFFC - 0xEE0];
    int bulletsFFC[256][30];                    // +0xFFC (stride 0x78 = 30 ints)
    unsigned char pad87FC[0x8810 - 0x87FC];
    PrPlayerOpt opts8810[8];                    // +0x8810..+0x8F2F
    unsigned char pad8F30[0x8ED4 - 0x8810 - 8 * 0xE4 + 0x100]; // wait: 0x8810 + 8*0xE4 = 0x8F30 > 0x8ED4!
};

struct PrPlayerExact
{
    unsigned char pad0000[0xECC];
    PrVec3 posECC;                              // +0xECC
    PrIVec2 fixedPosED8;                        // +0xED8
    unsigned char padEE0[0xFFC - 0xEE0];
    unsigned char bulletsFFC[256 * 0x78];       // +0xFFC (stride 0x78, ends at 0x87FC)
    unsigned char pad87FC[0x8810 - 0x87FC];
    PrPlayerOpt opts8810[7];                    // +0x8810..+0x8E4B
    unsigned char pad8E4C[0x8EC4 - 0x8E4C];
    int opt7Flag8EC4;                           // +0x8EC4
    unsigned char pad8EC8[0x8ED4 - 0x8EC8];
    int targetId8ED4;                           // +0x8ED4
    unsigned char targetByte8ED8;               // +0x8ED8

    static PrPlayerOpt *GetOptPtr(int idx);
    static float GetOptFloat4C(int idx);
    void ReadOpt54(int idx, int *out);
    void ClearTargetId(int id);
    static void SetFixedPos(const PrIVec2 *p);
};

struct PrInputFlags
{
    unsigned char pad0000[0x11C0];
    int field11C0;                              // +0x11C0
    unsigned char pad11C4[0x3EE4 - 0x11C4];
    unsigned int flags3EE4;                     // +0x3EE4
    unsigned char pad3EE8[0x50AC - 0x3EE8];
    unsigned int flags50AC;                     // +0x50AC

    int IsFlag3EE4Set();
    int IsFlag50ACSet();
    int Get11C0();
};

struct PrEnemyMgr
{
    unsigned char pad00000[0x6B218];
    int field6B218;                             // +0x6B218

    static int Get6B218();
};

struct PrBulletHit
{
    unsigned char pad00[0x18];
    float field18;                              // +0x18
    unsigned char pad1C[0x54 - 0x1C];
    int field54;                                // +0x54
    unsigned char pad58[0x64 - 0x58];
    int field64;                                // +0x64

    void Zero54And64A();
    void Zero54And64B();
    void AddField18(float v);
};

#pragma pack(push, 4)
struct PrFpsCounter
{
    unsigned char pad00[0x24];
    double d24;                                 // +0x24
    double d2C;                                 // +0x2C
    float field34;                              // +0x34

    static float Get34();
};
#pragma pack(pop)

struct PrGameThread
{
    unsigned char pad00[0x60];
    unsigned int flags60;                       // +0x60
};

struct PrReplayData
{
    unsigned char pad00[0x0C];
    int field0C;                                // +0x0C
    int pad10;
    int score14;                                // +0x14
    unsigned char pad18[0x54 - 0x18];
    float slowdown54;                           // +0x54
    int pad58;
    int stage5C;                                // +0x5C
    unsigned char pad60[0x68 - 0x60];
    int clearStage68;                           // +0x68
};

struct PrReplayMgr
{
    unsigned char pad000[0x10];
    int mode10;                                 // +0x10
    unsigned char pad014[0x1C - 0x14];
    PrReplayData *data1C;                       // +0x1C
    unsigned char pad020[0x3A4 - 0x20];
    int frame3A4;                               // +0x3A4
    unsigned char pad3A8[0x3BC - 0x3A8];
    unsigned int tag3BC;                        // +0x3BC

    int GetFastForwardStepA();
    int GetFastForwardStepB();
    char *GetTagStr();
};

struct PrReplayChunkNode
{
    void *owner;                                // +0x00
    PrReplayChunkNode *next;                    // +0x04
    PrReplayChunkNode *prev;                    // +0x08
};

struct PrReplayInput
{
    unsigned short a;
    unsigned short b;
    unsigned short c;
};
typedef char PrReplayInputSizeCheck[(sizeof(PrReplayInput) == 6) ? 1 : -1];

struct PrReplayChunk
{
    PrReplayInput inputs[900];                  // +0x0000..+0x1517 (900 * 6 = 0x1518)
    PrReplayInput *inputPtr1518;                // +0x1518
    unsigned char fpsBytes151C[900];            // +0x151C..+0x189F
    unsigned char *fpsPtr18A0;                  // +0x18A0
    PrReplayChunkNode node18A4;                 // +0x18A4..+0x18AF

    int GetFpsCount();
    int GetInputByteCount();
    int GetInputCount();
    int GetTotalByteCount();
    int PushInput(unsigned short a, unsigned short b, unsigned short c);
    int PushFps(unsigned char v);
    PrReplayChunk();
    void UnlinkNode();
    PrReplayChunk *DeleteSelf();
};
typedef char PrReplayChunkSizeCheck[(sizeof(PrReplayChunk) == 0x18B0) ? 1 : -1];

struct PrReplayFileHeader
{
    unsigned int magic00;                       // +0x00
    unsigned short ver04;                       // +0x04
    unsigned short pad06;
    int f08;                                    // +0x08
    int f0C;                                    // +0x0C
    int f10;                                    // +0x10
    int f14;                                    // +0x14
    int f18;                                    // +0x18
    int f1C;                                    // +0x1C
    int f20;                                    // +0x20

    void InitHeader();
};

struct PrReplayList
{
    unsigned char pad00[0x64];
    PrReplayChunkNode heads64[16];              // +0x64 (stride 0x0C)
    unsigned char pad124[0x134 - 0x124];
    int chunkCount134;                          // +0x134

    void AllocAndAppendChunk(int stageIdx);
    void FreeStageChunks(int stageIdx);
};

struct PrStageEntry24
{
    int f00;
    int f04;
    int f08;
    int f0C;
    int f10;
    int f14;
    PrReplayChunkNode node18;                   // +0x18..+0x23

    PrStageEntry24();
    ~PrStageEntry24();
    void ResetZero();
    void ResetNeg1();
};
typedef char PrStageEntry24SizeCheck[(sizeof(PrStageEntry24) == 0x24) ? 1 : -1];

struct PrReplayState84
{
    unsigned char pad00[0x1C];
    int field1C;                                // +0x1C
    unsigned char pad20[0x38 - 0x20];
    int field38;                                // +0x38
    unsigned char pad3C[0x80 - 0x3C];
    int field80;                                // +0x80
    int field84;                                // +0x84

    void Set1C(int v);
    void Set38(int v);
    void Set84(int v);
    int Get80();
    int Get1C();
};

struct PrBuf90 { unsigned char raw[0x90]; PrBuf90(); };
struct PrUserTag { unsigned int tag; void SetUserMagic(); };

struct PrScoreRecord
{
    unsigned char pad00[0x94];
    int idx94;
    int val98;
};

struct PrScoreFile
{
    unsigned int magic00;                       // +0x00
    unsigned int pad04;                         // +0x04
    unsigned int size08;                        // +0x08
    struct Entry {
        unsigned char pad[0x90];
        int id;                                 // +0x90 (+0x9C for [0])
        int val;                                // +0x94 (+0xA0 for [0])
    } entries[250];                             // +0x0C..+0x947B

    void InitScoreFile();
};
typedef char PrScoreFileSizeCheck[(sizeof(PrScoreFile) == 0x947C) ? 1 : -1];

extern int g_Pr4B4D40;                          // 0x004B4D40
extern int g_Pr4B4CC8;                          // 0x004B4CC8
extern int g_Pr4B4D38;                          // 0x004B4D38
extern int g_Pr4B4D34;                          // 0x004B4D34
extern int g_Pr4B4CF4;                          // 0x004B4CF4
extern PrPlayerExact *g_PrPlayer;               // 0x004B8A80
extern PrEnemyMgr *g_PrEnemyMgr;                // 0x004B8938
extern PrFpsCounter *g_PrFpsCounter;            // 0x004B894C
extern PrGameThread *g_PrGameThread;            // 0x004B8954
extern unsigned int g_Pr4DAFF8;                 // 0x004DAFF8
extern char g_PrTagBuf[8];                      // 0x004DB870
extern int g_Pr4D33CC;                          // 0x004D33CC
extern int g_Pr4D2AE4;                          // 0x004D2AE4
extern unsigned short g_Pr4D2AE0;               // 0x004D2AE0
extern signed char g_PrScoreTable[250];         // 0x004B23D0

void PrIncGrazeClamped();
float __stdcall PrAngleDiffA(float a, float b);
float __stdcall PrAngleDiffB(float a, float b);
float __stdcall PrFabsFloat(float v);
void PrVec2FloatToFixed128(const PrVec3 *in, PrIVec2 *out);
int PrGet4B4CC8Div4B4D38();
void PrSet4B4D38(int v);
void PrSet4B4D34(int v);
int PrGet4B4CF4();
void PrComputeBoxBounds(const PrVec3 *size, const PrVec3 *center, PrVec3 *outMin, PrVec3 *outMax);
int PrRet0_4404A0();
int PrGet4D33CC();
void PrZero4D2AE4();
unsigned short PrGet4D2AE0();
unsigned int __stdcall PrChecksumBytes(const unsigned char *buf, int totalSize);
