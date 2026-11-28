#include "../src/CleanBatchL.hpp"

namespace th128 {

int g_CblReplayActive = 0;
unsigned int g_CblInput0 = 0;
unsigned int g_CblInput4 = 0;
unsigned int g_CblInputC = 0;
unsigned int g_CblInput10 = 0;
CblInputObj g_CblInputObj;
int g_CblMode = 0;
int g_CblMode2 = 0;
int g_CblDiff = 0;
void* g_CblModePtr = 0;
int g_CblData1 = 0;
int g_CblData2 = 0;
CblScoreMgr* g_CblScoreMgr = 0;
CblStageBg* g_CblStageBg = 0;
CblAnmMgr* g_CblAnmMgr = 0;
CblGame2* g_CblGame2 = 0;
CblSeMgr g_CblSeMgr;

static volatile int g_Sink = 0;

__declspec(noinline) void CblInputObj::Update() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}
__declspec(noinline) void CblOnReplayEnd() {
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}
__declspec(noinline) int __stdcall CblFindScoreRank(void* entry) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_Sink += (int)entry + (int)(t1 * 3 + t2 * 5 + t3);
    return g_Sink;
}
__declspec(noinline) void __stdcall CblPlaySe(int* timer, int id) {
    g_Sink += (int)timer * 3;
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_Sink += id + (int)(t1 * 3 + t2 * 5 + t3);
}
__declspec(noinline) void CblInitVm(void* anm, CblBgVm* vm, int scriptId) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_Sink += (int)anm + (int)vm + scriptId + (int)(t1 * 3 + t2 * 5 + t3);
}
__declspec(noinline) void CblUpdateVm(CblBgVm* vm) {
    DWORD t = GetTickCount();
    g_Sink += (int)vm + (int)t;
}
__declspec(noinline) void CblSaveConfig(CblKeyConfig* cfg) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)cfg + (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += (int)cfg + (int)t2 + (int)t1 * 3;
}
__declspec(noinline) void CblSeMgr::PlaySe(int id) {
    int a = g_Sink;
    g_Sink = a + (1 << (a & 7)) + id * 3;
}

} // namespace th128

extern "C" int __stdcall CleanBatchLProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        return ((CblReplay*)arg)->StepInput();
    }
    if (selector == 2) {
        ((CblPauseMenu*)arg)->SetupNameEntry();
        return g_Sink;
    }
    if (selector == 3) {
        CblStageBgInit();
        return g_Sink;
    }
    if (selector == 4) {
        CblEnemyMgrClearAll((CblEnemyMgr*)arg);
        return g_Sink;
    }
    if (selector == 5) {
        CblSwapKey(arg & 3, (arg >> 2) & 0xff, (CblKeyConfig*)arg);
        return g_Sink;
    }
    if (selector == 6) {
        int a, b;
        CblDecodeFreezePercent(&a, &b);
        return a + b;
    }
    if (selector == 7) {
        return ((CblScoreTable*)arg)->GetStageValue(arg & 7, (arg >> 3) & 3);
    }
    if (selector == 8) {
        CblFindScoreRank((void*)arg);
        CblPlaySe((int*)arg, arg);
        CblInitVm((void*)arg, (CblBgVm*)(arg + 4), arg + 8);
        CblUpdateVm((CblBgVm*)arg);
        CblSaveConfig((CblKeyConfig*)arg);
        g_CblSeMgr.PlaySe(arg);
        return g_Sink;
    }
    return 0;
}
