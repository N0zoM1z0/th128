#include "AsciiScreenDraw.hpp"

AsciiInf *g_AsciiInf = 0;
float g_ZunTimerDefaultSpeed = 1.0f;

namespace th128 {

AsReplayMgr *g_AsReplayMgr = 0;
AsScoreData *g_AsScoreData = 0;
int g_AsDifficulty = 0;
int g_AsRoute = 0;
const char *g_AsRouteNames[] = { "A1", "A2", "B1", "B2", "C1", "C2", "EX" };
const char *g_AsDiffNames[] = { "Easy", "Normal", "Hard", "Lunatic", "Extra" };
const char *g_AsStageNames[] = { "Stage 1", "Stage 2", "Stage 3", "All" };
const char g_AsKeyboardChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";

volatile int g_AsAnchorSink = 0;

} // namespace th128

using namespace th128;

extern "C" int __stdcall AsciiScreenDrawProbe(
    AsReplayMenu *rmenu,
    AsScoreMenu *smenu,
    AsFreezeHud *fhud,
    AsNameEntry *nentry,
    const ZunVec3 *pos
) {
    volatile void *ptrs[] = {
        (void *)&AsDrawReplayMenu,
        (void *)&AsDrawHighScoreMenu,
        (void *)&AsDrawPlayerDataMenu,
        (void *)&AsDrawFreezeHud,
        (void *)&AsDrawNameEntry,
    };
    int acc = 0;
    acc ^= ((int (__stdcall *)(AsReplayMenu *))ptrs[0])(rmenu);
    acc ^= ((int (__stdcall *)(AsScoreMenu *))ptrs[1])(smenu);
    acc ^= ((int (__stdcall *)(AsScoreMenu *))ptrs[2])(smenu);
    acc ^= ((int (__stdcall *)(AsFreezeHud *))ptrs[3])(fhud);
    ((void (__stdcall *)(AsNameEntry *, ZunVec3))ptrs[4])(nentry, *pos);
    return acc ^ g_AsAnchorSink;
}
