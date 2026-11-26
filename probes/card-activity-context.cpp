#include "../src/CardInfActions.hpp"
#include "../src/PopupNotifications.hpp"
#include "../src/AnmText.hpp"

extern "C" void __stdcall CardActivityProbe(int cardIndex, const char *name, int durationFrames,
    PopupNotifications *notifications, AnmTextVmView *textVm)
{
    CardInfActions::Start(cardIndex, name, durationFrames);
    CardInfActions::Finish();
    CardInfActions::AddScore(durationFrames);
    notifications->Show(2, 0);
    notifications->Show(3, 0);
    notifications->Show(4, 0);
    textVm->DrawTextAt(0x00FFFFFF, 0, 0, 0, 0, "%s", name);
    textVm->DrawTextAtHalfCenter(0x00FFFFFF, 0, 0, 0, "%s", name);
}
