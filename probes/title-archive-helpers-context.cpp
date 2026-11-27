#include "../src/TitleArchiveHelpers.hpp"
#include <new>

void *TaSub10::g_TaSub10Vtbl;
void TaTitleMgrExact::DummyVtbl() {}
TaStaffMgr *g_TaStaffMgr;
void *g_TaBuf8CVtbl;
void *g_TaTitleVtbl;
TaTitleMgrExact *g_TaTitleMgr;
float g_TaDefaultTimeScale;
TaMother *g_TaMother;
TaScoreMgrExact *g_TaScoreMgr;
TaHelpMgr *g_TaHelpMgr;
unsigned int g_Ta4B4D50;
void *g_TaStreamBaseVtbl;
void *g_TaVtbl850;
unsigned long (__stdcall *g_TaGetModuleFileNameA)(void *, char *, unsigned long);

typedef int (TaObj04::*IsPosFn)();
typedef void (TaVtbl850A::*SetVtblAFn)();

extern "C" int __stdcall TitleArchiveHelpersProbe(
    TaAnmVm *av, TaNode4 *n4, TaStringList *sl, TaStaffMgr *sm, TaBuf8C *b8c,
    TaTitleMgrExact *tm, TaScoreMgrExact *scm, TaObj04 *o04, TaObjD4 *od4,
    TaAnmMgr *am, TaArchive *ar, TaEntryTable *et, TaStreamBase *sb,
    TaVtbl850A *v8a, TaVtbl850B *v8b, const char *cstr, char *buf,
    int *ip, int **ipp, char **cpp, int a, int b, float fa)
{
    int acc = 0;
    av->SetFloat60Flag3(fa);
    av->SetFloat468(fa);
    av->SetFloat464(fa);
    TaNoOp444130();
    acc += n4->IsNextNonZero();
    sl->FreeAll();
    new (sm) TaStaffMgr();
    acc += (int)b8c->DeleteSelf();
    acc += (int)TaStaffMgr::Create();
    acc += (int)sm->DestroySelf();
    TaStaffMgr::DestroyPtr(sm);
    TaNoOp444CB0();
    acc += TaRouteStageToIndex(a, b) + TaGetTitleSize();
    tm->SetStateAndResetTimer(a);
    tm->SetSubStateAndResetTimer(b);
    acc += TaMother::Get528() + scm->IsClearFlag(a) + TaScoreMgrExact::AreAll6Cleared();
    acc += TaScoreMgrExact::GetTable660(a, b);
    TaScoreMgrExact::SetAllClearFlags();
    TaHelpMgr::SetFloat12C(fa);
    new (tm) TaTitleMgrExact();
    TaTitleMgrExact::DestroyGlobal();
    acc += (int)tm->DestroySelf();
    acc += TaRet1_44B3B0() + TaRet1_44B3C0() + TaRet1_44B3D0();
    acc += (int)TaSkipLine(cstr, ip) + (int)TaReadLine(buf, ip, buf);
    acc += o04->GetField04();
    IsPosFn pPos = &TaObj04::IsPos0;
    acc += (o04->*pPos)();
    od4->ZeroD4();
    acc += TaRouteStageToIndexSwitch(a, b);
    TaClearFlag4B4D50Bit4();
    acc += o04->GetPlus7() + am->GetSlot4E90C0(a);
    TaNoOp44F590();
    TaArchive *arArr = new TaArchive[a]; delete[] arArr;
    new (ar) TaArchive();
    acc += et->FindSize(cstr) + (int)et->FindEntry(cstr);
    acc += (int)TaStrDupMalloc(cstr) + TaStreamAdvance4(ipp) + (int)TaStreamSkipAlignedStr(cpp);
    TaNoOp44FD40(); TaNoOp44FD50();
    new (sb) TaStreamBase();
    TaResolvePath(buf, cstr);
    SetVtblAFn pVA = &TaVtbl850A::SetVtblA;
    (v8a->*pVA)();
    v8b->SetVtblB();
    acc += (int)TaStripDirSlash(cstr);
    return acc;
}
