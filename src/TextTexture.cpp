#include "TextTexture.hpp"
#include "AnmD3DX.hpp"

#include <string.h>

namespace
{
    struct Pixel8888
    {
        unsigned char blue, green, red, alpha;
    };

    struct Pixel1555
    {
        unsigned short blue : 5;
        unsigned short green : 5;
        unsigned short red : 5;
        unsigned short alpha : 1;
    };

    struct Pixel4444
    {
        unsigned short blue : 4;
        unsigned short green : 4;
        unsigned short red : 4;
        unsigned short alpha : 4;
    };

    inline void AddColor(unsigned int *sums, const Pixel8888 *pixel, unsigned int *count)
    {
        if (pixel->alpha != 0)
        {
            sums[0] += pixel->red;
            sums[1] += pixel->green;
            sums[2] += pixel->blue;
            ++*count;
        }
    }

    inline void AddColor(unsigned int *sums, const Pixel4444 *pixel, unsigned int *count)
    {
        const unsigned short value = *reinterpret_cast<const unsigned short *>(pixel);
        if ((value & 0xF000) != 0)
        {
            sums[0] += (value >> 8) & 15;
            sums[1] += (value >> 4) & 15;
            sums[2] += value & 15;
            ++*count;
        }
    }
}

bool __stdcall FillTransparentTextColors(unsigned int rows)
{
    unsigned int sums[3];
    switch (g_TextImage.format)
    {
    case D3DFMT_A8R8G8B8:
    {
        Pixel8888 *pixel = static_cast<Pixel8888 *>(g_TextImage.pixels);
        for (unsigned int y = 0; y < rows; ++y)
        {
            for (unsigned int x = 0; x < g_TextImage.width; ++x, ++pixel)
            {
                if (pixel->alpha == 0)
                {
                    sums[0] = sums[1] = sums[2] = 0;
                    unsigned int count = 0;
                    if (x > 0)
                        AddColor(sums, pixel - 1, &count);
                    if (x < g_TextImage.width - 1)
                        AddColor(sums, pixel + 1, &count);
                    if (y > 0)
                        AddColor(sums, pixel - g_TextImage.pitch / 4, &count);
                    if (y < g_TextImage.height - 1)
                        AddColor(sums, pixel + g_TextImage.pitch / 4, &count);
                    if (count > 1)
                    {
                        sums[0] /= count;
                        sums[1] /= count;
                        sums[2] /= count;
                    }
                    pixel->red = static_cast<unsigned char>(sums[0]);
                    pixel->green = static_cast<unsigned char>(sums[1]);
                    pixel->blue = static_cast<unsigned char>(sums[2]);
                }
            }
        }
        break;
    }
    case D3DFMT_A4R4G4B4:
    {
        Pixel4444 *pixel = static_cast<Pixel4444 *>(g_TextImage.pixels);
        for (unsigned int y = 0; y < rows; ++y)
        {
            for (unsigned int x = 0; x < g_TextImage.width; ++x, ++pixel)
            {
                if (pixel->alpha == 0)
                {
                    sums[0] = sums[1] = sums[2] = 0;
                    unsigned int count = 0;
                    if (x > 0)
                        AddColor(sums, pixel - 1, &count);
                    if (x < g_TextImage.width - 1)
                        AddColor(sums, pixel + 1, &count);
                    if (y > 0)
                        AddColor(sums, pixel - g_TextImage.pitch / 2, &count);
                    if (y < g_TextImage.height - 1)
                        AddColor(sums, pixel + g_TextImage.pitch / 2, &count);
                    if (count > 1)
                    {
                        sums[0] /= count;
                        sums[1] /= count;
                        sums[2] /= count;
                    }
                    pixel->red = static_cast<unsigned char>(sums[0] >> 1);
                    pixel->green = static_cast<unsigned char>(sums[1] >> 1);
                    pixel->blue = static_cast<unsigned char>(sums[2] >> 1);
                }
            }
        }
        break;
    }
    }
    return true;
}

bool __stdcall InvertTextImageAlpha(int rows)
{
    const int bytes = g_TextImage.pitch * rows;
    unsigned char *pixels = static_cast<unsigned char *>(g_TextImage.pixels);
    switch (g_TextImage.format)
    {
    case D3DFMT_A8R8G8B8:
        for (int i = 3; i < bytes; i += 4)
            pixels[i] = ~pixels[i];
        break;
    case D3DFMT_A1R5G5B5:
    {
        Pixel1555 *pixel = reinterpret_cast<Pixel1555 *>(pixels);
        for (int i = 0; i < bytes; i += 2, ++pixel)
        {
            pixel->alpha = !pixel->alpha;
            if (pixel->alpha == 0)
                pixel->red = pixel->green = pixel->blue = 0;
        }
        break;
    }
    case D3DFMT_A4R4G4B4:
        for (int i = 1; i < bytes; i += 2)
            pixels[i] ^= 0xF0;
        break;
    default:
        return false;
    }
    return true;
}

