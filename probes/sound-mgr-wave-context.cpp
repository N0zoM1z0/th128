#include "../src/SoundMgrWave.hpp"

namespace th128 {

SmwSoundMgr g_SmwSoundMgr;
SmwSfxTableEntry g_SmwSfxTable[0x4c];
const char *g_SmwWavNames[65];
SmwErrorContext g_SmwErrorCtx;
SmwSupervisor g_SmwSupervisor;
int g_SmwAudioMode = 0;
void *g_SmwThreadParam = NULL;
const GUID g_SmwGuidNull = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
HRESULT (__stdcall *g_SmwDirectSoundCreate8)(const GUID *, ISmwDirectSound8 **, void *) = NULL;
static volatile int g_Sink = 0;

__declspec(noinline) DWORD __stdcall SmwBgmThreadProc(LPVOID param) {
    DWORD t = GetTickCount();
    g_Sink += (int)param + (int)t;
    return 0;
}

__declspec(noinline) void __cdecl SmwSoundDebugLog(const char *fmt, ...) {
    DWORD t = GetTickCount();
    g_Sink += (int)fmt + (int)t;
}

__declspec(noinline) void __cdecl SmwErrorContext::Log(const char *fmt, ...) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)fmt + (int)t;
}

__declspec(noinline) void SmwSupervisor::UnlockCritSec(int idx) {
    if ((gameFlags590 & 0x8000U) != 0) {
        LeaveCriticalSection(&critSecs810[idx]);
        --lockDepth930[idx];
    }
}

__declspec(noinline) HRESULT SmwCSoundManager::SetPrimaryBufferFormat() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
    return (HRESULT)g_Sink;
}

__declspec(noinline) HRESULT SmwCSoundManager::CreateStreamingFromFile(
    SmwStreamingSound **out, const char *path, DWORD flags, GUID alg,
    DWORD notifyCount, DWORD notifySize, HANDLE ev, SmwTrackInfo *info) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)out + (int)path + (int)flags + (int)alg.Data1 + (int)notifyCount + (int)notifySize + (int)ev + (int)info + (int)t;
    return (HRESULT)g_Sink;
}

__declspec(noinline) HRESULT SmwCSoundManager::CreateStreamingFromMemory(
    SmwStreamingSound **out, unsigned char *data, DWORD dataSize, SmwTrackInfo *info,
    DWORD flags, GUID alg, DWORD notifyCount, DWORD notifySize, HANDLE ev) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)out + (int)data + (int)dataSize + (int)info + (int)flags + (int)alg.Data1 + (int)notifyCount + (int)notifySize + (int)ev + (int)t;
    return (HRESULT)g_Sink;
}

__declspec(noinline) void SmwSoundMgr::StopAndDestroyBgm() {
    if (ds0 == NULL) return;
    DWORD t = GetTickCount();
    g_Sink += (int)ds0 + (int)t;
    ds0 = NULL;
}

__declspec(noinline) int SmwSoundMgr::LoadWav(int slotIdx, const char *wavName) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + slotIdx + (int)wavName + (int)t;
    return g_Sink;
}

__declspec(noinline) int SmwSoundMgr::OpenBgmByName(const char *name) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)name + (int)t;
    return g_Sink;
}

} // namespace th128

extern "C" int __stdcall SoundMgrWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) return ((SmwSoundMgr *)arg)->Initialize((HWND)arg);
    if (selector == 2) return g_SmwSoundMgr.Shutdown();
    if (selector == 3) return ((SmwSoundMgr *)arg)->FindTrackIndex((const char *)arg);
    if (selector == 4) return ((SmwSoundMgr *)arg)->StartStreamingBgm();
    if (selector == 5) return g_SmwSoundMgr.PreloadBgm((const char *)arg, arg & 15);
    if (selector == 6) return g_SmwSoundMgr.LoadPreloadedBgm(arg & 15);
    if (selector == 7) { ((SmwSupervisor *)arg)->UnlockCritSec(arg); return g_Sink; }
    return 0;
}
