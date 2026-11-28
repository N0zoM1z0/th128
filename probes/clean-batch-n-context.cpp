#include "../src/CleanBatchN.hpp"

namespace th128 {

CbnEnemyMgr* g_CbnEnemyMgr = 0;
CbnAnmMgr* g_CbnAnmMgr = 0;
void* g_CbnDsound = 0;
unsigned char g_CbnBgmEnabled = 0;
void* g_CbnSoundMgr = 0;
unsigned char g_CbnSoundFlags = 0;
char g_CbnBgmNames[16][256];
void* g_CbnBgmBufs[16];
CbnWaveHdr* g_CbnBgmHdrs[16];
unsigned int g_CbnBgmSizes[16];
unsigned int g_CbnBgmAux[16];
HANDLE g_CbnBgmEvent = 0;
HANDLE g_CbnBgmThread = 0;
DWORD g_CbnBgmThreadId = 0;
void* g_CbnSoundParam = 0;
CbnGuid16 g_CbnGuid;
CbnStreaming g_CbnStreaming;
int g_CbnCurBgmSlot = 0;

static volatile int g_Sink = 0;

__declspec(noinline) void CbnEnemySub::Cleanup() {
    DWORD t1 = GetTickCount();
    g_Sink += (int)this + (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += (int)this + (int)t2;
}
__declspec(noinline) int __fastcall CbnPlayBgmFallback(const char* name) {
    DWORD t = GetTickCount();
    g_Sink += (int)name + (int)t;
    return g_Sink;
}
__declspec(noinline) void __cdecl CbnLogPrintf(const char* fmt, ...) {
    DWORD t = GetTickCount();
    g_Sink += (int)fmt + (int)t;
}
__declspec(noinline) DWORD __stdcall CbnBgmThreadProc(LPVOID param) {
    return (DWORD)param + g_Sink;
}
__declspec(noinline) int __stdcall CbnStreaming::CreateFromMem(void* dsound, void* buf, CbnWaveHdr* hdr, CbnGuid16 guid, HANDLE ev, unsigned int bytes, unsigned int size) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_Sink += (int)this + (int)dsound + (int)buf + (int)hdr + (int)guid.a + (int)ev + (int)bytes + (int)size + (int)(t1 + t2);
    return g_Sink;
}

} // namespace th128

extern "C" int __stdcall CleanBatchNProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        ((CbnEnemy*)arg)->~CbnEnemy();
        return g_Sink;
    }
    if (selector == 2) {
        return ((CbnTextTex*)arg)->BlitToSurface((CbnSurface*)(arg + 4), arg & 15, (arg >> 4) & 15, (arg >> 8) & 15, (arg >> 12) & 15) ? 1 : 0;
    }
    if (selector == 3) {
        return CbnPlayBgmSlot(arg & 15);
    }
    if (selector == 4) {
        CbnGuid16 g = { 1, 2, 3, 4 };
        g_CbnStreaming.CreateFromMem((void*)arg, (void*)arg, (CbnWaveHdr*)arg, g, (HANDLE)arg, (unsigned int)arg, (unsigned int)arg);
        CbnPlayBgmFallback((const char*)arg);
        return g_Sink;
    }
    return 0;
}
