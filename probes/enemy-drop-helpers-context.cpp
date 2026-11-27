#include "../src/EnemyDropHelpers.hpp"
#include <new>

char g_DropPathBuf[260];
DropEnemyMgr *g_DropEnemyMgr;
Drop4B8A80 *g_Drop4B8A80;
DropGuiMini *g_DropGui;
Drop4B8934 *g_Drop4B8934;

typedef int (DropEclFile::*LoadFn)(const char *);
typedef void (__stdcall *SetFlag7CFn)(int);

extern "C" int __stdcall EnemyDropHelpersProbe(
    DropIsPos *ip, DropSet0 *s0, DropGet0 *g0, DropArr *arr,
    DropOff34 *o34, DropOff3E3C *o3e, DropOff5004 *o50, DropOff11C8 *o11,
    DropFlag30 *f30, DropObj17C *o17c, DropDerefOuter *dr,
    DropFlag14 *f14, DropState *st, DropArea *ar, DropEnemyCore *ec,
    DropEclFile *ef, DropGuiMini *gm, DropObj50 *obj50, DropObj5CC *obj5cc,
    DropZero354A *za, DropZero354B *zb, DropZero354C *zc, DropZero374 *z374,
    const DropVec3 *v3, const char *name, int a, int b, float fa, float fb)
{
    int acc = 0;
    acc += ip->IsPositive() + g0->Get() + (int)o34->Get() + (int)o3e->Get() + (int)o50->Get() + (int)o11->Get();
    acc += o17c->Get() + dr->Get();
    s0->Set(a); arr->Set(a, b); f30->ClearBit0(); f14->Reset(); st->Commit();
    ar->SetRadius(fa, fb); ar->Reset(); ar->ClearCounts();
    new (ec) DropEnemyCore();
    LoadFn pLoad = &DropEclFile::Load;
    acc += (ef->*pLoad)(name);

    DropEnemyMgr::SetArr80(a, b); DropEnemyMgr::SetArr5C(a, b);
    SetFlag7CFn pF7C = &DropEnemyMgr::SetFlag7CBit0;
    pF7C(a);
    acc += DropEnemyMgr::HasEnemyId(a) + (int)DropEnemyMgr::FindEnemyById(a);
    acc += DropEnemyMgr::GetLastId() + DropEnemyMgr::GetNextId() + DropEnemyMgr::AllocId();
    acc += Drop4B8A80::GetF78();

    DropGuiMini::SetEntry(a, b, fa, b);
    acc += gm->HasActiveObj() + DropGuiMini::GetActiveObjField17C();
    DropGuiMini::SetTimerPair(a, b);
    acc += Drop4B8934::IsFlag0And6() + Drop4B8934::GetFlag4();
    Drop4B8934::SetFlag4();

    obj50->SetVec3(v3); obj50->SetF68(fa); obj50->SetF70(fa); obj50->SetF74(fa); obj50->Clear80();
    new (za) DropZero354A(); new (zb) DropZero354B(); new (zc) DropZero354C(); new (z374) DropZero374();
    obj5cc->SetVec3(v3); obj5cc->SetF5DC(fa);
    return acc;
}
