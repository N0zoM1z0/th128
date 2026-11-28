#include "../src/CleanSmallWaveD.hpp"

namespace th128 {

volatile int g_CsdSink = 0;

CsdScoreGlobal *g_CsdScoreGlobal = NULL;
CsdDirectInput *g_CsdDirectInput = NULL;
void *g_CsdPads[1] = { NULL };
unsigned char *g_CsdTextBits = NULL;
int g_CsdTextByteLen = 0;
int g_CsdSavedScreenSave = 0;
int g_CsdSavedLowPower = 0;
int g_CsdSavedPowerOff = 0;

__declspec(noinline) CsdLaserBase::CsdLaserBase() {
    Sleep(1);
    pad04[0] = 0;
    g_CsdSink += (int)this;
}

__declspec(noinline) CsdLaserBase::~CsdLaserBase() {
    g_CsdSink += pad04[0];
}

__declspec(noinline) CsdLaserLine::~CsdLaserLine() {
    g_CsdSink += pad5c0[0];
}

__declspec(noinline) CsdAnmVm::CsdAnmVm() {
    Sleep(1);
    data[0] = 0;
    g_CsdSink += (int)this;
}

__declspec(noinline) void CsdThreadObj::Stop() {
    if (this->handle != NULL) {
        WaitForSingleObject(this->handle, 100);
        CloseHandle(this->handle);
        this->handle = NULL;
    }
}

__declspec(noinline) CsdCardEffectMgr::CsdCardEffectMgr() {
    Sleep(1);
    g_CsdSink += (int)this;
}

__declspec(noinline) void CsdCardEffectMgr::Shutdown() {
    g_CsdSink += (int)this;
    Sleep(1);
}

__declspec(noinline) CsdScoreMgr::CsdScoreMgr() {
    Sleep(1);
    g_CsdSink += (int)this;
}

__declspec(noinline) CsdScoreMgr::~CsdScoreMgr() {
    Sleep(1);
    g_CsdSink += (int)this;
}

__declspec(noinline) unsigned __stdcall CsdWorkerProc(void *arg) {
    g_CsdSink += (int)arg;
    return 0;
}

__declspec(noinline) int __stdcall CsdInitCardEffectMgr(CsdCardEffectMgr *mgr) {
    Sleep(1);
    return (int)mgr & 1;
}

__declspec(noinline) int __stdcall CsdRegScoreMgr(CsdScoreMgr *mgr) {
    Sleep(1);
    return (int)mgr & 1;
}

} // namespace th128

static void *volatile g_CsdKeep[8];
static void (th128::CsdBullet::*volatile g_CsdBulletDtor)() = NULL;

extern "C" int __stdcall CleanSmallWaveDProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    g_CsdKeep[0] = (void *)&CsdInitCardEffectMgr;
    g_CsdKeep[1] = (void *)&CsdRegScoreMgr;
    g_CsdKeep[2] = (void *)&CsdEnumJoysticksCb;

    CsdDestroyScoreGlobal();
    CsdDestroyScoreGlobal();

    CsdEclStack *es = (CsdEclStack *)(int)k;
    g_CsdSink += es->Push((const void *)(int)a, (char)b);
    g_CsdSink += (es + 1)->Push((const void *)(int)b, (char)a);

    g_CsdSink += (int)(new CsdLaserLine());

    CsdFontHolder *fh = (CsdFontHolder *)(int)k;
    fh->RecreateFont();
    (fh + 1)->RecreateFont();

    CsdThreadObj *to = (CsdThreadObj *)(int)k;
    to->StartWorker();
    (to + 1)->StartWorker();

    g_CsdSink += (int)CsdCreateCardEffectMgr();
    g_CsdSink += (int)CsdCreateCardEffectMgr();
    g_CsdSink += (int)CsdCreateScoreMgr();
    g_CsdSink += (int)CsdCreateScoreMgr();

    CsdFillTextImageRgb444((unsigned int)a);
    CsdFillTextImageRgb444((unsigned int)b);

    CsdBullet *bul = new CsdBullet[2];
    delete[] bul;

    g_CsdSink += (int)(new CsdReplayStage());

    CsdRestoreSystemSettings();
    CsdRestoreSystemSettings();

    return g_CsdSink;
}
