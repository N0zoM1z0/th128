#include "AnmText.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Reviewed external boundaries: ECX texture, EDX text and seven/six stack
// arguments, respectively. Their GDI/D3D bodies are not reconstructed here.
void __fastcall DrawTextToTexture(void *texture, const char *text, const RECT *rect,
    int x, int fontWidth, COLORREF foreground, COLORREF background, int option, int font);
void __fastcall DrawAlternateTextToTexture(void *texture, const char *text, const RECT *rect,
    int x, int fontWidth, COLORREF foreground, int option, int font);

void __stdcall RenderAnmSpriteText(int fontWidth, void *texture, const char *text,
    const AnmFileEntry *sprite, int x, COLORREF foreground, COLORREF background,
    int alternate, int option, int font)
{
    if (fontWidth <= 0)
        fontWidth = 17;
    else if (fontWidth <= 8)
        return;

    RECT rect = { (int)sprite->left, (int)sprite->top,
        (int)sprite->right, (int)sprite->bottom };
    if (alternate == 0)
        DrawTextToTexture(texture, text, &rect, x, fontWidth, foreground, background, option, font);
    else
        DrawAlternateTextToTexture(texture, text, &rect, x, fontWidth, foreground, option, font);
}

void __cdecl AnmTextVmView::DrawCenteredText(COLORREF foreground, COLORREF background,
    int option, int font, const char *format, ...)
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
        foreground, background, (vm->stateFlags >> 4) & 1U, option, font);
    vm->flags |= 1U;
}

void __cdecl AnmTextVmView::DrawTextAt(COLORREF foreground, COLORREF background,
    int option, int x, int font, const char *format, ...)
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
        foreground, background, (vm->stateFlags >> 4) & 1U, option, font);
    vm->flags |= 1U;
}

void __cdecl AnmTextVmView::DrawTextAtHalfCenter(COLORREF foreground, COLORREF background,
    int option, int font, const char *format, ...)
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
        foreground, background, (vm->stateFlags >> 4) & 1U, option, font);
    vm->flags |= 1U;
}
