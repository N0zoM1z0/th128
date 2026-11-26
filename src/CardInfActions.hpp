#pragma once

namespace CardInfActions
{
void __stdcall Start(int cardIndex, const char *name, int durationFrames);
void Finish();
void __stdcall AddScore(int amount);
}
