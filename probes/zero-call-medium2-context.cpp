#include "ZeroCallMedium2.hpp"

namespace th128 {

Zc2CameraSlot g_Zc2Cameras[3];
int g_Zc2Difficulty = 0;
int g_Zc2Score = 0;
unsigned char g_Zc2Continues = 0;
unsigned char g_Zc2StageNum = 0;
Zc2FpsCounter *g_Zc2Fps = 0;
float g_Zc2GameSpeed = 1.0f;

unsigned char g_Zc2WinMode = 0;
unsigned char g_Zc2CfgByte4e = 0;
unsigned char g_Zc2CfgByte53 = 0;
int g_Zc2WinX = 0;
int g_Zc2WinY = 0;
unsigned int g_Zc2CfgFlags = 0;
unsigned int g_Zc2SupFlags = 0;
int g_Zc2IsWindowed = 0;
int g_Zc2WindowActive = 0;
int g_Zc2WindowClosing = 0;
HWND g_Zc2MainHwnd = 0;
HWND g_Zc2SupHwnd = 0;
RECT g_Zc2WinRect;
int g_Zc2InputCfg[13];

Zc2AnmMgrVB *g_Zc2AnmMgr = 0;
Zc2D3DDev *g_Zc2D3DDev = 0;
Zc2QuadVertexD g_Zc2QuadVertsGlobal[4];

volatile int g_Zc2AnchorSink = 0;

__declspec(noinline) LRESULT CALLBACK Zc2MainWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    g_Zc2AnchorSink ^= (int)h ^ (int)m ^ (int)w ^ (int)l;
    return 0;
}

} // namespace th128

using namespace th128;

extern "C" int __stdcall ZeroCallMedium2Probe(
    Zc2StageCamOwner *camOwner,
    Zc2ScoreFile *scoreFile,
    Zc2Player *player,
    const Zc2Vec3 *pos,
    Zc2Fade *fade,
    HINSTANCE hInst,
    HWND hDlg,
    Zc2D3DTex **ppTex,
    Zc2EclManager *eclMgr,
    Zc2EclHeader *eclHdr
) {
    volatile void *ptrs[] = {
        (void *)&Zc2MainWndProc,
        (void *)&Zc2InitMenuCameras,
        (void *)&Zc2InsertHighScore,
        (void *)&Zc2CreateMainWindow,
        (void *)&Zc2ConfigDialogProc,
        (void *)&Zc2InitQuadVertexBuffer,
        (void *)&Zc2ClearTextureSurface,
    };
    void (Zc2StageCamOwner::*volatile pCam)() = &Zc2StageCamOwner::InitGameCameras;
    Zc2IceShard *(Zc2Player::*volatile pShard)(const Zc2Vec3 *, float, float, int, int, int) = &Zc2Player::SpawnIceShard;
    int (Zc2Fade::*volatile pFade)() = &Zc2Fade::OnCalc;
    int (Zc2EclManager::*volatile pEcl)(Zc2EclHeader *) = &Zc2EclManager::LoadFileData;

    int acc = 0;
    (camOwner->*pCam)();
    ((void (*)())ptrs[1])();
    acc ^= ((int (__stdcall *)(Zc2ScoreFile *))ptrs[2])(scoreFile);
    acc ^= (int)(player->*pShard)(pos, 1.0f, 2.0f, 10, 3, 4);
    acc ^= ((int (__stdcall *)(HINSTANCE))ptrs[3])(hInst);
    acc ^= (int)((INT_PTR (CALLBACK *)(HWND, UINT, WPARAM, LPARAM))ptrs[4])(hDlg, 0, 0, 0);
    acc ^= (fade->*pFade)();
    ((void (*)())ptrs[5])();
    ((void (__stdcall *)(Zc2D3DTex **))ptrs[6])(ppTex);
    acc ^= (eclMgr->*pEcl)(eclHdr);
    return acc ^ g_Zc2AnchorSink;
}
