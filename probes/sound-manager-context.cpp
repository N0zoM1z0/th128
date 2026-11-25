// Non-runnable VC10 /GL context for SoundManager.
#include "../src/SoundManager.hpp"
#include "../src/Supervisor.hpp"

Supervisor g_Supervisor;
SoundManager g_SoundManager;
SfxDesc g_SfxTable[80];

unsigned int *g_ProbeSupFlags = &g_Supervisor.flags;
CRITICAL_SECTION *g_ProbeSupCs = g_Supervisor.criticalSections;
unsigned char *g_ProbeSupDepth = g_Supervisor.lockDepths;
SoundManager *g_ProbeSoundManagerAddr = &g_SoundManager;

extern "C" void __stdcall SoundManagerProbe(
    SoundManager *sm1, SoundManager *sm2,
    SfxChannel *ch1, SfxChannel *ch2,
    BgmStreamView *bs1, BgmStreamView *bs2,
    int id1, int id2, float x1, float x2,
    const char *s1, const char *s2)
{
    SoundDebugLog("probe %s", s1);
    ch1->Init(id1);
    ch2->Init(id2);
    ch1->ReleaseBuffer();
    ch2->ReleaseBuffer();
    ch1->SetPanDirect(id1);
    ch2->SetPanDirect(id2);
    ch1->SetPriority(id1);
    ch2->SetPriority(id2);
    int gid = ch1->GetId() + ch2->GetId();
    void *buf = ch1->GetBuffer();
    void *buf2 = ch2->GetBuffer();
    bs1->SetFadeOut(x1);
    bs2->SetFadeOut(x2);
    bs1->HasField10();
    bs2->HasField10();

    g_SoundManager.PlaySfx(id1);
    g_SoundManager.PlaySfx(id2);
    g_SoundManager.PlaySfxWithPan(id1, x1);
    g_SoundManager.PlaySfxWithPan(id2, x2);
    g_SoundManager.SetSfxPan(id1, x1);
    g_SoundManager.SetSfxPan(id2, x2);
    g_SoundManager.StopSfx(id1);
    g_SoundManager.StopSfx(id2);
    sm1->EnqueueBgmCommand(id1, id2, s1);
    sm2->EnqueueBgmCommand(id2, id1, s2);
    g_SoundManager.SetBgmFadeOut(x1);
    g_SoundManager.SetBgmFadeOut(x2);
    sm1->FreeWaveBuffer18C0(gid + (buf == buf2));
    sm2->FreeWaveBuffer18C0(id2);
}
