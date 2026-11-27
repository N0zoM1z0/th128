#pragma once

#include <stddef.h>

struct DropVec3
{
    float x;
    float y;
    float z;
};

struct DropIsPos   { int v; int IsPositive(); };
struct DropSet0    { int v; void Set(int x); };
struct DropGet0    { int v; int Get(); };
struct DropArr     { int a[8]; void Set(int idx, int v); };
struct DropOff34   { unsigned char pad[0x34]; int f; int *Get(); };
struct DropOff3E3C { unsigned char pad[0x3E3C]; int f; int *Get(); };
struct DropOff5004 { unsigned char pad[0x5004]; int f; int *Get(); };
struct DropOff11C8 { unsigned char pad[0x11C8]; int f; int *Get(); };
struct DropFlag30  { unsigned char pad[0x30]; unsigned int flags; void ClearBit0(); };
struct DropObj17C  { unsigned char pad[0x17C]; int v; int Get(); };

struct DropDerefInner { unsigned int pad; int val; };
struct DropDerefOuter { unsigned int pad; DropDerefInner *sub; int Get(); };

struct DropFlag14
{
    int a;
    int b;
    int c;
    int pad0C;
    int pad10;
    unsigned int flags14;

    void Reset();
};

struct DropState
{
    int state;                                  // +0x00
    int pad04;                                  // +0x04
    int cur;                                    // +0x08
    int pad0C;                                  // +0x0C
    int saved;                                  // +0x10

    void Commit();
};

struct DropArea
{
    int mainItem;                               // +0x00
    int counts[10];                             // +0x04..+0x2B
    float radiusX;                              // +0x2C
    float radiusY;                              // +0x30

    void SetRadius(float rx, float ry);
    void Reset();
    void ClearCounts();
};
typedef char DropAreaSizeCheck[(sizeof(DropArea) == 0x34) ? 1 : -1];

struct DropEnemySub380
{
    unsigned char pad000[0x374];
    int field374;                               // +0x374
    unsigned char pad378[0x08];

    DropEnemySub380();
};
typedef char DropEnemySub380SizeCheck[(sizeof(DropEnemySub380) == 0x380) ? 1 : -1];

struct DropEnemyCore
{
    unsigned char pad000[0x27C];
    unsigned int fl27C;                         // +0x27C
    unsigned char pad280[0x290 - 0x280];
    unsigned int fl290;                         // +0x290
    unsigned char pad294[0x2E0 - 0x294];
    unsigned int fl2E0;                         // +0x2E0
    unsigned char pad2E4[0x32C - 0x2E4];
    unsigned int fl32C;                         // +0x32C
    unsigned char pad330[0x368 - 0x330];
    unsigned int fl368;                         // +0x368
    unsigned char pad36C[0x3A4 - 0x36C];
    unsigned int fl3A4;                         // +0x3A4
    unsigned char pad3A8[0x3E0 - 0x3A8];
    unsigned int fl3E0;                         // +0x3E0
    unsigned char pad3E4[0x41C - 0x3E4];
    unsigned int fl41C;                         // +0x41C
    unsigned char pad420[0x458 - 0x420];
    unsigned int fl458;                         // +0x458
    unsigned char pad45C[0x494 - 0x45C];
    unsigned int fl494;                         // +0x494
    unsigned char pad498[0x4A0 - 0x498];
    DropEnemySub380 subs[16];                   // +0x4A0..+0x3C9F
    unsigned char pad3CA0[0x3EB0 - 0x3CA0];
    unsigned int fl3EB0;                        // +0x3EB0
    unsigned char pad3EB4[0x3EC4 - 0x3EB4];
    unsigned int fl3EC4;                        // +0x3EC4
    unsigned char pad3EC8[0x3EDC - 0x3EC8];
    unsigned int fl3EDC;                        // +0x3EDC

    DropEnemyCore();
};

struct DropEclFile
{
    int Parse(void *buf);
    int Load(const char *name);
};

struct DropEnemyObj
{
    unsigned char pad000[0x11C0];
    int id;                                     // +0x11C0
};

