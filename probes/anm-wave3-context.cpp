#include "../src/AnmWave3.hpp"

#include <windows.h>

namespace th128 {

Aw3D3dDevice *g_Aw3D3dDevice = 0;
Aw3AnmManager *g_Aw3AnmManager = 0;
Aw3Rng g_Aw3Rng;

static volatile int g_sink = 0;
static volatile float g_fsink = 0.0f;

void Aw3AnmManager::FlushAnmSpriteBatch() {
    g_sink += *(int *)this;
    Sleep(1);
    g_sink += *(int *)this;
}

void Aw3Vertex::SetPolar(float angle, float radius) {
    float *pa = &angle;
    float *pr = &radius;
    this->x = *pa;
    this->y = *pr;
    Sleep(1);
    g_fsink += this->x + this->y;
}

float __stdcall AddAngles(float a, float b) {
    float *pa = &a;
    float *pb = &b;
    Sleep(1);
    return *pa + *pb + g_fsink;
}

unsigned int Aw3Rng::GetU32() {
    g_sink += *(int *)this;
    Sleep(1);
    return (unsigned int)g_sink;
}

} // namespace th128

extern "C" int __stdcall AnmWave3Probe(int a, int b) {
    using namespace th128;
    void (Aw3Vertex::*fpolar)(float, float) = &Aw3Vertex::SetPolar;
    Aw3Vertex dummy;
    (dummy.*fpolar)((float)a, (float)b);

    Aw3AnmManager *m = (Aw3AnmManager *)(intptr_t)a;
    int r = 0;
    r += m->DrawFilledCircle((float)a, (float)b, 10.0f, 0.5f, a, (unsigned int)b, (unsigned int)(a + b));
    r += m->DrawPolyLine((const Aw3Vec2 *)(intptr_t)a, (const unsigned int *)(intptr_t)b, a + 1);
    r += m->DrawCircleOutline((float)a, (float)b, 10.0f, 0.5f, a, (unsigned int)b);
    r += m->DrawRing((float)a, (float)b, 10.0f, 2.0f, 0.5f, a, (unsigned int)b);
    m->FlushAnmSpriteBatch();

    Aw3AnmVm *vm = (Aw3AnmVm *)(intptr_t)b;
    Aw3Vec3 outPos;
    GetAnmVmDrawPosition(vm, &outPos);
    g_fsink = outPos.x + outPos.y + outPos.z + dummy.x;
    return r + g_sink;
}

int main() {
    return AnmWave3Probe(1, 2);
}
