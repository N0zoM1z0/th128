#include "../src/AnmManager.hpp"
#include "../src/AnmResource.hpp"
#include "../src/GameErrorContext.hpp"

GameErrorContext g_GameErrorContext;

extern "C" AnmResource *__stdcall AnmLoadProbe(
    AnmManager *m1, AnmManager *m2, GameErrorContext *e1, GameErrorContext *e2,
    int slot, const char *filename)
{
    AnmResource *r1 = m1->LoadAnm(slot, filename);
    AnmResource *r2 = m2->LoadAnm(slot + 1, filename);
    e1->Fatal("%s", filename);
    e2->Fatal("%s", filename);
    return r2 != 0 ? r2 : r1;
}
