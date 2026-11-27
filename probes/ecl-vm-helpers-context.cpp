#include "../src/EclVmHelpers.hpp"
#include <stddef.h>

void __declspec(noinline) __cdecl EclSubNotFoundLog(const char *, ...)
{
}

__declspec(noinline) EclContext::EclContext()
{
    time = 0.0f;
    curInstr = NULL;
}

__declspec(noinline) int EclVm::GetIntVar(int id)
{
    return id;
}

__declspec(noinline) int *EclVm::GetIntVarPtr(int)
{
    return &mainContext.asyncId;
}

__declspec(noinline) float EclVm::GetFloatVar(int id)
{
    return (float)id;
}

__declspec(noinline) float *EclVm::GetFloatVarPtr(int)
{
    return &mainContext.time;
}

__declspec(noinline) int EclContext::Run(float dt)
{
    time += dt;
    return curInstr == NULL ? 1 : 0;
}

typedef int (EclContext::*GetIntVarFn)(int);

extern "C" int __stdcall EclVmHelpersProbe(
    EclVm *vm, EclContext *ctx, EclStack *stk, SptResourceInf *res,
    int a, int b, float fa, float fb, const char *name, void *buf, char tag)
{
    int acc = 0;
    acc += (int)stk->GetLocalPtr(a);
    acc += (int)stk->GetPtr(b);
    acc += stk->GetSp();
    stk->SetSp(a);
    acc += stk->Push(buf, tag);
    acc += stk->Pop(buf, tag);
    acc += stk->AllocFrame(a);
    acc += stk->FreeFrame();

    acc += (int)vm->GetResource();
    vm->SetResource(res);
    vm->SetCurInstr((EclInstr *)buf);
    acc += (int)vm->GetCurInstr();
    vm->SetTime(fa);
    vm->OnSubHook();

    acc += ctx->CallSub(&vm->mainContext, a);
    acc += ctx->GetIntArg(a);
    acc += (int)ctx->GetFloatArg(a);
    GetIntVarFn pGetIntVar = &EclContext::GetIntVar;
    acc += (ctx->*pGetIntVar)(a);
    acc += (int)ctx->GetFloatVar(fa);
    acc += (int)ctx->GetIntArg0Ptr();
    acc += (int)ctx->GetFloatArgPtr(a);
    acc += (int)ctx->GetFloatArg1Ptr((EclInstr *)buf);

    acc += vm->RunAllContexts(fa);
    acc += vm->SpawnAsyncContext(a, b);
    acc += (int)vm->FindAsyncContext(a);
    vm->StopAsyncContexts();
    acc += (int)NormalizeAngleDiff(fa, fb);

    acc += (int)res->FindSub(name);
    acc += (int)res->CreateVm(name);
    acc += vm->BindSub(res, name);
    acc += vm->ResetToSub(res, name);
    acc += vm->JumpToSub(name);
    return acc;
}
