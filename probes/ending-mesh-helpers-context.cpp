#include "../src/EndingMeshHelpers.hpp"
#include <new>

EndAnmMgrMini *g_EndAnmMgr;
EndMgr *g_EndMgr;
EndChainMgrMini *g_EndChainMgr;
unsigned int g_EndSupFlags;
unsigned char g_EndCsObj[0x18];
unsigned char g_EndCsDepth;
int g_End4D2AD4;
int g_EndNextScene;
unsigned int g_EndInputMask;
char g_EndPathBuf[260];
void *g_EndSubVtbl;
void (__stdcall *g_EndEnterCS)(void *);
void (__stdcall *g_EndLeaveCS)(void *);

typedef int (EndVmMini::*MeshUpdFn)();
typedef void (EndVec2::*AngleFn)(float, float);

extern "C" int __stdcall EndingMeshHelpersProbe(
    EndVmMini *vm, EndMesh *mesh, EndVec2 *v2a, const EndVec2 *v2b, const EndVec3 *v3,
    EndListNode *na, EndListNode *nb, EndSub *sub, EndMgr *mgr,
    EndGet0 *g0, EndSet0 *s0, EndGet04 *g04, EndPair *pr, EndClear0 *c0, EndOffset3368 *o3368,
    float *fp1, float *fp2, const char *path, int n, float fa)
{
    int acc = 0;
    MeshUpdFn pUpd = &EndVmMini::OnMeshUpdate;
    acc += (vm->*pUpd)();
    mesh->SyncVerts();
    mesh->CopyToVms();
    mesh->HideAll();
    mesh->ShowAll();
    mesh->SetBlendMode(n);

    acc += g0->Get();
    s0->Set(n);
    v2a->Add(*v2b);
    AngleFn pAng = &EndVec2::FromAngleRadius;
    (v2a->*pAng)(fa, fa);
    acc += g04->Get() + pr->HasChanged();
    vm->SetMode0D(); vm->SetFlag2(); vm->ClearFlag2(); vm->SetPos464(v3);
    EndPosToUv(v2b, fp1, fp2);
    EndCopyFloat(fp1, fp2);
    na->Append(nb);
    c0->Clear();
    EndNoOp412FB0();

    acc += sub->GetFlag1() + sub->GetFlag2();
    sub->DestroySelf();
    acc += (int)EndMgr::LoadFile(path);
    acc += EndMgr::IsFlag0() + mgr->IsFlag1() + (int)o3368->Get();
    new (mgr) EndMgr();
    acc += (int)EndMgr::Create();
    mgr->DestroySelf();
    EndMgr::DestroyGlobal();
    acc += mgr->OnCalc();
    return acc;
}
