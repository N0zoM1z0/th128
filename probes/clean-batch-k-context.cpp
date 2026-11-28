#include "../src/CleanBatchK.hpp"

namespace th128 {

int g_CbkHighScore = 0;
int g_CbkScore = 0;
int g_CbkDifficulty = 0;
int g_CbkStage = 0;
int g_CbkHighScoreStage = 0;
int g_CbkExtendIdx = 0;
unsigned int g_CbkStageFlags = 0;
int g_CbkNormalExtends[16];
int g_CbkExtraExtends[16];
CbkGui* g_CbkGui = 0;
CbkEnemyMgr* g_CbkEnemyMgr = 0;
CbkAnmMgr* g_CbkAnmMgr = 0;
CbkScoreMgr* g_CbkScoreMgr = 0;
CbkSoundMgr g_CbkSoundMgr;
unsigned char g_CbkDefaultKeys[18];
unsigned int g_CbkInput = 0;
unsigned int g_CbkSupFlags = 0;
int g_CbkNextMode = 0;

static volatile int g_Sink = 0;

__declspec(noinline) void CbkThreadBase::Reset() {
    DWORD t1 = GetTickCount();
    g_Sink += (int)this + (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += (int)this + (int)t2;
}
__declspec(noinline) CbkAnmVm* CbkAnmMgr::FindVm(unsigned int id) {
    int a = g_Sink;
    g_Sink = (a ^ (1 << (a & 7))) + (int)this * 3;
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    DWORD t4 = GetTickCount();
    g_Sink += (int)id + (int)(t1 * 3 + t2 * 5 + t3 * 7 + t4);
    return (CbkAnmVm*)g_Sink;
}
__declspec(noinline) void CbkSoundMgr::PlayBgm(const char* path, int a, int b) {
    g_Sink += (int)this * 3;
    DWORD t1 = GetTickCount();
    g_Sink += (int)t1 + (int)path;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)path + a + b;
}
__declspec(noinline) void CbkShowPopup(CbkGui* gui, int a, int b, int c) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_Sink += (int)gui + a + b + c + (int)(t1 * 3 + t2 * 5);
}
__declspec(noinline) void CbkOnExtend(int* highScorePtr) {
    g_Sink += (int)highScorePtr;
}
__declspec(noinline) void CbkAnmMgr::DrawVm(void* vm) {
    g_Sink += (int)vm * 3;
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    DWORD t4 = GetTickCount();
    g_Sink += (int)this + (int)(t1 * 3 + t2 * 5 + t3 * 7 + t4);
}
__declspec(noinline) int __stdcall CbkUpdateEndingSub(CbkEndingSubObj* sub) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    DWORD t4 = GetTickCount();
    g_Sink += (int)sub + (int)(t1 * 3 + t2 * 5 + t3 * 7 + t4);
    return g_Sink;
}
__declspec(noinline) void CbkTickTimer(int* timer) {
    DWORD t = GetTickCount();
    g_Sink += (int)timer + (int)t;
}

} // namespace th128

extern "C" int __stdcall CleanBatchKProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        CbkUpdateScoreDisplay();
        return g_Sink;
    }
    if (selector == 2) {
        ((CbkEndingSub*)arg)->~CbkEndingSub();
        return g_Sink;
    }
    if (selector == 3) {
        return ((CbkGui*)arg)->Draw();
    }
    if (selector == 4) {
        ((CbkStageClear*)arg)->ClearVms();
        return g_Sink;
    }
    if (selector == 5) {
        return CbkPlayMusicTrack(arg & 7, (const char*)arg);
    }
    if (selector == 6) {
        CbkInitConfig((CbkConfig*)arg);
        return g_Sink;
    }
    if (selector == 7) {
        return ((CbkEndingObj*)arg)->OnCalc();
    }
    if (selector == 8) {
        CbkShowPopup((CbkGui*)arg, arg, arg + 1, arg + 2);
        CbkOnExtend((int*)arg);
        ((CbkAnmMgr*)(arg + 4))->DrawVm((void*)arg);
        ((CbkSoundMgr*)arg)->PlayBgm((const char*)arg, arg, arg + 1);
        return g_Sink;
    }
    return 0;
}
