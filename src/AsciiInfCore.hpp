#pragma once

#include <stddef.h>

struct AsciiVec3
{
    float x;
    float y;
    float z;
};

struct AsciiEntryId
{
    int id;
};

struct AsciiTimerMini
{
    int previous;                               // +0x00
    int current;                                // +0x04
    float currentF;                             // +0x08
    float *gameSpeed;                           // +0x0C
    unsigned int flags;                         // +0x10

    void Reset();
};

struct AsciiPopupEntry
{
    char text[0x100];                           // +0x000
    int id;                                     // +0x100
    AsciiVec3 pos;                              // +0x104
    unsigned int color;                         // +0x110
    unsigned int color1;                        // +0x114
    unsigned int color2;                        // +0x118
    float scaleX;                               // +0x11C
    float scaleY;                               // +0x120
    int unk124;                                 // +0x124
    int shadow;                                 // +0x128
    int layer;                                  // +0x12C
    int fontId;                                 // +0x130
    int blendMode;                              // +0x134
    int duration;                               // +0x138 (decremented until < 0, cap 1000000)
    int unk13C;                                 // +0x13C
    int unk140;                                 // +0x140
    int blinkPeriod;                            // +0x144
    int unk148;                                 // +0x148
    int pauseAffected;                          // +0x14C
    AsciiTimerMini timer;                       // +0x150..+0x163
};
typedef char AsciiPopupEntrySizeCheck[(sizeof(AsciiPopupEntry) == 0x164) ? 1 : -1];

struct AsciiD3DDeviceVtbl
{
    void *pad[0xBC / 4];
    int (__stdcall *SetViewport)(void *self, const void *vp);
};

struct AsciiD3DDevice
{
    AsciiD3DDeviceVtbl *vtbl;
};

struct AsciiViewportSlot
{
    unsigned char pad00[0xCC];
    unsigned int d3dViewport[6];
    unsigned char padE4[0x118 - 0xE4];
};

struct AsciiSupervisorMini
{
    unsigned char pad00[0x08];
    AsciiD3DDevice *device;                     // +0x08
    unsigned char pad0C[0x204 - 0x0C];
    AsciiViewportSlot viewports[3];             // +0x204 (+0x204 = 0x4D306C, +0x31C = 0x4D3184)
    AsciiViewportSlot *currentViewport;         // +0x54C (0x4D33B4)
    int currentViewportIndex;                   // +0x550 (0x4D33B8)

    void SetViewport(int index);
    void ApplyCurrentViewport();                // 0x004351F0 (external)
};

struct AsciiAnmNodeMini
{
    struct AsciiAnmVmCore *vm;                  // +0x00
    AsciiAnmNodeMini *next;                     // +0x04
};

struct AsciiAnmVmCore
{
    int id;                                     // +0x000
    unsigned char pad004[0x14 - 0x04];
    AsciiAnmNodeMini *childHead;                // +0x014
    void *parentPtr;                            // +0x018
    unsigned char pad01C[0x4AC - 0x1C];
    void *extraData;                            // +0x4AC
    unsigned int flags4B0;                      // +0x4B0

    void FreeExtraInternal();
    void FreeExtraThiscall();
};

struct AsciiAnmManagerMini
{
    unsigned char pad00[0x8B9704];
    AsciiAnmNodeMini *listA;                    // +0x8B9704
    unsigned int pad8B9708;
    AsciiAnmNodeMini *listB;                    // +0x8B970C

    void FlushSprites();                        // 0x0045EF60 (external)
};

struct AsciiGameManagerCore
{
    unsigned int pad00;
    int paused;                                 // +0x04
};

struct StageBgMini
{
    int count;                                  // +0x00
    int unk04;                                  // +0x04
    int *anmIds;                                // +0x08
    void *buf0C;                                // +0x0C
    void *buf10;                                // +0x10
    void *buf14;                                // +0x14

    void Cleanup();
    static StageBgMini *Destroy(StageBgMini *self);
};

struct AsciiFlag40A { unsigned char pad[0x40]; unsigned int flags; void Clear(); };
struct AsciiFlag20A { unsigned char pad[0x20]; unsigned int flags; void Clear(); };
struct AsciiFlag30  { unsigned char pad[0x30]; unsigned int flags; void Clear(); };
struct AsciiFlag40B { unsigned char pad[0x40]; unsigned int flags; void Clear(); };
struct AsciiFlag20B { unsigned char pad[0x20]; unsigned int flags; void Clear(); };
struct AsciiFlag20R { unsigned char pad[0x20]; unsigned int flags; AsciiFlag20R *Clear(); };
struct AsciiRet0    { AsciiRet0 *Self(); };
struct AsciiRet1    { AsciiRet1 *Self(); };
struct StageFlag7C  { unsigned char pad[0x7C]; unsigned int flags; unsigned int GetBit0(); };
struct StageFlag80  { unsigned char pad[0x80]; unsigned int flags; void ClearBit0(); };

class AsciiInfCore
{
public:
    unsigned char pad000[0x9E4];
    AsciiPopupEntry entries[321];               // +0x9E4..+0x1C847
    int entryCount;                             // +0x1C848
    unsigned int color;                         // +0x1C84C
    unsigned int color2;                        // +0x1C850
    int blinkPeriod;                            // +0x1C854
    float scaleX;                               // +0x1C858
    float scaleY;                               // +0x1C85C
    int shadow;                                 // +0x1C860
    int align;                                  // +0x1C864
    int fontId;                                 // +0x1C868
    int layer;                                  // +0x1C86C
    int blendMode;                              // +0x1C870
    int duration;                               // +0x1C874
    int unk1C878;                               // +0x1C878
    int unk1C87C;                               // +0x1C87C
    int unk1C880;                               // +0x1C880
    int pauseAffected;                          // +0x1C884
    int unk1C888;                               // +0x1C888
    int unk1C88C;                               // +0x1C88C
    int frameCount;                             // +0x1C890
    unsigned char pad1C894[0x1C8A8 - 0x1C894];

    AsciiEntryId *AddString(const char *str, const AsciiVec3 *pos, AsciiEntryId *outId); // 0x004015F0 (external)
    void DrawEntry(AsciiPopupEntry *entry);                                               // 0x00401AD0 (external)

    AsciiEntryId *AddFormattedInt(int val, const AsciiVec3 *pos, AsciiEntryId *outId);
    AsciiEntryId *AddFormattedStr(AsciiEntryId *outId, const AsciiVec3 *pos, const char *fmt, ...);
    int DrawLayer(int targetLayer);
    int DrawLayer1();
    int DrawLayer2();
    int OnDraw0();
    int OnDraw2();
    void DrawFormattedNow(const AsciiVec3 *pos, const char *fmt, ...);
    void DrawIntNow(int val, const AsciiVec3 *pos);
    void UpdatePopups();
    void SetUnk1C878_7C(int a, int b);
    void ResetUnk1C88C();
};

extern AsciiInfCore *g_AsciiInfCore;                // 0x004B8920
extern AsciiGameManagerCore *g_GameManagerCore;     // 0x004B8A7C
extern AsciiAnmManagerMini *g_AnmManagerMini;       // 0x004D2E50
extern AsciiSupervisorMini g_SupervisorMini;        // 0x004D2E68
extern float g_DefaultTimeScaleCore;                // 0x004B6F58

float __stdcall AsciiMeasureStringWidth(const char *str, int fontId, float charWidth);
void AsciiNoOp403050();
void AsciiFreePtr(void *p);
void AsciiNoOp4030E0();
void AsciiNoOp403110();
void AsciiNoOp403150();
void AsciiNoOp403190();
