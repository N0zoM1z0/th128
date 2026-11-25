// Non-runnable VC10 /GL context for AnmHelpers (0x467480..0x467FF1).
#include "../src/AnmHelpers.hpp"
#include "../src/Supervisor.hpp"

Supervisor g_Supervisor;
AnmManager *g_AnmManager = 0;
AnmManager **g_ProbeAnmMgrAddr = &g_AnmManager;

int __stdcall DrawAnmVmWithManager(AnmVm *vm, AnmManager *manager)
{
    AnmManager *volatile *p = &manager;
    unsigned int x = vm->flags;
    return (int)(x ^ (*p)->nextVmId);
}

int AnmVm::BindAnmFile(AnmResource *resource, int scriptIndex)
{
    volatile int x = scriptIndex + (int)(size_t)resource + (int)id;
    return x;
}

int __stdcall BindAnmFileKeepState(AnmVm *vm, AnmResource *resource, int scriptIndex)
{
    volatile int x = scriptIndex + (int)(size_t)resource + (int)vm->id;
    return x;
}

void (__stdcall *g_ProbeClearTex)(AnmD3DTextureSurfaceView **) = AnmHelpers::ClearD3DTexture;
void (__stdcall *g_ProbeClearTexBlack)(AnmD3DTextureSurfaceView **) = AnmHelpers::ClearD3DTextureBlack;
void (__stdcall *g_ProbeDrawSingle)(AnmVm *) = AnmHelpers::DrawSingleVm;

