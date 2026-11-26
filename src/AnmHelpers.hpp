#ifndef TH128_ANM_HELPERS_HPP
#define TH128_ANM_HELPERS_HPP

#include <windows.h>

#include "AnmManager.hpp"
#include "AnmResource.hpp"

struct AnmD3DSurfaceDesc
{
    DWORD Format;
    DWORD Type;
    DWORD Usage;
    DWORD Pool;
    DWORD MultiSampleType;
    DWORD MultiSampleQuality;
    UINT Width;
    UINT Height;
};

struct AnmD3DLockedRect
{
    INT Pitch;
    void *pBits;
};

struct AnmD3DSurfaceView
{
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                // +0x04
    virtual ULONG __stdcall Release() = 0;                               // +0x08
    virtual HRESULT __stdcall Slot03() = 0;                              // +0x0C
    virtual HRESULT __stdcall Slot04() = 0;                              // +0x10
    virtual HRESULT __stdcall Slot05() = 0;                              // +0x14
    virtual HRESULT __stdcall Slot06() = 0;                              // +0x18
    virtual HRESULT __stdcall Slot07() = 0;                              // +0x1C
    virtual HRESULT __stdcall Slot08() = 0;                              // +0x20
    virtual HRESULT __stdcall Slot09() = 0;                              // +0x24
    virtual HRESULT __stdcall Slot10() = 0;                              // +0x28
    virtual HRESULT __stdcall Slot11() = 0;                              // +0x2C
    virtual HRESULT __stdcall GetDesc(AnmD3DSurfaceDesc *desc) = 0;      // +0x30
    virtual HRESULT __stdcall LockRect(AnmD3DLockedRect *locked, const RECT *rect, DWORD flags) = 0; // +0x34
    virtual HRESULT __stdcall UnlockRect() = 0;                          // +0x38
};

struct AnmD3DTextureSurfaceView
{
    virtual HRESULT __stdcall Slot00() = 0;
    virtual HRESULT __stdcall Slot01() = 0;
    virtual HRESULT __stdcall Slot02() = 0;
    virtual HRESULT __stdcall Slot03() = 0;
    virtual HRESULT __stdcall Slot04() = 0;
    virtual HRESULT __stdcall Slot05() = 0;
    virtual HRESULT __stdcall Slot06() = 0;
    virtual HRESULT __stdcall Slot07() = 0;
    virtual HRESULT __stdcall Slot08() = 0;
    virtual HRESULT __stdcall Slot09() = 0;
    virtual HRESULT __stdcall Slot10() = 0;
    virtual HRESULT __stdcall Slot11() = 0;
    virtual HRESULT __stdcall Slot12() = 0;
    virtual HRESULT __stdcall Slot13() = 0;
    virtual HRESULT __stdcall Slot14() = 0;
    virtual HRESULT __stdcall Slot15() = 0;
    virtual HRESULT __stdcall Slot16() = 0;
    virtual HRESULT __stdcall GetLevelDesc(UINT level, AnmD3DSurfaceDesc *desc) = 0; // +0x44
    virtual HRESULT __stdcall GetSurfaceLevel(UINT level, AnmD3DSurfaceView **out) = 0; // +0x48
};

namespace AnmHelpers
{
void __stdcall SetVmGamePosition(AnmVm *vm, const ZunVec3 *pos);
void __stdcall SetVmGamePosition2(AnmVm *vm, const ZunVec3 *pos);
void __stdcall DrawSingleVm(AnmVm *vm);
void __stdcall SetVmRotX(AnmVm *vm, float v);
void __stdcall SetVmRotY(AnmVm *vm, float v);
void __stdcall SetVmRotZ(AnmVm *vm, float v);
void __stdcall SetVmRotY2(AnmVm *vm, float v);
void __stdcall SetVmRotX2(AnmVm *vm, float v);
void __stdcall SetVmRotZ2(AnmVm *vm, float v);
unsigned int *__stdcall FindChildIdFromVm(unsigned int *outId, AnmVm *vm, int scriptIndex);
AnmVm *__stdcall FindChildVmFromVm(AnmVm *vm, int scriptIndex);
void __stdcall ClearD3DTexture(AnmD3DTextureSurfaceView **tex);
void __stdcall ClearD3DTextureBlack(AnmD3DTextureSurfaceView **tex);

// GetVmById-based helpers (using real g_AnmManager->GetVmById(AnmId))
void __stdcall MarkVmTreeForDeleteById(AnmId id);
void __stdcall SetVmPosById(AnmId id, const ZunVec3 *pos);
void __stdcall SetVmPosById2(AnmId id, const ZunVec3 *pos);
void __stdcall SetVmRotZById(AnmId id, float z);
void __stdcall SetVmGamePosById(AnmId id, const ZunVec3 *pos);
ZunVec3 *__stdcall GetVmPosById(AnmManager *mgr, AnmId id);
AnmVm *__stdcall ResolveVmIdPtr(AnmId *id);
void __stdcall StopVmByIdPtr(AnmId *id, short intr);
void __stdcall SetInterruptAndUpdateByIdPtr(AnmId *id, short intr);
void __stdcall HideVmTreeByIdPtr(AnmId *id);
void __stdcall ShowVmTreeByIdPtr(AnmId *id);
void __stdcall SetVisibleBitByIdPtr(AnmId *id);
void __stdcall ClearVisibleBitByIdPtr(AnmId *id);
void __stdcall DeleteVmByIdPtr(AnmId *id);
void __stdcall SetVmPosByIdPtr(AnmId *id, const ZunVec3 *pos);
void __stdcall SetVmPosByIdPtr2(AnmId *id, const ZunVec3 *pos);
void __stdcall SetVmRotZByIdPtr(AnmId *id, float z);
void __stdcall SetVmGamePosByIdPtr(AnmId *id, const ZunVec3 *pos);
void __stdcall BindVmScriptByIdPtr(AnmId *id, int scriptIndex);
void __stdcall BindVmOwnResource(AnmVm *vm, int scriptIndex);
void __stdcall BindVmResource(AnmVm *vm, AnmResource *res, int scriptIndex);
void __stdcall BindVmResource2(AnmVm *vm, AnmResource *res, int scriptIndex);
void __stdcall BindVmOwnResourceKeepState(AnmVm *vm, int scriptIndex);
void __stdcall BindVmResourceKeepState(AnmVm *vm, AnmResource *res, int scriptIndex);
void __stdcall BindVmResourceByIdPtr(AnmId *id, AnmResource *res, int scriptIndex);
void __stdcall BindVmKeepStateByIdPtr(AnmId *id, int scriptIndex);
unsigned int *__stdcall FindChildIdByIdPtr(unsigned int *outId, AnmId *rootId, int scriptIndex);
AnmVm *__stdcall FindChildVmByIdPtr(AnmId *rootId, int scriptIndex);
void __stdcall SetChildVisibleByScript(AnmId *rootId, int scriptIndex);
void __stdcall ClearChildVisibleByScript(AnmId *rootId, int scriptIndex);
void __stdcall SetVmColorByIdPtr(AnmId *id, unsigned int color);
bool __stdcall AnmVmIdEquals(const AnmId *a, const AnmId *b);
unsigned int __stdcall NextVmId(AnmId *counter);
}

#endif
