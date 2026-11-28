#include "../src/EnemyGetFloatVar.hpp"
#include <windows.h>

namespace th128 {

EgfRng g_EgfRng;
EgfRng g_EgfRng2;
EgfPlayer *g_EgfPlayer;
EgfEnemyManager *g_EgfEnemyManager;
EgfSpellCard g_EgfSpellCard;
EgfSpellCard g_EgfSpellCard2;
int g_EgfDifficulty;
int g_EgfField4B4CF4;
int g_EgfField4B4CF8;
int g_EgfField4B4D30;
int g_EgfField4D33CC;
volatile int g_EgfSink;

__declspec(noinline) unsigned int EgfRng::GetU32() {
    volatile int x = *(int *)this;
    Sleep(1);
    g_EgfSink += x + *(int *)this;
    return (unsigned int)g_EgfSink;
}

__declspec(noinline) float EgfRng::GetFloat() {
    volatile int x = *(int *)this;
    Sleep(1);
    g_EgfSink += x + *(int *)this;
    return (float)g_EgfSink;
}

__declspec(noinline) float EgfRng::GetFloatSigned() {
    volatile int x = *(int *)this;
    Sleep(1);
    g_EgfSink += x + *(int *)this;
    return (float)g_EgfSink;
}

__declspec(noinline) float EgfVec3::AngleToPlayer() const {
    float r = x + y;
    g_EgfSink += (int)r;
    return r;
}

__declspec(noinline) float EgfVec3::DistanceTo(const EgfVec3 *other) const {
    float r = x + other->x;
    g_EgfSink += (int)r;
    return r;
}

__declspec(noinline) int EgfSpellCard::GetSomething() {
    volatile int x = *(int *)this;
    Sleep(1);
    g_EgfSink += x + *(int *)this;
    return g_EgfSink;
}

} // namespace th128

extern "C" int __stdcall EnemyGetFloatVarProbe(th128::EgfEnemy *e, int varId) {
    float (th128::EgfEnemy::*fn)(int) = &th128::EgfEnemy::GetFloatVar;
    float (th128::EgfVec3::*fn2)() const = &th128::EgfVec3::AngleToPlayer;
    th128::g_EgfSink += *(int *)&fn + *(int *)&fn2;
    th128::g_EgfSink += (int)th128::g_EgfRng2.GetU32();
    th128::g_EgfSink += (int)th128::g_EgfRng2.GetFloat();
    th128::g_EgfSink += (int)th128::g_EgfRng2.GetFloatSigned();
    th128::g_EgfSink += th128::g_EgfSpellCard2.GetSomething();
    float r = e->GetFloatVar(varId);
    r += e->GetFloatVar(varId + 1);
    return (int)r;
}
