#pragma once

#include <windows.h>

namespace th128 {

struct Zc3AnmVm;

struct Zc3AnmListNode {
    Zc3AnmVm *vm;
    Zc3AnmListNode *next;
    Zc3AnmListNode *prev;
};

struct Zc3AnmVm {
    int id;
    unsigned char pad04[0x0c];
    Zc3AnmListNode childNode;
    unsigned char pad1c[0x3f8 - 0x1c];
    short pendingInterrupt;
    unsigned char pad3fa[0x4c8 - 0x3fa];
    void (__fastcall *onInterrupt)(Zc3AnmVm *vm, int interrupt);
    unsigned char pad4cc[0x4e8 - 0x4cc];

    inline void Interrupt(short intr) {
        if (onInterrupt != 0) {
            onInterrupt(this, intr);
        }
        pendingInterrupt = intr;
    }
};

struct Zc3AnmMgr {
    unsigned char pad00[0x8b9704];
    Zc3AnmListNode *worldListHead;
    unsigned int pad8b9708;
    Zc3AnmListNode *uiListHead;
};

struct Zc3PlayerOption {
    int anmIdA;
    int anmIdB;
    unsigned char pad08[0xe4 - 8];
};

struct Zc3Player {
    unsigned char pad00[0x8864];
    Zc3PlayerOption options[8];
    unsigned char pad_after[0x438ec - (0x8864 + 8 * 0xe4)];
    unsigned int flags;
    int field438f0;
};

struct Zc3ReplayStageData {
    unsigned short stageNum;  // +0x00
    unsigned short rngSeed;   // +0x02
    unsigned char pad04[8];
    int score;                // +0x0c
    short power;              // +0x10
    short pad12;
    int graze;                // +0x14
    unsigned char pad18[8];
    int continues;            // +0x20
    unsigned char pad24[0x80 - 0x24];
    int freezeVal;            // +0x80
    int field84;              // +0x84
    float field88;            // +0x88
    unsigned int flags;       // +0x8c
};

struct Zc3ReplayStageSlot {
    int f00;
    int f04;
    int f08;
    int f0c;
    Zc3ReplayStageData *data;
    int f14;
    int pad18[3];
};

struct Zc3ReplayMgr {
    unsigned char pad00[0x10];
    int mode;                            // +0x10
    unsigned char pad14[0x20 - 0x14];
    Zc3ReplayStageData *recStages[7];    // +0x20
    unsigned char pad3c[0x138 - 0x3c];
    Zc3ReplayStageSlot playStages[7];    // +0x138
};

struct Zc3LockedRect {
    int pitch;
    void *pBits;
};

struct Zc3SurfDesc {
    unsigned int format;
    unsigned int type;
    unsigned int usage;
    unsigned int pool;
    unsigned int msType;
    unsigned int msQual;
    unsigned int width;
    unsigned int height;
};

struct Zc3D3DSurfVtbl {
    void *pad00[12];
    HRESULT (__stdcall *GetDesc)(void *self, Zc3SurfDesc *desc);
    HRESULT (__stdcall *LockRect)(void *self, Zc3LockedRect *rect, const RECT *r, unsigned int flags);
    HRESULT (__stdcall *UnlockRect)(void *self);
};

struct Zc3D3DSurf {
    Zc3D3DSurfVtbl *vtbl;
};

struct Zc3FontSurface {
    unsigned char pad00[0x100];
    unsigned int format;    // +0x100
    int width;              // +0x104
    int height;             // +0x108
    unsigned int pad10c;
    int pitch;              // +0x110
    unsigned char pad114[8];
    int initialized;        // +0x11c
    unsigned char *pixels;  // +0x120
};

extern Zc3AnmMgr *g_Zc3AnmMgr;
extern Zc3ReplayMgr *g_Zc3ReplayMgr;
extern int g_Zc3StageNum;
extern unsigned short g_Zc3RngSeed;
extern int g_Zc3RngCount;
extern unsigned int g_Zc3SupFlags3cc;
extern int g_Zc3Score;
extern int g_Zc3Power;
extern int g_Zc3PowerMax;
extern int g_Zc3PowerMin;
extern int g_Zc3Graze;
extern int g_Zc3Continues;
extern int g_Zc3FreezeVal;
extern int g_Zc3FreezeSub;
extern int g_Zc3FieldD70;
extern float g_Zc3FieldD60;

extern volatile unsigned int g_Zc3TimerFlags;
extern unsigned int g_Zc3SupFlags3f8;
extern CRITICAL_SECTION g_Zc3CritSec;
extern unsigned char g_Zc3CritDepth;
extern LARGE_INTEGER g_Zc3QpcFreq;
extern LARGE_INTEGER g_Zc3QpcStart;
extern double g_Zc3TimeBase;
extern double g_Zc3CurTime;
extern double g_Zc3PrevTime;
extern double g_Zc3NextFrameTime;
extern int g_Zc3TickCount;

struct Zc3DInputDevVtbl {
    void *pad00[7];
    HRESULT (__stdcall *Acquire)(void *self);
    void *pad08[1];
    HRESULT (__stdcall *GetDeviceState)(void *self, DWORD cbData, LPVOID lpvData);
    void *pad0a[15];
    HRESULT (__stdcall *Poll)(void *self);
};

struct Zc3DInputDev {
    Zc3DInputDevVtbl *vtbl;
};

struct Zc3JoyState {
    long lX;
    long lY;
    unsigned char pad08[0x28];
    unsigned char rgbButtons[0x110 - 0x30];
};

extern Zc3DInputDev *g_Zc3JoyDev;
extern short g_Zc3JoyBtnShot;
extern short g_Zc3JoyBtnBomb;
extern short g_Zc3JoyBtnPause;
extern short g_Zc3JoyBtnSlow;
extern short g_Zc3JoyBtnSkip;
extern unsigned int g_Zc3JoyXMin;
extern unsigned int g_Zc3JoyXMax;
extern unsigned int g_Zc3JoyYMin;
extern unsigned int g_Zc3JoyYMax;
extern short g_Zc3JoyDeadX;
extern short g_Zc3JoyDeadY;
extern void Zc3DebugLog(const char *fmt, ...);

void __stdcall Zc3PlayerStartAllOptionAnms(Zc3Player *player);
void Zc3TimingThreadProc();
void Zc3ReplayStageInit();
bool __fastcall Zc3BlitSurface(Zc3FontSurface *self, int x, Zc3D3DSurf *surf, int y, int w, int h);
int __stdcall Zc3HitTestCircleOrientedRect(float cx, float cy, float w, float h, float angle, float px, float py, float r);
unsigned int __fastcall Zc3PollJoystickButtons(unsigned int keys);

} // namespace th128
