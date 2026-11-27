#include "InterpBatch1.hpp"

namespace th128 {

float g_IbDefaultTimeScale = 1.0f;
static volatile float g_IbSink;

void IbTimer::Increment() {
    prev = cur;
    curF += *scale;
    cur = (int)curF;
    g_IbSink = curF;
}

float __stdcall IbEaseTransition(unsigned int mode, float t) {
    float r = t;
    switch (mode) {
    case 0: r = t * t; break;
    case 1: r = t * t * t; break;
    case 2: r = 1.0f - (1.0f - t) * (1.0f - t); break;
    case 3: r = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); break;
    default: r = t; break;
    }
    g_IbSink = r;
    return r;
}

static IbInterp<IbVec3> g_V3[2];
static IbInterp<IbVec2> g_V2[2];
static IbInterp<IbInt3> g_I3[2];
static IbInterp<float> g_F[2];
static IbInterp<int> g_I[2];

} // namespace th128

extern "C" int __stdcall InterpBatch1Probe(int idx, int mode) {
    float acc = 0.0f;
    if (mode & 1) {
        th128::IbVec3 v = th128::g_V3[idx].Step();
        acc += v.x + v.y + v.z;
    }
    if (mode & 2) {
        th128::IbVec3 v = th128::g_V3[idx ^ 1].Step();
        acc += v.x;
    }
    if (mode & 4) {
        th128::IbVec2 v = th128::g_V2[idx].Step();
        acc += v.x + v.y;
    }
    if (mode & 8) {
        th128::IbVec2 v = th128::g_V2[idx ^ 1].Step();
        acc += v.x;
    }
    if (mode & 16) {
        th128::IbInt3 v = th128::g_I3[idx].Step();
        acc += (float)(v.x + v.y + v.z);
    }
    if (mode & 32) {
        th128::IbInt3 v = th128::g_I3[idx ^ 1].Step();
        acc += (float)v.x;
    }
    if (mode & 64) {
        acc += th128::g_F[idx].Step();
        acc += th128::g_F[idx ^ 1].Step();
    }
    if (mode & 128) {
        acc += (float)th128::g_I[idx].Step();
        acc += (float)th128::g_I[idx ^ 1].Step();
    }
    return (int)acc;
}
