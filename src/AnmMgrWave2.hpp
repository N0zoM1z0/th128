#ifndef TH128_ANM_MGR_WAVE2_HPP
#define TH128_ANM_MGR_WAVE2_HPP

#include <windows.h>

namespace th128 {

struct Amw2Vec3 {
    float x;
    float y;
    float z;

    Amw2Vec3() {}
    Amw2Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct Amw2VmId {
    int value;

    Amw2VmId() : value(0) {}
    Amw2VmId(int v) : value(v) {}
};

struct Amw2D3dViewport {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    float minZ;
    float maxZ;
};

struct Amw2SpriteViewport {
    unsigned char pad00[0xcc];
    Amw2D3dViewport viewport;

    void Set2DView();
};

struct Amw2D3dDevice {
    virtual long __stdcall QueryInterface(const void *, void **) = 0;
    virtual unsigned long __stdcall AddRef() = 0;
    virtual unsigned long __stdcall Release() = 0;
    virtual void __stdcall M03() = 0;
    virtual void __stdcall M04() = 0;
    virtual void __stdcall M05() = 0;
    virtual void __stdcall M06() = 0;
    virtual void __stdcall M07() = 0;
    virtual void __stdcall M08() = 0;
    virtual void __stdcall M09() = 0;
    virtual void __stdcall M10() = 0;
    virtual void __stdcall M11() = 0;
    virtual void __stdcall M12() = 0;
    virtual void __stdcall M13() = 0;
    virtual void __stdcall M14() = 0;
    virtual void __stdcall M15() = 0;
    virtual void __stdcall M16() = 0;
    virtual void __stdcall M17() = 0;
    virtual void __stdcall M18() = 0;
    virtual void __stdcall M19() = 0;
    virtual void __stdcall M20() = 0;
    virtual void __stdcall M21() = 0;
    virtual void __stdcall M22() = 0;
    virtual void __stdcall M23() = 0;
    virtual void __stdcall M24() = 0;
    virtual void __stdcall M25() = 0;
    virtual void __stdcall M26() = 0;
    virtual void __stdcall M27() = 0;
    virtual void __stdcall M28() = 0;
    virtual void __stdcall M29() = 0;
    virtual void __stdcall M30() = 0;
    virtual void __stdcall M31() = 0;
    virtual void __stdcall M32() = 0;
    virtual void __stdcall M33() = 0;
    virtual void __stdcall M34() = 0;
    virtual void __stdcall M35() = 0;
    virtual void __stdcall M36() = 0;
    virtual void __stdcall M37() = 0;
    virtual void __stdcall M38() = 0;
    virtual void __stdcall M39() = 0;
    virtual void __stdcall M40() = 0;
    virtual void __stdcall M41() = 0;
    virtual void __stdcall M42() = 0;
    virtual void __stdcall M43() = 0;
    virtual void __stdcall M44() = 0;
    virtual void __stdcall M45() = 0;
    virtual void __stdcall M46() = 0;
    virtual long __stdcall SetViewport(const Amw2D3dViewport *vp) = 0;
    virtual void __stdcall M48() = 0;
    virtual void __stdcall M49() = 0;
    virtual void __stdcall M50() = 0;
    virtual void __stdcall M51() = 0;
    virtual void __stdcall M52() = 0;
    virtual void __stdcall M53() = 0;
    virtual void __stdcall M54() = 0;
    virtual void __stdcall M55() = 0;
    virtual void __stdcall M56() = 0;
    virtual long __stdcall SetRenderState(unsigned int state, unsigned int value) = 0;
};

struct Amw2AnmVm;

struct Amw2ListNode {
    Amw2AnmVm *entry;
    Amw2ListNode *next;
    Amw2ListNode *prev;
};

struct Amw2AnmVm {
    Amw2VmId id;
    Amw2ListNode worldNode;
    Amw2ListNode childNode;
    Amw2AnmVm *nextDraw;
    int layer;
    unsigned char pad24[0x18];
    Amw2AnmVm *parent;
    unsigned char pad40[0x3b8];
    short pendingInterrupt;
    unsigned char pad3fa[0x6a];
    Amw2Vec3 pos;
    unsigned char pad470[0x40];
    unsigned int flags;
    unsigned int flags2;
    unsigned char pad4b8[0x04];
    int (__fastcall *onTick)(Amw2AnmVm *vm);
    void (__fastcall *onDraw)(Amw2AnmVm *vm);
    unsigned char pad4c4[0x04];
    void (__fastcall *onInterrupt)(Amw2AnmVm *vm, int interrupt);
    unsigned char pad4cc[0x1c];

    int Update();
    void Draw(struct Amw2AnmManager *mgr);
    Amw2VmId RegisterWorldVmAtTail();
    Amw2VmId RegisterWorldVmAtHead();
    Amw2VmId RegisterUiVmAtTail();
    Amw2VmId RegisterUiVmAtHead();
};

struct Amw2AnmResource {
    unsigned char pad00[0x130];
    int refCount;

    void BindVmScript(Amw2AnmVm *vm, int scriptId);
    void BindVmAtPosition(Amw2AnmVm *vm, int scriptId);
    Amw2VmId CreateVm(const Amw2Vec3 *pos, int scriptId, int layer, unsigned int flags);
    Amw2VmId SpawnChildVm(int scriptId, Amw2AnmVm *parent, unsigned int flags);
};

struct Amw2AnmManager {
    unsigned char pad00[0xb8];
    int activeVmCount;
    unsigned char padbc[0x8b9648];
    Amw2ListNode *worldListHead;
    Amw2ListNode *worldListTail;
    Amw2ListNode *uiListHead;
    Amw2ListNode *uiListTail;
    Amw2AnmVm layerHeads[33];
    int nextVmId;

    void FlushAnmSpriteBatch();
    void CleanupVm(Amw2AnmVm *vm);
    Amw2AnmVm *GetVmById(Amw2VmId id);
    int DrawLayer4();
    int DrawLayer21();
    int DrawLayer24();
    int DrawLayer32();
    int UpdateWorldVms();
    int DrawVmGroup(int layer);
};

void SetInterruptAndUpdate(Amw2VmId id, short interrupt);
Amw2AnmVm *AllocateAnmVm();

extern Amw2D3dDevice *g_Amw2D3dDevice;
extern Amw2AnmManager *g_Amw2AnmManager;
extern Amw2SpriteViewport g_Amw2Viewport0;
extern Amw2SpriteViewport g_Amw2Viewport1;
extern Amw2SpriteViewport g_Amw2Viewport2;
extern Amw2SpriteViewport *g_Amw2CurrentViewport;
extern int g_Amw2CurrentViewportId;
extern int g_Amw2FogEnabled;
extern unsigned int g_Amw2SupervisorFlags;
extern CRITICAL_SECTION g_Amw2CritSec;
extern unsigned char g_Amw2CritSecDepth;

} // namespace th128

#endif
