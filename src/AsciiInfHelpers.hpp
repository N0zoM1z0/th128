#pragma once

struct GameManagerMini
{
    unsigned int pad00;
    int field04;
};

extern GameManagerMini *g_GameManagerMini;      // 0x004B8A7C

struct AsciiListNode
{
    void *owner;                                // +0x00
    AsciiListNode *next;                        // +0x04
    AsciiListNode *prev;                        // +0x08

    void Init(void *o);
};

struct AsciiMatrix4x4
{
    float m[4][4];

    void SetIdentity();
};

struct AsciiSpriteMini
{
    unsigned char pad00[0x24];
    float u0;                                   // +0x24
    float v0;                                   // +0x28
    float u1;                                   // +0x2C
    float v1;                                   // +0x30
    unsigned char pad34[0x48 - 0x34];
};

struct AsciiAnmResMini
{
    unsigned char pad00[0x118];
    AsciiSpriteMini *sprites;                   // +0x118

    AsciiSpriteMini *GetSprite(int index);
};

struct AsciiAnmVmMini
{
    unsigned int unk00;                         // +0x000
    AsciiListNode node04;                       // +0x004
    AsciiListNode node10;                       // +0x010
    unsigned char pad1C[4];                     // +0x01C
    int saved20;                                // +0x020
    unsigned char pad24[0x40 - 0x24];
    float scaleX;                               // +0x040
    float scaleY;                               // +0x044
    float scaleZ;                               // +0x048
    float scaleW;                               // +0x04C
    unsigned char pad50[8];
    float uvScaleX;                             // +0x058
    float uvScaleY;                             // +0x05C
    unsigned char pad60[0x10];
    int timePrev;                               // +0x070 (-999999)
    int timeCur;                                // +0x074
    float timeSub;                              // +0x078
    unsigned char pad7C[8];
    float uv0x;                                 // +0x084
    float uv0y;                                 // +0x088
    float uv1x;                                 // +0x08C
    float uv1y;                                 // +0x090
    float uv2x;                                 // +0x094
    float uv2y;                                 // +0x098
    float uv3x;                                 // +0x09C
    float uv3y;                                 // +0x0A0
    unsigned char padA4[0xE8 - 0xA4];
    int interp0;                                // +0x0E8
    unsigned char padEC[0x134 - 0xEC];
    int interp1;                                // +0x134
    unsigned char pad138[0x160 - 0x138];
    int interp2;                                // +0x160
    unsigned char pad164[0x1AC - 0x164];
    int interp3;                                // +0x1AC
    unsigned char pad1B0[0x214 - 0x1B0];
    int interp4;                                // +0x214
    unsigned char pad218[0x250 - 0x218];
    int interp5;                                // +0x250
    unsigned char pad254[0x29C - 0x254];
    int interp6;                                // +0x29C
    unsigned char pad2A0[0x2C8 - 0x2A0];
    int interp7;                                // +0x2C8
    unsigned char pad2CC[0x2F4 - 0x2CC];
    int interp8;                                // +0x2F4
    unsigned char pad2F8[0x320 - 0x2F8];
    int interp9;                                // +0x320
    unsigned char pad324[0x330 - 0x324];
    AsciiMatrix4x4 matrix;                      // +0x330 (size 0x40)
    unsigned char pad370[0x3F0 - 0x370];
    union
    {
        int field3F0;
        struct
        {
            unsigned char b3F0;
            unsigned char b3F1;
            unsigned char b3F2;
            unsigned char b3F3;
        };
    };                                          // +0x3F0
    unsigned char pad3F4[0x428 - 0x3F4];
    AsciiSpriteMini *sprite;                    // +0x428
    unsigned char pad42C[0x464 - 0x42C];
    AsciiListNode node464;                      // +0x464
    unsigned char pad470[0x4B0 - 0x470];
    unsigned short flags4B0;                    // +0x4B0
    unsigned short pad4B2;                      // +0x4B2
    unsigned int flags4B4;                      // +0x4B4
    unsigned char pad4B8[0x4E8 - 0x4B8];

    void SetByte3F3(unsigned char v);
    void SetInt3F0(int v);
    AsciiSpriteMini *GetSprite();
    void BindSprite(AsciiSpriteMini *spr);
    void Reset();
};

struct AsciiEntry
{
    unsigned char pad00[0x100];
    int id;                                     // +0x100
    unsigned char pad104[0x160 - 0x104];
    unsigned int flags;                         // +0x160

    AsciiEntry *ClearFlag0();
};

struct AsciiZero44A { unsigned char pad[0x44]; int v; void Zero(); };
struct AsciiZero34  { unsigned char pad[0x34]; int v; void Zero(); };
struct AsciiZero44B { unsigned char pad[0x44]; int v; void Zero(); };
struct AsciiZero24A { unsigned char pad[0x24]; int v; void Zero(); };
struct AsciiZero24B { unsigned char pad[0x24]; int v; void Zero(); };

class AsciiInf
{
public:
    unsigned char pad000[0x9E4];
    AsciiEntry entries[321];                    // +0x9E4..+0x1C847 (321 * 0x164 = 0x1BE64)
    int entryCount;                             // +0x1C848
    unsigned int color;                         // +0x1C84C
    unsigned int color2;                        // +0x1C850
    int unk1C854;                               // +0x1C854
    float scaleX;                               // +0x1C858
    float scaleY;                               // +0x1C85C
    int shadow;                                 // +0x1C860
    int align;                                  // +0x1C864
    int fontId;                                 // +0x1C868
    int layer;                                  // +0x1C86C
    int blendMode;                              // +0x1C870
    int unk1C874;                               // +0x1C874
    int unk1C878;                               // +0x1C878
    int unk1C87C;                               // +0x1C87C
    int unk1C880;                               // +0x1C880
    unsigned char pad1C884[8];
    int unk1C88C;                               // +0x1C88C
    int frameCount;                             // +0x1C890
    unsigned char pad1C894[0x1C8A8 - 0x1C894];

    static AsciiInf *Create();
    static AsciiInf *Destroy(AsciiInf *p);
    static void DestroyGlobal();
    static unsigned int GetStructSize();

    AsciiInf();
    ~AsciiInf();
    int Init();
    int OnTickInner();
    int OnTick();
    int OnDraw0();
    int OnDraw2();
    void UpdatePopups();
    int DrawLayer0();
    void DrawLayer(int layerIndex);

    AsciiEntry *FindEntryByIdPtr(const int *idPtr);
    AsciiEntry *FindEntryById(int id);

    void NoOp1();
    void SetColor(unsigned int c);
    void ResetAlign();
    void SetScale(float sx, float sy);
    void SetFontId(int id);
    void ResetShadow();
    void SetLayer(int l);
    void SetBlendMode(int mode);
    void ResetFlags();
    void ResetColors();
    void ResetStyle();
    void NoOp2();
    void NoOp3();
};

extern AsciiInf *g_AsciiInf;                    // 0x004B8920

int GetGameManagerField4();
bool AsciiIdEquals(const int *a, const int *b);
void AsciiSetIntCell(int val, int *cell);
