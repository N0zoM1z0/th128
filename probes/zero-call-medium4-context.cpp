#include "../src/ZeroCallMedium4.hpp"

float g_ZunTimerDefaultSpeed = 1.0f;

namespace th128 {

int g_Zc4LzssRoot = 0;
unsigned char g_Zc4LzssBuf[0x2000];
Zc4LzssNode g_Zc4LzssTree[0x2002];
Zc4Player *g_Zc4Player = 0;
Zc4Gui *g_Zc4Gui = 0;
Zc4CardInf *g_Zc4CardInf = 0;
Zc4Gate *g_Zc4Gate = 0;
Zc4EnemyMgr *g_Zc4EnemyMgr = 0;

} // namespace th128

using namespace th128;

typedef int (Zc4LaserLine::*LaserInitFn)(const Zc4LaserDesc *);
typedef int (Zc4Enemy::*EnemyCheckFn)();

static __declspec(noinline) int __stdcall CallLaserHitHelper(const ZunVec3 *laserPos, float a, float w, float l) {
    return Zc4PlayerCheckLaserHit(laserPos, a, w, l);
}

extern "C" int __stdcall ZeroCallMedium4Probe(
    Zc4RouteScore *route,
    int node,
    int *outPos,
    const ZunVec3 *laserPos,
    float a,
    float w,
    float l,
    Zc4LaserLine *laser,
    const Zc4LaserDesc *desc,
    Zc4Enemy *enemy
) {
    LaserInitFn pInit = &Zc4LaserLine::Init;
    EnemyCheckFn pCheck = &Zc4Enemy::CheckSpellInterrupts;
    volatile void *ptrs[] = {
        (void *)&g_Zc4LzssRoot,
        (void *)&g_Zc4LzssTree,
        (void *)&g_Zc4Player,
        (void *)&g_Zc4Gui,
        (void *)&g_Zc4CardInf,
        (void *)&g_Zc4Gate,
        (void *)&g_Zc4EnemyMgr,
        (void *)&CallLaserHitHelper,
    };
    int acc = (int)ptrs[node & 7];
    Zc4InitRouteScores(route);
    acc += Zc4LzssInsertNode(node, outPos);
    acc += ((int (__stdcall *)(const ZunVec3 *, float, float, float))ptrs[7])(laserPos, a, w, l);
    acc += (laser->*pInit)(desc);
    acc += (enemy->*pCheck)();
    return acc;
}
