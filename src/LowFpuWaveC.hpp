#pragma once

#include <windows.h>
#include <string.h>

namespace th128 {

void LfcResetEnemySub(void *sub);

struct LfcTimer {
    int previous;
    int current;
    float currentF;
    const float *gameSpeed;
    unsigned int control;

    LfcTimer() {
        control &= ~1u;
    }

    void Reset() {
        if ((control & 1u) == 0) {
            currentF = 0.0f;
            current = 0;
            previous = -999999;
            gameSpeed = &g_LfcGameSpeed;
            control |= 1u;
        }
        currentF = 0.0f;
        current = 0;
        previous = -1;
    }

    static float g_LfcGameSpeed;
};

struct LfcChainNode {
    int priority;
    unsigned int flags;
    void *callback;
    void *onRegister;
    void *onDelete;
    LfcChainNode *self;
    LfcChainNode *next;
    LfcChainNode *prev;
    void *arg;

    LfcChainNode() {
        flags &= ~1u;
        callback = NULL;
        onRegister = NULL;
        onDelete = NULL;
        priority = 0;
        self = this;
        next = NULL;
        prev = NULL;
    }
};

struct LfcGameGlobal {
    unsigned char pad00[0x8a7f9c];
    int f8a7f9c;
};

struct LfcEndingSubBase {
    virtual ~LfcEndingSubBase();
    unsigned char pad04[0x108c];
    void *f1090;
    void *f1094;

    LfcEndingSubBase() {
        f1090 = NULL;
        f1094 = NULL;
        memset(this, 0, sizeof(LfcEndingSubBase));
    }
};

struct LfcEndingSub : LfcEndingSubBase {
    virtual ~LfcEndingSub();
    virtual int V1();
    virtual int Load(int arg);

    LfcEndingSub() {}
};

struct LfcEnding {
    unsigned char pad00[0x08];
    LfcChainNode *calcNode;
    LfcChainNode *drawNode;
    unsigned char pad10[0x70];
    int f80;
    unsigned char pad84[0x1c];
    LfcTimer timerA0;
    LfcEndingSub *subB4;

    int Init(int arg);
};

struct LfcObj36470 {
    unsigned char pad00[0x08];
    LfcChainNode *calcNode;
    LfcChainNode *drawNode;
    LfcTimer timer10;
    LfcTimer timer24;

    int Register();
};

struct LfcVec3 {
    float x;
    float y;
    float z;
};

struct LfcPopup {
    unsigned char digits[12];
    LfcVec3 pos;
    int color;
    LfcTimer timer;
    unsigned char pad30[8];
    unsigned char active;
    unsigned char digitCount;
    unsigned char pad3a[6];
};

struct LfcPopupMgr {
    unsigned char pad00[0x14];
    int nextIdx;
    unsigned char pad18[0x4e8];
    LfcPopup popups[16];
};

struct LfcAnmVm;

struct LfcAnmListNode {
    LfcAnmVm *vm;
    LfcAnmListNode *next;
};

struct LfcAnmVm {
    unsigned char pad00[0x10];
    LfcAnmListNode listNode;
    unsigned char pad18[0x124];
    int f13c;
    int f140;
    int f144;
    int f148;
    LfcTimer timer14c;
    int f160;
    int f164;
    unsigned char pad168[0x2b6];
    short scriptIdx;
    unsigned char pad420[0x20];
    float f440;
    unsigned char pad444[0xa4];

    LfcAnmVm();
    LfcAnmVm *FindChildByScript(int targetScript) {
        LfcAnmListNode *node = &listNode;
        while (node != NULL) {
            LfcAnmVm *vm = node->vm;
            if (vm->scriptIdx == targetScript || (targetScript == -1 && vm != this)) {
                return vm;
            }
            node = node->next;
        }
        return NULL;
    }
    void SetEffectState(int p1, unsigned char p2, unsigned char p3, unsigned char p4);
};

struct LfcAnmManager {
    LfcAnmVm *GetVmById(int id);
};

struct LfcHud {
    unsigned char pad00[0x68];
    int anmId68;