void __fastcall DrawTextToTexture(void *texture, const char *text, const RECT *rect,
    int x, int fontWidth, COLORREF foreground, COLORREF background, int font, int spacing)
{
    HFONT selectedFont;
    switch (font)
    {
    case 2: selectedFont = g_TextFont2; break;
    case 1: selectedFont = g_TextFont1; break;
    case 3: selectedFont = g_TextFont3; break;
    case 4: selectedFont = g_TextFont4; break;
    case 6: selectedFont = g_TextFont6; break;
    case 5: selectedFont = g_TextFont5; break;
    case 7: selectedFont = g_TextFont7; break;
    default: selectedFont = g_TextFontDefault; break;
    }
    if (fontWidth < 17)
        fontWidth = 17;
    memset(g_TextImage.pixels, 0, g_TextImage.bytes);
    HDC dc = g_TextImage.dc;
    void *object = SelectObject(dc, selectedFont);
    InvertTextImageAlpha(fontWidth * 2 + 6);
    SetBkMode(dc, TRANSPARENT);
    const int length = strlen(text);
    if (spacing == 0)
    {
        SetTextColor(dc, background);
        TextOutA(dc, x * 2 + 2, 4, text, length);
        TextOutA(dc, x * 2 - 2, 4, text, length);
        TextOutA(dc, x * 2 + 2, 0, text, length);
        TextOutA(dc, x * 2 - 2, 0, text, length);
        SetTextColor(dc, foreground);
        TextOutA(dc, x * 2, 2, text, length);
    }
    else
    {
        char pair[3] = { 0, 0, 0 };
        for (int i = 0; i < length; i += 2)
        {
            SetTextColor(dc, background);
            pair[0] = text[i];
            pair[1] = text[i + 1];
            TextOutA(dc, x * 2 + 2, 4, pair, 2);
            TextOutA(dc, x * 2 - 2, 4, pair, 2);
            TextOutA(dc, x * 2 + 2, 0, pair, 2);
            TextOutA(dc, x * 2 - 2, 0, pair, 2);
            SetTextColor(dc, foreground);
            TextOutA(dc, x * 2, 2, pair, 2);
            x += spacing;
        }
    }
    SelectObject(dc, object);
    InvertTextImageAlpha(fontWidth * 2 + 6);
    FillTransparentTextColors(fontWidth * 2 + 6);
    SelectObject(dc, object);
    RECT sourceRect = { 0, 0, (rect->right - rect->left) * 2 + 22, fontWidth * 2 + 2 };
    if (sourceRect.right > 1024)
        sourceRect.right = 1024;
    reinterpret_cast<IDirect3DTexture9 *>(texture)->GetSurfaceLevel(0,
        reinterpret_cast<IDirect3DSurface9 **>(&object));
    D3DXLoadSurfaceFromMemory(static_cast<IDirect3DSurface9 *>(object), 0, rect, g_TextImage.pixels,
        g_TextImage.format, g_TextImage.pitch, 0, &sourceRect, 4, 0);
    if (object != 0)
        static_cast<IDirect3DSurface9 *>(object)->Release();
}

void __fastcall DrawAlternateTextToTexture(void *texture, const char *text, const RECT *rect,
    int x, int fontWidth, COLORREF foreground, int font, int spacing)
{
    HFONT selectedFont;
    switch (font)
    {
    case 2: selectedFont = g_TextFont2; break;
    case 1: selectedFont = g_TextFont1; break;
    case 3: selectedFont = g_TextFont3; break;
    case 4: selectedFont = g_TextFont4; break;
    case 6: selectedFont = g_TextFont6; break;
    case 5: selectedFont = g_TextFont5; break;
    case 7: selectedFont = g_TextFont7; break;
    default: selectedFont = g_TextFontDefault; break;
    }
    if (fontWidth < 17)
        fontWidth = 17;
    const unsigned int color = (((((foreground >> 20) & 15) << 4) | ((foreground >> 12) & 15)) << 4) |
        ((foreground >> 4) & 15);
    unsigned char *pixels = static_cast<unsigned char *>(g_TextImage.pixels);
    for (int i = 0; i < g_TextImage.bytes; i += 2)
        *reinterpret_cast<unsigned short *>(pixels + i) = static_cast<unsigned short>(color);
    HDC dc = g_TextImage.dc;
    void *object = SelectObject(dc, selectedFont);
    InvertTextImageAlpha(fontWidth * 2 + 6);
    SetBkMode(dc, TRANSPARENT);
    const int length = strlen(text);
    if (spacing == 0)
    {
        SetTextColor(dc, foreground);
        TextOutA(dc, x * 2, 2, text, length);
    }
    else
    {
        char pair[3] = { 0, 0, 0 };
        for (int i = 0; i < length; i += 2)
        {
            SetTextColor(dc, foreground);
            pair[0] = text[i];
            pair[1] = text[i + 1];
            TextOutA(dc, x * 2, 2, pair, 2);
            x += spacing;
        }
    }
    SelectObject(dc, object);
    InvertTextImageAlpha(fontWidth * 2 + 6);
    FillTransparentTextColors(fontWidth * 2 + 6);
    SelectObject(dc, object);
    RECT sourceRect = { 0, 0, (rect->right - rect->left) * 2 + 22, fontWidth * 2 + 2 };
    if (sourceRect.right > 1024)
        sourceRect.right = 1024;
    reinterpret_cast<IDirect3DTexture9 *>(texture)->GetSurfaceLevel(0,
        reinterpret_cast<IDirect3DSurface9 **>(&object));
    D3DXLoadSurfaceFromMemory(static_cast<IDirect3DSurface9 *>(object), 0, rect, g_TextImage.pixels,
        g_TextImage.format, g_TextImage.pitch, 0, &sourceRect, 4, 0);
    if (object != 0)
        static_cast<IDirect3DSurface9 *>(object)->Release();
}
