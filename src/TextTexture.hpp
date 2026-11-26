#pragma once

#include <windows.h>
#include <d3d9.h>
#include <stddef.h>

// Partial view of the contiguous scratch-image state at 0x004B4ED0.
// The GDI initialization and destruction owners remain unreconstructed.
struct TextImageState
{
    D3DFORMAT format;
    unsigned int width;
    unsigned int height;
    int bytes;
    int pitch;
    HDC dc;
    unsigned char unknown018[8];
    void *pixels;
};

static_assert(offsetof(TextImageState, pixels) == 0x20, "text-image pixels offset");
static_assert(sizeof(TextImageState) == 0x24, "reviewed text-image view size");

extern TextImageState g_TextImage;

extern HFONT g_TextFontDefault;
extern HFONT g_TextFont1;
extern HFONT g_TextFont2;
extern HFONT g_TextFont3;
extern HFONT g_TextFont4;
extern HFONT g_TextFont5;
extern HFONT g_TextFont6;
extern HFONT g_TextFont7;

bool __stdcall InvertTextImageAlpha(int rows);
bool __stdcall FillTransparentTextColors(unsigned int rows);

void __fastcall DrawTextToTexture(void *texture, const char *text, const RECT *rect,
    int x, int fontWidth, COLORREF foreground, COLORREF background, int font, int spacing);
void __fastcall DrawAlternateTextToTexture(void *texture, const char *text, const RECT *rect,
    int x, int fontWidth, COLORREF foreground, int font, int spacing);