    void SetBarFill(float fill, int index);
};

struct LfcD3dRect {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
};

struct LfcD3dDevice {
    virtual HRESULT __stdcall M00() = 0;
    virtual HRESULT __stdcall M01() = 0;
    virtual HRESULT __stdcall M02() = 0;
    virtual HRESULT __stdcall M03() = 0;
    virtual HRESULT __stdcall M04() = 0;
    virtual HRESULT __stdcall M05() = 0;
    virtual HRESULT __stdcall M06() = 0;
    virtual HRESULT __stdcall M07() = 0;
    virtual HRESULT __stdcall M08() = 0;
    virtual HRESULT __stdcall M09() = 0;
    virtual HRESULT __stdcall M10() = 0;
    virtual HRESULT __stdcall M11() = 0;
    virtual HRESULT __stdcall M12() = 0;
    virtual HRESULT __stdcall M13() = 0;
    virtual HRESULT __stdcall M14() = 0;
    virtual HRESULT __stdcall M15() = 0;
    virtual HRESULT __stdcall M16() = 0;
    virtual HRESULT __stdcall M17() = 0;
    virtual HRESULT __stdcall M18() = 0;
    virtual HRESULT __stdcall M19() = 0;
    virtual HRESULT __stdcall M20() = 0;
    virtual HRESULT __stdcall M21() = 0;
    virtual HRESULT __stdcall M22() = 0;
    virtual HRESULT __stdcall M23() = 0;
    virtual HRESULT __stdcall M24() = 0;
    virtual HRESULT __stdcall M25() = 0;
    virtual HRESULT __stdcall M26() = 0;
    virtual HRESULT __stdcall M27() = 0;
    virtual HRESULT __stdcall M28() = 0;
    virtual HRESULT __stdcall M29() = 0;
    virtual HRESULT __stdcall M30() = 0;
    virtual HRESULT __stdcall M31() = 0;
    virtual HRESULT __stdcall M32() = 0;
    virtual HRESULT __stdcall M33() = 0;
    virtual HRESULT __stdcall M34() = 0;
    virtual HRESULT __stdcall M35() = 0;
    virtual HRESULT __stdcall M36() = 0;
    virtual HRESULT __stdcall SetRenderTarget(DWORD idx, void *surface) = 0;
    virtual HRESULT __stdcall M38() = 0;
    virtual HRESULT __stdcall M39() = 0;
    virtual HRESULT __stdcall M40() = 0;
    virtual HRESULT __stdcall M41() = 0;
    virtual HRESULT __stdcall M42() = 0;
    virtual HRESULT __stdcall Clear(DWORD count, const LfcD3dRect *rects, DWORD flags, DWORD color, float z, DWORD stencil) = 0;
};

struct LfcAbilityEntry {
    unsigned char pad00[0x150];
    LfcTimer timer;
};

struct LfcAbilityMgr {
    virtual ~LfcAbilityMgr();

    unsigned int flags04;
    unsigned char pad08[0x0c];
    LfcAnmVm vm1;
    LfcAnmVm vm2;
    LfcAbilityEntry entries[0x141];
    unsigned char pad1c848[0x04];
    int f1c84c;
    unsigned char pad1c850[0x08];
    float f1c858;
    float f1c85c;
    unsigned char pad1c860[0x04];
    int f1c864;
    unsigned char pad1c868[0x10];
    int f1c878;
    int f1c87c;
    unsigned char pad1c880[0x0c];
    int f1c88c;
    unsigned char pad1c890[0x18];

    LfcAbilityMgr();
};

struct LfcLaser {
    virtual ~LfcLaser();
    virtual void V1();
    virtual void V2();
    virtual void V3();
    virtual void V4();
    virtual void OnDraw();

    unsigned char pad04[0x04];
    LfcLaser *next08;
    int state0c;
};

struct LfcLaserMgr {
    unsigned char pad00[0x18];
    LfcLaser *head18;

    int OnDraw();
};

struct LfcEclFrame {
    float time;
    void *ip;
};

struct LfcEclSlot {
    void *ptr;
    unsigned char pad04[0x28];
};

struct LfcSptResourceInf;

struct LfcEclVm {
    virtual ~LfcEclVm();

    LfcEclFrame *curFrame;
    unsigned char sub08[0x1044];
    LfcEclSlot slots[8];
    LfcSptResourceInf *resource11ac;
    unsigned char pad11b0[0x0c];

    LfcEclVm() {
        LfcResetEnemySub(sub08);
        for (int i = 0; i < 8; i++) {
            slots[i].ptr = NULL;
        }
    }
};

struct LfcSptResourceInf {
    void *FindSub(const char *name);
    LfcEclVm *CreateVm(const char *name);
};

extern LfcGameGlobal *g_LfcGameGlobal;
extern LfcPopupMgr *g_LfcPopupMgr;
extern LfcAnmManager *g_LfcAnmManager;
extern LfcD3dDevice *g_LfcD3dDevice;
extern void *g_LfcSurfaceA;
extern void *g_LfcSurfaceB;
extern int g_LfcViewportX;
extern int g_LfcViewportY;
extern int g_LfcViewportW;
extern int g_LfcViewportH;
extern DWORD g_LfcClearColor;
extern LfcAbilityMgr *g_LfcAbilityMgr;

void LfcRegisterCalc(LfcChainNode *node, int prio);
void LfcRegisterDraw(LfcChainNode *node, int prio);
void LfcFlushAnmSpriteBatch(LfcAnmManager *mgr);
void LfcResetEnemySub(void *sub);
void LfcEndingOnCalc();
void LfcEndingOnDraw();
void LfcObj36470OnCalc();
void LfcObj36470OnDraw();

void LfcSpawnPopupNumber(const LfcVec3 *pos, int value, int color);
int LfcPauseDrawTargetA();
int LfcPauseDrawTargetB();

} // namespace th128
