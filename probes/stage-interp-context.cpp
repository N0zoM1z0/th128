#include "../src/StageInterp.hpp"

StageBgStateMini *g_StageBgState;
StageAnmManagerMini *g_StageAnmManager;
float g_StageDefaultTimeScale;

typedef int (__stdcall *UpdateVmFn)(StageAnmVmMini *);
typedef void (StageFog::*PackRgbaFn)();
PackRgbaFn g_StageFogPackRgba = &StageFog::PackRgba;

extern "C" int __stdcall StageInterpProbe(
    StageBgStateMini *bg, StageInterp<StageVec3> *iv, StageInterp<StageFog> *ifog,
    StageVec3 *va, StageVec3 *vb, StageFog *fa, StageFog *fb,
    StageAnmVmMini *vm, float *fp, int n, float s)
{
    int acc = 0;
    StageBgStateMini::JumpToLabel(n);
    bg->SetVecPair(va, vb);
    bg->SetFlag2910();
    bg->StopBgVms();
    StageBgStateMini::StartFadeOut60();

    *va = iv->Step();
    acc += iv->GetDuration();
    iv->SetMode(n);
    iv->SetStart(*va);
    iv->SetEnd(*vb);
    iv->SetVel0(*va);
    iv->SetVel1(*vb);
    iv->SetDuration(n);
    acc += (int)iv->EvalT();

    *fa = ifog->Step();
    acc += ifog->GetDuration();
    ifog->SetMode(n);
    ifog->SetStart(*fa);
    ifog->SetEnd(*fb);
    ifog->SetDuration(n);
    ifog->ClearDuration();
    ifog->ResetTimer();
    acc += (int)ifog->EvalT();

    StageFog fc(s, s, s, s, s, s);
    fc = (*fa - *fb) * s + *fa;
    (fc.*g_StageFogPackRgba)();
    *fb = fc;

    *va = (*va * s) / s;
    acc += (int)va->LengthSq();
    acc += (int)StageCosf(s);
    StageNormalizeAngleInto(fp, s);
    vm->ClearFlag0();
    acc += vm->HasField424() ? 1 : 0;
    StageSetAnmDirty();
    UpdateVmFn pUpd = &StageUpdateVm;
    acc += pUpd(vm);
    acc += (int)StageMalloc((size_t)n);
    return acc;
}
