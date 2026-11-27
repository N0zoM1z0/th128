#pragma once

#include <stddef.h>

struct EndVec2
{
    float x;
    float y;

    void Add(const EndVec2 &other);
    void FromAngleRadius(float angle, float radius);
};

struct EndVec3
{
    float x;
    float y;
    float z;
};

struct EndMeshVert
{
    EndVec3 pos;
    float rhw;
    unsigned int color;
    float u;
    float v;
};
typedef char EndMeshVertSizeCheck[(sizeof(EndMeshVert) == 0x1C) ? 1 : -1];

struct EndMesh;

struct EndVmMini
{
    int id;                                     // +0x00
    unsigned char pad04[0x14 - 0x04];
    struct EndAnmNode *children;                // +0x14
    int field18;                                // +0x18
    unsigned char pad1C[0x20 - 0x1C];
    int mode20;                                 // +0x20
    unsigned char pad24[0x464 - 0x24];
    EndVec3 pos464;                             // +0x464
    unsigned char pad470[0x4AC - 0x470];
    void *extraData;                            // +0x4AC
    unsigned int flags4B0;                      // +0x4B0
    unsigned char pad4B4[0x4CC - 0x4B4];
    EndMesh *ownerMesh;                         // +0x4CC

    int OnMeshUpdate();
    void SetMode0D();
    void SetFlag2();
    void ClearFlag2();
    void SetPos464(const EndVec3 *pos);
};

struct EndMesh
{
    int cols;                                   // +0x00
    int rows;                                   // +0x04
    int *anmIds;                                // +0x08
    EndVmMini **vms;                            // +0x0C
    EndMeshVert *verts;                         // +0x10
    EndVec3 *positions;                         // +0x14

    void SyncVerts();
    void CopyToVms();
    void HideAll();
    void ShowAll();
    void SetBlendMode(int mode);
};

struct EndListNode
{
    void *owner;                                // +0x00
    EndListNode *next;                          // +0x04
    EndListNode *prev;                          // +0x08

    void Append(EndListNode *node);
};

struct EndAnmNode
{
    EndVmMini *vm;                              // +0x00
    EndAnmNode *next;                           // +0x04
};

struct EndAnmMgrMini
{
    unsigned char pad00[0x4E9140];
    void *endingAnms[4];                        // +0x4E9140..+0x4E914F
    unsigned char pad4E9150[0x8B9704 - 0x4E9150];
    EndAnmNode *listA;                          // +0x8B9704
    unsigned int pad8B9708;
    EndAnmNode *listB;                          // +0x8B970C
};

struct EndSub
{
    unsigned char pad00[0x04];
    unsigned char timer04[0x3C];                // +0x04
    int anmIds[5];                              // +0x40..+0x53
    unsigned char pad54[0x74 - 0x54];
    unsigned int flags74;                       // +0x74
    unsigned char pad78[0xD0 - 0x78];
    void *vtblD0;                               // +0xD0

    ~EndSub();
    int GetFlag1();
    int GetFlag2();
    EndSub *DestroySelf();
};

struct EndChainMgrMini
{
    void Cut(void *elem);
};

struct EndMgr
{
    unsigned int flags00;                       // +0x00
    unsigned int pad04;                         // +0x04
    void *calcElem;                             // +0x08
    void *drawElem;                             // +0x0C
    unsigned int pad10;                         // +0x10
    void *fileBuf;                              // +0x14
    EndSub *sub;                                // +0x18
    int field1C;                                // +0x1C
    unsigned int flags20;                       // +0x20
    int frameCount;                             // +0x24

    EndMgr();
    ~EndMgr();
    static void *LoadFile(const char *name);
    static int IsFlag0();
    int IsFlag1();
    static EndMgr *Create();
    EndMgr *DestroySelf();
    static void DestroyGlobal();
    int OnCalc();
};
typedef char EndMgrSizeCheck[(sizeof(EndMgr) == 0x28) ? 1 : -1];

struct EndGet0       { int v; int Get(); };
struct EndSet0       { int v; void Set(int x); };
struct EndGet04      { unsigned int pad; int v; int Get(); };
struct EndPair       { int prev; int cur; int HasChanged(); };
struct EndClear0     { int v; void Clear(); };
struct EndOffset3368 { unsigned char pad[0x3368]; int field3368; int *Get(); };

extern EndAnmMgrMini *g_EndAnmMgr;              // 0x004D2E50
extern EndMgr *g_EndMgr;                        // 0x004B8944
extern EndChainMgrMini *g_EndChainMgr;          // 0x004D2E14
extern unsigned int g_EndSupFlags;              // 0x004D33F8
extern unsigned char g_EndCsObj[0x18];          // 0x004D3678
extern unsigned char g_EndCsDepth;              // 0x004D3798
extern int g_End4D2AD4;                         // 0x004D2AD4
extern int g_EndNextScene;                      // 0x004D33C0
extern unsigned int g_EndInputMask;             // 0x004DAFF8
extern char g_EndPathBuf[260];                  // 0x004DB878
extern void *g_EndSubVtbl;                      // 0x004A6050
extern void (__stdcall *g_EndEnterCS)(void *);  // 0x0049A088
extern void (__stdcall *g_EndLeaveCS)(void *);  // 0x0049A08C

extern void *__stdcall EndReadFile(const char *path, int a, int b); // 0x00469C40
extern int __stdcall EndMgrInit(EndMgr *mgr);   // 0x004131D0
extern int __stdcall EndSubUpdate(EndSub *sub); // 0x00413A20
extern void EndTimerTick(void *timer);          // 0x00402AC0
extern void EndSubDtor46ACF0(void *obj);        // 0x0046ACF0
extern void EndAnmRelease465F60(void *anm);     // 0x00465F60

void EndPosToUv(const EndVec2 *pos, float *u, float *v);
void EndCopyFloat(float *dst, const float *src);
void EndNoOp412FB0();
