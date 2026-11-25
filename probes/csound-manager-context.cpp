// Non-runnable VC10 /GL context for CSoundManager, CSoundView, & CWaveFileView.
#include "../src/CSoundManager.hpp"

int g_BgmVolume = 100;
DWORD g_BgmBaseOffset = 0;
const GUID g_IID_IDirectSound3DBuffer = {0};

int *g_ProbeBgmVol = &g_BgmVolume;
DWORD *g_ProbeBgmBase = &g_BgmBaseOffset;

extern "C" HRESULT __stdcall CSoundManagerProbe(
    CSoundManager *m1, CSoundManager *m2, HWND h1, HWND h2,
    CWaveFileView *w1, CWaveFileView *w2,
    CSoundView *s1, CSoundView *s2,
    IDirectSoundBufferFullView *b1, IDirectSoundBufferFullView *b2,
    int *restored, void **out3d, unsigned char *mem, DWORD sz, void *info)
{
    CSoundManager local;
    HRESULT hr = m1->Initialize(h1);
    hr |= m2->Initialize(h2);
    hr |= m1->SetPrimaryBufferFormat();
    hr |= m2->SetPrimaryBufferFormat();
    m1->~CSoundManager();
    m2->~CSoundManager();

    CWaveFileView localW;
    w1->OpenFromMemory(mem, sz, info);
    w2->OpenFromMemory(mem + 4, sz + 4, info);
    hr |= w1->ResetFile(true, sz) + w2->ResetFile(false, 0);
    DWORD rd1 = 0;
    hr |= w1->Read(mem, sz, &rd1) + w2->Read(mem + 4, sz + 4, 0);
    hr |= (HRESULT)w1->GetSize() + (HRESULT)w2->GetSize();
    hr |= w1->Close() + w2->Close();
    hr |= (HRESULT)w1->GetSavedPos() + (HRESULT)w2->GetSavedPos();
    w1->SaveFilePos();
    w2->SaveFilePos();
    w1->~CWaveFileView();
    w2->~CWaveFileView();
    delete w1;
    delete w2;

    hr |= s1->RestoreBuffer(b1, restored);
    hr |= s2->RestoreBuffer(b2, 0);
    if (s1->GetFreeBuffer() == s2->GetFreeBuffer()) hr |= 1;
    if (s1->GetBuffer0() == s2->GetBuffer0()) hr |= 2;
    hr |= s1->Get3DBufferInterface(sz, out3d);
    hr |= s2->Get3DBufferInterface(0, out3d);
    hr |= s1->SetVolume((int)sz);
    hr |= s2->SetVolume(-100);
    hr |= s1->Stop(1) + s2->Stop(0);
    hr |= s1->Pause() + s2->Pause();
    hr |= s1->Reset() + s2->Reset();
    hr |= s1->IsSoundPlaying() + s2->IsSoundPlaying();
    hr |= s1->UpdateFadeOut() + s2->UpdateFadeOut();
    hr |= s1->UpdateFadeIn() + s2->UpdateFadeIn();
    hr |= s1->UpdateFadeLow() + s2->UpdateFadeLow();
    hr |= s1->UpdateFadeHigh() + s2->UpdateFadeHigh();

    WaveDebugLog("probe %d", sz);
    EclDebugLog("probe %d", sz);
    return hr;
}