extern "C" AnmVm *__stdcall AnmHelpersProbe(
    AnmVm *v1, AnmVm *v2, const ZunVec3 *p1, const ZunVec3 *p2,
    AnmId *id1, AnmId *id2, unsigned int *out1, unsigned int *out2,
    int s1, int s2, float f1, float f2, unsigned int c1, unsigned int c2,
    AnmD3DTextureSurfaceView **t1, AnmD3DTextureSurfaceView **t2)
{
    AnmId a1 = { c1 };
    AnmId a2 = { c2 };
    AnmHelpers::SetVmGamePosition(v1, p1);
    AnmHelpers::SetVmGamePosition(v2, p2);
    AnmHelpers::SetVmGamePosition2(v1, p2);
    AnmHelpers::SetVmGamePosition2(v2, p1);
    AnmHelpers::DrawSingleVm(v1);
    AnmHelpers::DrawSingleVm(v2);
    AnmHelpers::SetVmRotX(v1, f1);
    AnmHelpers::SetVmRotX(v2, f2);
    AnmHelpers::SetVmRotY(v1, f1);
    AnmHelpers::SetVmRotY(v2, f2);
    AnmHelpers::SetVmRotZ(v1, f1);
    AnmHelpers::SetVmRotZ(v2, f2);
    AnmHelpers::SetVmRotY2(v1, f1);
    AnmHelpers::SetVmRotY2(v2, f2);
    AnmHelpers::SetVmRotX2(v1, f1);
    AnmHelpers::SetVmRotX2(v2, f2);
    AnmHelpers::SetVmRotZ2(v1, f1);
    AnmHelpers::SetVmRotZ2(v2, f2);
    AnmHelpers::FindChildIdFromVm(out1, v1, s1);
    AnmHelpers::FindChildIdFromVm(out2, v2, s2);
    AnmVm *r = AnmHelpers::FindChildVmFromVm(v1, s1);
    r = AnmHelpers::FindChildVmFromVm(v2, s2);
    AnmHelpers::ClearD3DTexture(t1);
    AnmHelpers::ClearD3DTexture(t2);
    AnmHelpers::ClearD3DTextureBlack(t1);
    AnmHelpers::ClearD3DTextureBlack(t2);

    AnmHelpers::MarkVmTreeForDeleteById(a1);
    AnmHelpers::MarkVmTreeForDeleteById(a2);
    AnmHelpers::SetVmPosById(a1, p1);
    AnmHelpers::SetVmPosById(a2, p2);
    AnmHelpers::SetVmPosById2(a1, p2);
    AnmHelpers::SetVmPosById2(a2, p1);
    AnmHelpers::SetVmRotZById(a1, f1);
    AnmHelpers::SetVmRotZById(a2, f2);
    AnmHelpers::SetVmGamePosById(a1, p1);
    AnmHelpers::SetVmGamePosById(a2, p2);
    if (AnmHelpers::GetVmPosById(g_AnmManager, a1) == AnmHelpers::GetVmPosById((AnmManager *)v1, a2)) *out1 = 1;
    r = AnmHelpers::ResolveVmIdPtr(id1);
    r = AnmHelpers::ResolveVmIdPtr(id2);
    AnmHelpers::StopVmByIdPtr(id1, (short)s1);
    AnmHelpers::StopVmByIdPtr(id2, (short)s2);
    AnmHelpers::SetInterruptAndUpdateByIdPtr(id1, (short)s1);
    AnmHelpers::SetInterruptAndUpdateByIdPtr(id2, (short)s2);
    AnmHelpers::HideVmTreeByIdPtr(id1);
    AnmHelpers::HideVmTreeByIdPtr(id2);
    AnmHelpers::ShowVmTreeByIdPtr(id1);
    AnmHelpers::ShowVmTreeByIdPtr(id2);
    AnmHelpers::SetVisibleBitByIdPtr(id1);
    AnmHelpers::SetVisibleBitByIdPtr(id2);
    AnmHelpers::ClearVisibleBitByIdPtr(id1);
    AnmHelpers::ClearVisibleBitByIdPtr(id2);
    AnmHelpers::DeleteVmByIdPtr(id1);
    AnmHelpers::DeleteVmByIdPtr(id2);
    AnmHelpers::SetVmPosByIdPtr(id1, p1);
    AnmHelpers::SetVmPosByIdPtr(id2, p2);
    AnmHelpers::SetVmPosByIdPtr2(id1, p2);
    AnmHelpers::SetVmPosByIdPtr2(id2, p1);
    AnmHelpers::SetVmRotZByIdPtr(id1, f1);
    AnmHelpers::SetVmRotZByIdPtr(id2, f2);
    AnmHelpers::SetVmGamePosByIdPtr(id1, p1);
    AnmHelpers::SetVmGamePosByIdPtr(id2, p2);
    AnmHelpers::BindVmScriptByIdPtr(id1, s1);
    AnmHelpers::BindVmScriptByIdPtr(id2, s2);
    AnmHelpers::BindVmOwnResource(v1, s1);
    AnmHelpers::BindVmOwnResource(v2, s2);
    AnmHelpers::BindVmResource(v1, v2->resource, s1);
    AnmHelpers::BindVmResource(v2, v1->resource, s2);
    AnmHelpers::BindVmResource2(v1, v2->resource, s1);
    AnmHelpers::BindVmResource2(v2, v1->resource, s2);
    AnmHelpers::BindVmOwnResourceKeepState(v1, s1);
    AnmHelpers::BindVmOwnResourceKeepState(v2, s2);
    AnmHelpers::BindVmResourceKeepState(v1, v2->resource, s1);
    AnmHelpers::BindVmResourceKeepState(v2, v1->resource, s2);
    AnmHelpers::BindVmResourceByIdPtr(id1, v1->resource, s1);
    AnmHelpers::BindVmResourceByIdPtr(id2, v2->resource, s2);
    AnmHelpers::BindVmKeepStateByIdPtr(id1, s1);
    AnmHelpers::BindVmKeepStateByIdPtr(id2, s2);
    AnmHelpers::FindChildIdByIdPtr(out1, id1, s1);
    AnmHelpers::FindChildIdByIdPtr(out2, id2, s2);
    r = AnmHelpers::FindChildVmByIdPtr(id1, s1);
    r = AnmHelpers::FindChildVmByIdPtr(id2, s2);
    AnmHelpers::SetChildVisibleByScript(id1, s1);
    AnmHelpers::SetChildVisibleByScript(id2, s2);
    AnmHelpers::ClearChildVisibleByScript(id1, s1);
    AnmHelpers::ClearChildVisibleByScript(id2, s2);
    AnmHelpers::SetVmColorByIdPtr(id1, c1);
    AnmHelpers::SetVmColorByIdPtr(id2, c2);
    if (AnmHelpers::AnmVmIdEquals(id1, id2) || AnmHelpers::AnmVmIdEquals(id2, id1)) *out2 = AnmHelpers::NextVmId(id1) + AnmHelpers::NextVmId(id2);
    return r;
}
