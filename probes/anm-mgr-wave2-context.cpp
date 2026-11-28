#include "../src/AnmMgrWave2.hpp"

#include <windows.h>

namespace th128 {

Amw2D3dDevice *g_Amw2D3dDevice = 0;
Amw2AnmManager *g_Amw2AnmManager = 0;
Amw2SpriteViewport g_Amw2Viewport0;
Amw2SpriteViewport g_Amw2Viewport1;
Amw2SpriteViewport g_Amw2Viewport2;
Amw2SpriteViewport *g_Amw2CurrentViewport = 0;
int g_Amw2CurrentViewportId = 0;
int g_Amw2FogEnabled = 0;
unsigned int g_Amw2SupervisorFlags = 0;
CRITICAL_SECTION g_Amw2CritSec;
unsigned char g_Amw2CritSecDepth = 0;

static volatile int g_sink = 0;
static volatile double g_dsink = 0.0;

void Amw2SpriteViewport::Set2DView() {
    int *p1 = (int *)g_Amw2D3dDevice;
    g_sink += *p1 + *(int *)this;
    Sleep(1);
    g_sink += *p1 + *(int *)this;
}

void Amw2AnmManager::FlushAnmSpriteBatch() {
    g_sink += *(int *)this;
    Sleep(1);
    g_sink += *(int *)this;
}

void Amw2AnmManager::CleanupVm(Amw2AnmVm *vm) {
    g_sink += *(int *)this + *(int *)vm;
    Sleep(1);
    g_sink += *(int *)this + *(int *)vm;
}

Amw2AnmVm *Amw2AnmManager::GetVmById(Amw2VmId id) {
    Amw2VmId *pid = &id;
    int *p1 = (int *)g_Amw2D3dDevice;
    g_sink += *p1 + *(int *)this + pid->value;
    return (Amw2AnmVm *)(intptr_t)g_sink;
}

int Amw2AnmVm::Update() {
    __declspec(align(8)) double d = g_dsink + 1.0;
    g_sink += *(int *)this;
    Sleep(1);
    g_dsink = d;
    return g_sink + *(int *)this;
}

void Amw2AnmVm::Draw(Amw2AnmManager *mgr) {
    Amw2AnmManager **pm = &mgr;
    int t = *(int *)this;
    int m = *(int *)*pm;
    Sleep(1);
    g_sink += t + m;
}

Amw2VmId Amw2AnmVm::RegisterWorldVmAtHead() {
    int *p1 = (int *)g_Amw2D3dDevice;
    int *p2 = (int *)g_Amw2AnmManager;
    g_sink += *p1 + *p2 + *(int *)this;
    Sleep(1);
    g_sink += *p1 + *p2 + *(int *)this;
    return Amw2VmId(g_sink);
}

Amw2VmId Amw2AnmVm::RegisterUiVmAtHead() {
    int *p1 = (int *)g_Amw2D3dDevice;
    int *p2 = (int *)g_Amw2AnmManager;
    g_sink += *p1 + *p2 + *(int *)this;
    Sleep(1);
    g_sink += *p1 + *p2 + *(int *)this;
    return Amw2VmId(g_sink);
}

void Amw2AnmResource::BindVmScript(Amw2AnmVm *vm, int scriptId) {
    int *p1 = (int *)g_Amw2D3dDevice;
    g_sink += *p1 + *(int *)this + *(int *)vm + scriptId;
    Sleep(1);
    g_sink += *p1 + *(int *)this + *(int *)vm + scriptId;
}

void Amw2AnmResource::BindVmAtPosition(Amw2AnmVm *vm, int scriptId) {
    int *p1 = (int *)g_Amw2D3dDevice;
    int *ps = &scriptId;
    g_sink += *p1 + *(int *)this + *(int *)vm + *ps;
    Sleep(1);
    g_sink += *p1 + *(int *)this + *(int *)vm + *ps;
}

Amw2AnmVm *AllocateAnmVm() {
    Sleep(1);
    return (Amw2AnmVm *)(intptr_t)g_sink;
}

} // namespace th128

extern "C" int __stdcall AnmMgrWave2Probe(int a, int b) {
    using namespace th128;
    Amw2AnmManager *m = (Amw2AnmManager *)(intptr_t)a;
    int (Amw2AnmManager::*f4)() = &Amw2AnmManager::DrawLayer4;
    int (Amw2AnmManager::*f21)() = &Amw2AnmManager::DrawLayer21;
    int (Amw2AnmManager::*f24)() = &Amw2AnmManager::DrawLayer24;
    int (Amw2AnmManager::*f32)() = &Amw2AnmManager::DrawLayer32;
    int (Amw2AnmVm::*fupd)() = &Amw2AnmVm::Update;

    int r = 0;
    r += (m->*f4)();
    r += (m->*f21)();
    r += (m->*f24)();
    r += (m->*f32)();
    m->FlushAnmSpriteBatch();
    r += m->UpdateWorldVms();
    r += m->DrawVmGroup(b);

    Amw2AnmVm *vm = (Amw2AnmVm *)(intptr_t)(b + 4);
    r += (vm->*fupd)();
    Amw2VmId id0(a);
    r += (int)(intptr_t)m->GetVmById(id0);

    Amw2AnmResource *res = (Amw2AnmResource *)(intptr_t)b;
    const Amw2Vec3 *pos = (const Amw2Vec3 *)(intptr_t)(a + 4);
    Amw2VmId id1 = res->CreateVm(pos, a, b, (unsigned int)(a + b));
    Amw2VmId id2 = res->SpawnChildVm(a, vm, (unsigned int)(a ^ b));
    SetInterruptAndUpdate(id1, (short)b);
    SetInterruptAndUpdate(id2, (short)a);
    return r + id1.value + id2.value;
}

int main() {
    return AnmMgrWave2Probe(1, 2);
}