struct DropEnemyNode
{
    DropEnemyObj *obj;                          // +0x00
    DropEnemyNode *next;                        // +0x04
};

struct DropEnemyMgr
{
    unsigned char pad00[0x5C];
    int arr5C[8];                               // +0x5C
    unsigned int flags7C;                       // +0x7C
    int arr80[8];                               // +0x80
    unsigned char padA0[0xB8 - 0xA0];
    DropEnemyNode *headB8;                      // +0xB8
    unsigned int padBC;
    unsigned int padC0;
    int nextId;                                 // +0xC4
    int lastId;                                 // +0xC8

    static void SetArr80(int idx, int val);
    static void SetArr5C(int idx, int val);
    static void __stdcall SetFlag7CBit0(int bit);
    static int HasEnemyId(int id);
    static DropEnemyObj *FindEnemyById(int id);
    static int GetLastId();
    static int GetNextId();
    static int AllocId();
};

struct Drop4B8A80
{
    unsigned char pad000[0xF78];
    int fieldF78;                               // +0xF78

    static int GetF78();
};

struct DropGuiCell
{
    float f;
    int v;
};

struct DropGuiRow
{
    DropGuiCell cols[10];                       // 0x50
    int extra;                                  // +0x50 -> stride 0x54
};
typedef char DropGuiRowSizeCheck[(sizeof(DropGuiRow) == 0x54) ? 1 : -1];

struct DropGui
{
    unsigned char pad0000[0x5918];
    DropGuiRow rows[4];                         // +0x5918
    unsigned char pad5A68[0x5A1C - 0x5A68 + 0x50]; // unused
};

struct DropGuiMini
{
    unsigned char pad0000[0x5918];
    unsigned char table5918[0x104];             // +0x5918..+0x5A1B
    DropObj17C *activeObj;                      // +0x5A1C
    int pad5A20;                                // +0x5A20
    int field5A24;                              // +0x5A24
    int field5A28;                              // +0x5A28

    static void SetEntry(int row, int col, float f, int v);
    int HasActiveObj();
    static int GetActiveObjField17C();
    static void SetTimerPair(int a, int b);
};

struct Drop4B8934
{
    unsigned char pad00[0x7C];
    unsigned int flags7C;                       // +0x7C

    static int IsFlag0And6();
    static void SetFlag4();
    static int GetFlag4();
};

struct DropObj50
{
    unsigned char pad00[0x50];
    DropVec3 pos50;                             // +0x50
    unsigned char pad5C[0x68 - 0x5C];
    float f68;                                  // +0x68
    float pad6C;
    float f70;                                  // +0x70
    float f74;                                  // +0x74
    unsigned char pad78[0x80 - 0x78];
    int field80;                                // +0x80

    void SetVec3(const DropVec3 *v);
    void SetF68(float v);
    void SetF70(float v);
    void SetF74(float v);
    void Clear80();
};

struct DropZero354A { unsigned char data[0x354]; DropZero354A(); };
struct DropZero354B { unsigned char data[0x354]; DropZero354B(); };
struct DropZero354C { unsigned char data[0x354]; DropZero354C(); };

struct DropZero374
{
    unsigned char pad00[0x2C];
    float f2C;                                  // +0x2C
    unsigned char pad30[0x374 - 0x30];

    DropZero374();
};
typedef char DropZero374SizeCheck[(sizeof(DropZero374) == 0x374) ? 1 : -1];

struct DropObj5CC
{
    unsigned char pad000[0x5CC];
    DropVec3 pos5CC;                            // +0x5CC
    float pad5D8;
    float f5DC;                                 // +0x5DC

    void SetVec3(const DropVec3 *v);
    void SetF5DC(float v);
};

extern char g_DropPathBuf[260];                 // 0x004DB878
extern DropEnemyMgr *g_DropEnemyMgr;            // 0x004B8948
extern Drop4B8A80 *g_Drop4B8A80;                // 0x004B8A80
extern DropGuiMini *g_DropGui;                  // 0x004B8950
extern Drop4B8934 *g_Drop4B8934;                // 0x004B8934

extern void *__stdcall DropReadFile(const char *path, int a, int b); // 0x00469C40
