#include "../src/ReplayScoreWave.hpp"

namespace th128 {

void *g_RswReplayActive = NULL;
RswInputState g_RswInput;
unsigned int g_RswInputHeld = 0;
unsigned int g_RswInputPressed = 0;
unsigned int g_RswGameFlags = 0;
CRITICAL_SECTION g_RswRngCs;
unsigned char g_RswRngLockDepth = 0;
unsigned short g_RswRngSeed = 0;
unsigned int g_RswRngCount = 0;
char g_RswAppDataDir[0x1000];
char g_RswGameDir[0x1000];
int g_RswSupMode3c0 = 0;
RswEclViewer *g_RswEclViewer = NULL;

static volatile int g_Sink = 0;

__declspec(noinline) void __stdcall RswDecryptBlocks(unsigned char *buf, int size, unsigned char key, unsigned char step, int block, int limit) {
    g_Sink += (int)key;
    DWORD t = GetTickCount();
    g_Sink += (int)buf + size + (int)step + block + limit + (int)t;
}

__declspec(noinline) void RswMenuCursor::Move(int delta) {
    g_Sink += cur + delta;
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}

__declspec(noinline) void RswReplayEnd() {
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}

__declspec(noinline) void RswInitCardBlock(RswCardBlock *cards) {
    g_Sink += (int)cards;
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}

__declspec(noinline) void RswInitRouteBlock(RswRouteBlock *route) {
    g_Sink += (int)route;
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}

__declspec(noinline) void RswPlaySfx(int id) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += id + (int)t2;
}

__declspec(noinline) void RswStopSfx(int id) {
    DWORD t = GetTickCount();
    g_Sink += id * 3 + (int)t;
}

} // namespace th128

extern "C" int __stdcall ReplayScoreWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) return ((RswReplayMgr *)arg)->PlaybackStep();
    if (selector == 2) { RswScoreFile *sf = new RswScoreFile(); return (int)sf; }
    if (selector == 3) return ((RswSoundTest *)arg)->Update();
    if (selector == 4) return RswLoadSelectedEcl();
    if (selector == 5) { RswDecryptBlocks((unsigned char *)arg, arg + 1, (unsigned char)arg, (unsigned char)(arg >> 8), arg + 2, arg + 3); return g_Sink; }
    return 0;
}
