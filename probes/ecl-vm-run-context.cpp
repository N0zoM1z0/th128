#include "../src/EclVmRun.hpp"

namespace th128 {

__declspec(noinline) EclRunContext::EclRunContext() {
    time = 0.0f;
    curInstr = NULL;
}

__declspec(noinline) int EclRunVm::OnSubHook() {
    return 0;
}

__declspec(noinline) int EclRunVm::GetIntVar(int id) {
    return id;
}

__declspec(noinline) int *EclRunVm::GetIntVarPtr(int) {
    return &mainContext.asyncId;
}

__declspec(noinline) float EclRunVm::GetFloatVar(int id) {
    return (float)id;
}

__declspec(noinline) float *EclRunVm::GetFloatVarPtr(int) {
    return &mainContext.time;
}

__declspec(noinline) float __stdcall EclRunNormalizeAngle(float angle) {
    return angle;
}

__declspec(noinline) float __stdcall EclRunNormalizeAngleDiff(float a, float b) {
    return a - b;
}

__declspec(noinline) void EclRunVec3::FromAngle(float angle, float radius) {
    x = angle * radius;
    y = angle - radius;
}

volatile int g_EclRunSideEffect = 0;

__declspec(noinline) float EclRunInterpScalar(EclRunInterp *interp) {
    g_EclRunSideEffect += interp->duration;
    return interp->start + interp->goal;
}

__declspec(noinline) void EclRunResetInterpTimer(void *interp) {
    ((EclRunInterp *)interp)->timer[0] = 0;
}

__declspec(noinline) void __cdecl EclRunSubNotFoundLog(const char *, ...) {
}

} // namespace th128

using namespace th128;

extern "C" int __stdcall EclVmRunProbe(
    EclRunVm *vm,
    EclRunContext *ctx,
    EclRunResource *res,
    int a,
    float fa,
    const char *name,
    void *buf,
    char tag
) {
    volatile void *ptrs[] = {
        (void *)&EclRunNormalizeAngle,
        (void *)&EclRunNormalizeAngleDiff,
    };
    int acc = 0;
    acc += ctx->Run(fa);
    acc += vm->RunAllContexts(fa);
    acc += ctx->GetIntArg(a);
    acc += ctx->GetIntArg(1) + ctx->GetIntArg(2) + ctx->GetIntArg(3) + ctx->GetIntArg(4) + ctx->GetIntArg(5) + ctx->GetIntArg(6);
    acc += (int)ctx->GetFloatArg(a);
    acc += (int)ctx->GetIntArg0Ptr();
    acc += (int)ctx->GetFloatArgPtr(a);
    acc += ctx->stack.Push(buf, tag);
    acc += (int)res->CreateVm(name);
    acc += vm->JumpToSub(name);
    acc += (int)ptrs[a & 1];
    return acc ^ g_EclRunSideEffect;
}
