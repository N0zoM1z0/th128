#include "AnmText.hpp"
#include "TextTexture.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void __stdcall RenderAnmSpriteText(int fontWidth, void *texture, const char *text,
    const AnmFileEntry *sprite, int x, COLORREF foreground, COLORREF background,
    int alternate, int font, int spacing)
{
    if (fontWidth <= 0)
        fontWidth = 17;
    else if (fontWidth <= 8)
        return;

    RECT rect = { (int)sprite->left, (int)sprite->top,
        (int)sprite->right, (int)sprite->bottom };
    if (alternate == 0)
        DrawTextToTexture(texture, text, &rect, x, fontWidth, foreground, background, font, spacing);
    else
        DrawAlternateTextToTexture(texture, text, &rect, x, fontWidth, foreground, font, spacing);
}

void __cdecl AnmTextVmView::DrawCenteredText(COLORREF foreground, COLORREF background,
    int font, int spacing, const char *format, ...)
{
    int width = 17;
    if (fontWidth != 0)
        width = fontWidth;
    char buffer[0x80];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    const unsigned int length = strlen(buffer);
    AnmVm *vm = reinterpret_cast<AnmVm *>(this);
    AnmFileEntry *sprite = vm->anmFileEntry;
    const int x = (int)(sprite->spriteWidth - ((width - 1) * length >> 1));
    RenderAnmSpriteText(width, *sprite->texture, buffer, sprite, x,
        foreground, background, (vm->stateFlags >> 4) & 1U, font, spacing);
    vm->flags |= 1U;
}

void __cdecl AnmTextVmView::DrawTextAt(COLORREF foreground, COLORREF background,
    int font, int x, int spacing, const char *format, ...)
{
    const int width = fontWidth;
    char buffer[0x80];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    AnmVm *vm = reinterpret_cast<AnmVm *>(this);
    AnmFileEntry *sprite = vm->anmFileEntry;
    RenderAnmSpriteText(width, *sprite->texture, buffer, sprite, x,
        foreground, background, (vm->stateFlags >> 4) & 1U, font, spacing);
    vm->flags |= 1U;
}

void __cdecl AnmTextVmView::DrawTextAtHalfCenter(COLORREF foreground, COLORREF background,
    int font, int spacing, const char *format, ...)
{
    int width = 17;
    if (fontWidth != 0)
        width = fontWidth;
    char buffer[0x80];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    const unsigned int length = strlen(buffer);
    AnmVm *vm = reinterpret_cast<AnmVm *>(this);
    AnmFileEntry *sprite = vm->anmFileEntry;
    const int x = (int)sprite->spriteWidth / 2 - ((width - 1) * length >> 2);
    RenderAnmSpriteText(width, *sprite->texture, buffer, sprite, x,
        foreground, background, (vm->stateFlags >> 4) & 1U, font, spacing);
    vm->flags |= 1U;
}
