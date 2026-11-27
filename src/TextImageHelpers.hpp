#pragma once

#include "TextTexture.hpp"
#include "TextImageLifetime.hpp"

struct TextImageFormatRecord
{
    D3DFORMAT format;
    int bitsPerPixel;
    DWORD alphaMask, redMask, greenMask, blueMask;
};

extern TextImageFormatRecord g_TextImageFormats[];

struct TextPixel8888
{
    unsigned char blue, green, red, alpha;
};

struct TextPixel4444
{
    unsigned short blue : 4;
    unsigned short green : 4;
    unsigned short red : 4;
    unsigned short alpha : 4;
};

struct TextImage
{
    unsigned char noise[256];
    D3DFORMAT format;
    unsigned int width;
    unsigned int height;
    int bytes;
    int pitch;
    HDC dc;
    HGDIOBJ previousBitmap;
    HBITMAP bitmap;
    void *pixels;

    bool SetAlpha(unsigned char alpha);
    bool DrawOutlinedText(HFONT font, const char *text, int x, int y, COLORREF foreground, COLORREF outline);
    bool CopyFromSurface(IDirect3DSurface9 *surface);
    bool CopyToSurface(IDirect3DSurface9 *surface);
    bool AlphaBlendToSurface(IDirect3DSurface9 *surface, int x, int y, int w, int h);
};

static_assert(offsetof(TextImage, format) == 0x100, "TextImage format offset");
static_assert(offsetof(TextImage, bitmap) == 0x11C, "TextImage bitmap offset");
static_assert(offsetof(TextImage, pixels) == 0x120, "TextImage pixels offset");

void FillTextImage4444(COLORREF color);
TextImageFormatRecord *FindTextImageFormat(D3DFORMAT format);
void AddTextColor8888(unsigned int *sums, const TextPixel8888 *pixel, unsigned int *count);
void AddTextColor4444(const TextPixel4444 *pixel, unsigned int *sums, unsigned int *count);
IDirect3DTexture9 *CreateOffscreenA1R5G5B5Texture(UINT width, UINT height);
