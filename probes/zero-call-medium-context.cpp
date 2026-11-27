#include "ZeroCallMedium.hpp"

namespace th128 {

ZcEnemyMgr *g_ZcEnemyMgr = 0;
ZcGui *g_ZcGui = 0;
ZcSpellCard *g_ZcSpellCard = 0;
ZcStage *g_ZcStage = 0;
ZcAnmMgr *g_ZcAnmMgr = 0;
ZcPlayer *g_ZcPlayer = 0;
float g_ZcGameSpeed = 1.0f;
int g_ZcFreezeMeter = 0;
volatile int g_ZcAnchorSink = 0;

} // namespace th128

using namespace th128;

extern "C" int __stdcall ZeroCallMediumProbe(
    ZcEnemy *enemy,
    int varId,
    ZcGaugeGui *gaugeGui,
    int fullCount,
    int partialState,
    ZcGui *gui,
    ZcLaserHit *laserHit,
    const ZcVec3 *pos,
    float radius,
    ZcLaserBeam *laserBeam,
    const ZcLaserData *laserData
) {
    volatile void *ptrs[] = {
        (void *)&ZcSetGaugeIconsA,
        (void *)&ZcSetGaugeIconsB,
        (void *)&ZcPlayerStopAllOptionAnms,
        (void *)&ZcGuiUpdateMeterAnms,
    };
    int *(ZcEnemy::*volatile pInt)(int) = &ZcEnemy::GetIntVarPtr;
    float *(ZcEnemy::*volatile pFlt)(int) = &ZcEnemy::GetFloatVarPtr;
    int (ZcEnemy::*volatile pUpd)() = &ZcEnemy::UpdateInterrupts;
    int (ZcLaserHit::*volatile pHitA)(const ZcVec3 *, float) = &ZcLaserHit::HitTestPointA;
    int (ZcLaserHit::*volatile pHitB)(const ZcVec3 *, float) = &ZcLaserHit::HitTestPointB;
    int (ZcLaserHit::*volatile pHitC)(const ZcVec3 *, float) = &ZcLaserHit::HitTestPointC;
    int (ZcLaserBeam::*volatile pInit)(const ZcLaserData *) = &ZcLaserBeam::InitFromData;

    int acc = 0;
    acc ^= (int)(enemy->*pInt)(varId);
    acc ^= (int)(enemy->*pFlt)(varId);
    acc ^= (enemy->*pUpd)();
    ((void (__stdcall *)(ZcGaugeGui *, int, int))ptrs[0])(gaugeGui, fullCount, partialState);
    ((void (__stdcall *)(ZcGaugeGui *, int, int))ptrs[1])(gaugeGui, fullCount, partialState);
    ZcPlayerStopAllOptionAnms();
    ((void (__stdcall *)(ZcGui *))ptrs[3])(gui);
    acc ^= (laserHit->*pHitA)(pos, radius);
    acc ^= (laserHit->*pHitB)(pos, radius);
    acc ^= (laserHit->*pHitC)(pos, radius);
    acc ^= (laserBeam->*pInit)(laserData);
    return acc ^ g_ZcAnchorSink;
}
