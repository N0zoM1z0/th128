#include "../src/AsciiInfCore.hpp"

AsciiInfCore *g_AsciiInfCore;
AsciiGameManagerCore *g_GameManagerCore;
AsciiAnmManagerMini *g_AnmManagerMini;
AsciiSupervisorMini g_SupervisorMini;
float g_DefaultTimeScaleCore;

typedef int (AsciiInfCore::*OnDrawFn)();
typedef void (AsciiAnmVmCore::*FreeExtraFn)();
typedef AsciiRet0 *(AsciiRet0::*Ret0Fn)();
typedef AsciiRet1 *(AsciiRet1::*Ret1Fn)();
typedef AsciiFlag20R *(AsciiFlag20R::*Flag20RFn)();

extern "C" int __stdcall AsciiInfCoreProbe(
    AsciiInfCore *asc, const AsciiVec3 *pos, AsciiEntryId *outId,
    const char *str, int val, float fw,
    AsciiSupervisorMini *sup, AsciiAnmVmCore *vm, StageBgMini *bg,
    AsciiTimerMini *tm, void *ptr,
    AsciiFlag40A *f40a, AsciiFlag20A *f20a, AsciiFlag30 *f30,
    AsciiFlag40B *f40b, AsciiFlag20B *f20b, AsciiFlag20R *f20r,
    AsciiRet0 *r0, AsciiRet1 *r1, StageFlag7C *sf7c, StageFlag80 *sf80)
{
    int acc = 0;
    acc += (int)asc->AddFormattedInt(val, pos, outId);
    acc += (int)asc->AddFormattedStr(outId, pos, "%s %d", str, val);
    acc += asc->DrawLayer(val);
    acc += (int)AsciiMeasureStringWidth(str, val, fw);
    acc += asc->DrawLayer1();
    acc += asc->DrawLayer1();
    acc += asc->DrawLayer2();

    OnDrawFn pDraw0 = &AsciiInfCore::OnDraw0;
    acc += (asc->*pDraw0)();
    OnDrawFn pDraw2 = &AsciiInfCore::OnDraw2;
    acc += (asc->*pDraw2)();

    asc->DrawFormattedNow(pos, "%s %d", str, val);
    asc->DrawIntNow(val, pos);
    asc->UpdatePopups();

    sup->SetViewport(val);
    asc->SetUnk1C878_7C(val, val + 1);
    asc->ResetUnk1C88C();
    AsciiNoOp403050();
    AsciiFreePtr(ptr);

    vm->FreeExtraInternal();
    FreeExtraFn pFree = &AsciiAnmVmCore::FreeExtraThiscall;
    (vm->*pFree)();

    Ret0Fn pR0 = &AsciiRet0::Self;
    acc += (int)(r0->*pR0)();
    f40a->Clear();
    AsciiNoOp4030E0();
    f20a->Clear();
    f30->Clear();
    AsciiNoOp403110();
    f40b->Clear();
    f20b->Clear();
    Flag20RFn pF20r = &AsciiFlag20R::Clear;
    acc += (int)(f20r->*pF20r)();
    AsciiNoOp403150();
    Ret1Fn pR1 = &AsciiRet1::Self;
    acc += (int)(r1->*pR1)();
    AsciiNoOp403190();

    tm->Reset();
    bg->Cleanup();
    acc += (int)sf7c->GetBit0();
    sf80->ClearBit0();
    acc += (int)StageBgMini::Destroy(bg);
    return acc;
}
