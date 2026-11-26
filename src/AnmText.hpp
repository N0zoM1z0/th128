#pragma once

#include "AnmVm.hpp"
#include "AnmResource.hpp"
#include <windows.h>

// View of the VM's text-specific byte at +0x4D0. The rest is accessed through
// the maintained AnmVm layout rather than extending the common VM interface.
struct AnmTextVmView
{
    unsigned char unknown000[0x4D0];
    unsigned char fontWidth;

    void __cdecl DrawTextAt(COLORREF foreground, COLORREF background,
        int option, int x, int font, const char *format, ...);
    void __cdecl DrawTextAtHalfCenter(COLORREF foreground, COLORREF background,
        int option, int font, const char *format, ...);
    void __cdecl DrawCenteredText(COLORREF foreground, COLORREF background,
        int option, int font, const char *format, ...);
};

void __stdcall RenderAnmSpriteText(int fontWidth, void *texture, const char *text,
    const AnmFileEntry *sprite, int x, COLORREF foreground, COLORREF background,
    int alternate, int option, int font);
