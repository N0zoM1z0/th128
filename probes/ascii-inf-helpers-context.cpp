#include "../src/AsciiInfHelpers.hpp"

GameManagerMini *g_GameManagerMini;
AsciiInf *g_AsciiInf;

typedef AsciiEntry *(AsciiEntry::*ClearFlag0Fn)();
typedef int (AsciiInf::*OnTickFn)();
typedef int (AsciiInf::*OnDraw0Fn)();
typedef int (AsciiInf::*OnDraw2Fn)();
typedef AsciiEntry *(AsciiInf::*FindEntryByIdFn)(int);

extern "C" int __stdcall AsciiInfHelpersProbe(
    AsciiInf *asc, AsciiEntry *ent, AsciiAnmVmMini *vm, AsciiAnmResMini *res,
    AsciiSpriteMini *spr, AsciiMatrix4x4 *mat, AsciiListNode *node,
    int *cell, const int *idA, const int *idB, int val, float fx, float fy,
    AsciiZero44A *z44a, AsciiZero34 *z34, AsciiZero44B *z44b, AsciiZero24A *z24a, AsciiZero24B *z24b)
{
    int acc = 0;
    acc += GetGameManagerField4();
    ClearFlag0Fn pClear = &AsciiEntry::ClearFlag0;
    acc += (int)(ent->*pClear)();

    acc += (int)AsciiInf::Create();
    acc += (int)AsciiInf::Destroy(asc);
    AsciiInf::DestroyGlobal();
    acc += asc->OnTickInner();
    OnTickFn pTick = &AsciiInf::OnTick;
    acc += (asc->*pTick)();
    OnDraw0Fn pDraw0 = &AsciiInf::OnDraw0;
    acc += (asc->*pDraw0)();
    OnDraw2Fn pDraw2 = &AsciiInf::OnDraw2;
    acc += (asc->*pDraw2)();

    acc += (int)asc->FindEntryByIdPtr(idA);
    acc += (int)AsciiInf::GetStructSize();
    acc += AsciiIdEquals(idA, idB) ? 1 : 0;
    FindEntryByIdFn pFindId = &AsciiInf::FindEntryById;
    acc += (int)(asc->*pFindId)(val);

    vm->SetByte3F3((unsigned char)val);
    vm->SetInt3F0(val);
    acc += (int)vm->GetSprite();
    acc += (int)res->GetSprite(val);
    vm->BindSprite(spr);
    vm->Reset();

    AsciiSetIntCell(val, cell);
    asc->NoOp1();
    asc->SetColor((unsigned int)val);
    asc->ResetAlign();
    asc->SetScale(fx, fy);
    asc->SetFontId(val);
    asc->ResetShadow();
    asc->SetLayer(val);
    asc->SetBlendMode(val);
    asc->ResetFlags();
    asc->ResetColors();
    asc->ResetStyle();
    asc->NoOp2();
    asc->NoOp3();

    mat->SetIdentity();
    z44a->Zero();
    z34->Zero();
    z44b->Zero();
    z24a->Zero();
    z24b->Zero();
    node->Init(vm);
    return acc;
}
