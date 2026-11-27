#include "ZeroCallMedium3.hpp"

namespace th128 {

Zc3AnmMgr *g_Zc3AnmMgr = 0;
Zc3ReplayMgr *g_Zc3ReplayMgr = 0;
int g_Zc3StageNum = 0;
unsigned short g_Zc3RngSeed = 0;
int g_Zc3RngCount = 0;
unsigned int g_Zc3SupFlags3cc = 0;
int g_Zc3Score = 0;
int g_Zc3Power = 0;
int g_Zc3PowerMax = 0;
int g_Zc3PowerMin = 0;
int g_Zc3Graze = 0;
int g_Zc3Continues = 0;
int g_Zc3FreezeVal = 0;
int g_Zc3FreezeSub = 0;
int g_Zc3FieldD70 = 0;
float g_Zc3FieldD60 = 0.0f;

volatile unsigned int g_Zc3TimerFlags = 0;
unsigned int g_Zc3SupFlags3f8 = 0;
CRITICAL_SECTION g_Zc3CritSec;
unsigned char g_Zc3CritDepth = 0;
LARGE_INTEGER g_Zc3QpcFreq;
LARGE_INTEGER g_Zc3QpcStart;
double g_Zc3TimeBase = 0.0;
double g_Zc3CurTime = 0.0;
double g_Zc3PrevTime = 0.0;
double g_Zc3NextFrameTime = 0.0;
int g_Zc3TickCount = 0;

Zc3DInputDev *g_Zc3JoyDev = 0;
short g_Zc3JoyBtnShot = 0;
short g_Zc3JoyBtnBomb = 0;
short g_Zc3JoyBtnPause = 0;
short g_Zc3JoyBtnSlow = 0;
short g_Zc3JoyBtnSkip = 0;
unsigned int g_Zc3JoyXMin = 0;
unsigned int g_Zc3JoyXMax = 0;
unsigned int g_Zc3JoyYMin = 0;
unsigned int g_Zc3JoyYMax = 0;
short g_Zc3JoyDeadX = 0;
short g_Zc3JoyDeadY = 0;

volatile int g_Zc3AnchorSink = 0;

__declspec(noinline) void Zc3DebugLog(const char *fmt, ...) {
    g_Zc3AnchorSink ^= (int)fmt;
}

} // namespace th128

using namespace th128;

extern "C" int __stdcall ZeroCallMedium3Probe(
    Zc3Player *player,
    Zc3FontSurface *fsurf,
    Zc3D3DSurf *d3dsurf
) {
    volatile void *ptrs[] = {
        (void *)&Zc3PlayerStartAllOptionAnms,
        (void *)&Zc3TimingThreadProc,
        (void *)&Zc3ReplayStageInit,
        (void *)&Zc3BlitSurface,
        (void *)&Zc3HitTestCircleOrientedRect,
        (void *)&Zc3PollJoystickButtons,
        (void *)&Zc3DebugLog,
        (void *)&g_Zc3StageNum,
        (void *)&g_Zc3RngSeed,
        (void *)&g_Zc3RngCount,
        (void *)&g_Zc3Score,
        (void *)&g_Zc3Power,
        (void *)&g_Zc3Graze,
        (void *)&g_Zc3Continues,
        (void *)&g_Zc3FreezeVal,
        (void *)&g_Zc3FieldD70,
        (void *)&g_Zc3FieldD60,
        (void *)&g_Zc3SupFlags3cc,
    };
    int acc = 0;
    ((void (__stdcall *)(Zc3Player *))ptrs[0])(player);
    ((void (*)())ptrs[1])();
    ((void (*)())ptrs[2])();
    acc ^= (int)((bool (__fastcall *)(Zc3FontSurface *, int, Zc3D3DSurf *, int, int, int))ptrs[3])(fsurf, 0, d3dsurf, 0, 10, 10);
    acc ^= ((int (__stdcall *)(float, float, float, float, float, float, float, float))ptrs[4])(0.0f, 0.0f, 10.0f, 10.0f, 0.5f, 1.0f, 1.0f, 2.0f);
    acc ^= (int)((unsigned int (__fastcall *)(unsigned int))ptrs[5])(0);
    return acc ^ g_Zc3AnchorSink;
}
