#include "../src/LowCallWaveA.hpp"

namespace th128 {

unsigned int g_LcwSupFlags = 0;
CRITICAL_SECTION g_LcwChainCs;
unsigned char g_LcwChainLockDepth = 0;
int g_LcwSceneNext = 0;

LcwGui *g_LcwGui = NULL;
LcwAnmMgr *g_LcwAnmMgr = NULL;
int g_LcwHiScore = 0;
int g_LcwScore = 0;
int g_LcwDifficulty = 0;
int g_LcwContinueCount = 0;
int g_LcwHiScoreContinue = 0;
int g_LcwExtendIdx = 0;
LcwStageFlagsBits g_LcwStageFlagsBits;
unsigned int g_LcwStageFlags = 0;
int g_LcwExtendNormal[16];
int g_LcwExtendExtra[16];
void *g_LcwSavedMsgBuf = NULL;

unsigned int g_LcwInputRepeat = 0;
unsigned int g_LcwInputPressed = 0;

int g_LcwCurScene = 0;
int g_LcwNextScene = 0;
int g_LcwRouteIdx = 0;
LcwScoreFile *g_LcwScoreFile = NULL;
void *g_LcwActiveTable = NULL;
unsigned char g_LcwTable0[16];
unsigned char g_LcwTable1[16];

int g_LcwReplayActive = 0;
LcwInputState g_LcwInputState;

static volatile int g_Sink = 0;

__declspec(noinline) void __fastcall LcwChainCutImpl(LcwChain *chain, LcwChainElem *elem) {
    DWORD t = GetTickCount();
    g_Sink += (int)chain + (int)elem + (int)t;
}
typedef void (__fastcall *LcwCutFn)(LcwChain *, LcwChainElem *);
volatile LcwCutFn g_LcwCutPtr = &LcwChainCutImpl;

__declspec(noinline) void LcwGui::ShowPopup(int kind, int param1, int param2) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + kind + param1 + param2 + (int)t;
}

__declspec(noinline) void LcwGrantExtend(int *hiScorePtr) {
    DWORD t = GetTickCount();
    g_Sink += *hiScorePtr + (int)t;
}

__declspec(noinline) int LcwMenuCursor::Move(int delta) {
    if (count > 0) {
        int disabledEntries = disabledCount;
    retry:
        cur += delta;
        while (cur >= count) {
            if (wraps) cur -= count;
            else cur = count - 1;
        }
        while (cur < 0) {
            if (wraps) cur += count;
            else cur = 0;
        }
        for (int i = 0; i < disabledEntries; ++i) {
            if (disabled[i] == cur) goto retry;
        }
    }
    return cur;
}

static short g_LcwSoundTbl[64];
__declspec(noinline) void LcwPlaySfx(int id) {
    int v = g_LcwSoundTbl[id * 10];
    if (v >= 0) g_Sink ^= v + id;
}

__declspec(noinline) void LcwStopSfx(int id) {
    int v = g_LcwSoundTbl[id * 10 + 1];
    if (v >= 0) g_Sink ^= v - id;
}

__declspec(noinline) int __stdcall LcwInsertScoreEntry(void *routeBlock) {
    DWORD t = GetTickCount();
    g_Sink += (int)routeBlock + (int)t;
    return g_Sink;
}

__declspec(noinline) void LcwMsgObj::Cleanup() {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_Sink += dummy + (int)t1 * 3 + (int)t2 * 5;
    dummy = (int)(t1 ^ t2);
}

__declspec(noinline) void LcwDeleteVmByIdPtr(unsigned int *idPtr) {
    if (*idPtr == 0) return;
    DWORD t = GetTickCount();
    g_Sink += (int)*idPtr + (int)t;
    *idPtr = 0;
}

__declspec(noinline) void LcwOnReplayEnd() {
    g_Sink += (int)GetTickCount();
}

__declspec(noinline) void LcwInputState::UpdateRepeatMasks() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}
typedef void (LcwInputState::*UpdateRepeatFn)();
volatile UpdateRepeatFn g_UpdateRepeatPtr = &LcwInputState::UpdateRepeatMasks;

} // namespace th128

extern "C" int __stdcall LowCallWaveAProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) return ((LcwChain *)arg)->RunCalcChain();
    if (selector == 2) { LcwGui::UpdateScoreDisplay(); return g_Sink; }
    if (selector == 3) return ((LcwSoundTest *)arg)->Update();
    if (selector == 4) { ((LcwNameEntry *)arg)->Init(); return g_Sink; }
    if (selector == 5) { ((LcwGui *)arg)->PreCleanup(); return g_Sink; }
    if (selector == 6) return ((LcwReplayMgr *)arg)->StepInput();
    if (selector == 7) {
        LcwMenuCursor *a = (LcwMenuCursor *)arg;
        LcwMenuCursor *b = (LcwMenuCursor *)(arg + 0x100);
        g_Sink += a->Move(selector) + b->Move(-1) + a->Move(1);
        g_LcwGui->ShowPopup(selector, arg, arg + 1);
        LcwPlaySfx(arg);
        LcwStopSfx(arg);
        return g_Sink;
    }
    return 0;
}
