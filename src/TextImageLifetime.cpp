#include "TextImageLifetime.hpp"
#include "TextTexture.hpp"
#include "Rng.hpp"

#include <string.h>

struct TextImageFormatRecord
{
    D3DFORMAT format;
    int bitsPerPixel;
    DWORD alphaMask, redMask, greenMask, blueMask;
};

// The original mutable table includes its sentinel at 0x004B19F0.
TextImageFormatRecord g_TextImageFormats[] = {
    { D3DFMT_X8R8G8B8, 32, 0, 0xFF0000, 0xFF00, 0xFF },
    { D3DFMT_A8R8G8B8, 32, 0xFF000000, 0xFF0000, 0xFF00, 0xFF },
    { D3DFMT_X1R5G5B5, 16, 0, 0x7C00, 0x3E0, 0x1F },
    { D3DFMT_R5G6B5, 16, 0, 0xF800, 0x7E0, 0x1F },
    { D3DFMT_A1R5G5B5, 16, 0x8000, 0x7C00, 0x3E0, 0x1F },
    { D3DFMT_A4R4G4B4, 16, 0xF000, 0xF00, 0xF0, 0xF },
    { static_cast<D3DFORMAT>(-1), 0, 0, 0, 0, 0 }
};

// Windows ANSI names in the target's CP932 encoding: MS Gothic / MS Mincho
// with the full-width MS prefix used by those installed font families.
extern const char g_TextFontFaceGothic[] =
    "\x82\x6C\x82\x72 \x83\x53\x83\x56\x83\x62\x83\x4E";
extern const char g_TextFontFaceMincho[] =
    "\x82\x6C\x82\x72 \x96\xBE\x92\xA9";

extern unsigned char g_TextImageNoise[256];
extern Rng g_TextImageRng;

namespace
{
    struct TextImageBitmapHandles
    {
        HGDIOBJ previous;
        HBITMAP bitmap;
    };

    inline TextImageBitmapHandles &BitmapHandles()
    {
        return *reinterpret_cast<TextImageBitmapHandles *>(g_TextImage.unknown018);
    }

    inline TextImageFormatRecord *FindTextImageFormat(D3DFORMAT format)
    {
        int index;
        for (index = 0; g_TextImageFormats[index].format != static_cast<D3DFORMAT>(-1); ++index)
        {
            if (g_TextImageFormats[index].format == format)
                break;
        }
        if (format == static_cast<D3DFORMAT>(-1))
            return 0;
        return &g_TextImageFormats[index];
    }
}

bool __stdcall DestroyTextImage()
{
    if (g_TextImage.dc != 0)
    {
        SelectObject(g_TextImage.dc, BitmapHandles().previous);
        DeleteDC(g_TextImage.dc);
        DeleteObject(BitmapHandles().bitmap);
        g_TextImage.width = 0;
        g_TextImage.height = 0;
        g_TextImage.dc = 0;
        BitmapHandles().bitmap = 0;
        BitmapHandles().previous = 0;
        g_TextImage.pixels = 0;
        g_TextImage.format = static_cast<D3DFORMAT>(-1);
        return true;
    }
    return false;
}

bool __stdcall InitializeTextImage(D3DFORMAT format)
{
    DestroyTextImage();
    BITMAPV4HEADER info;
    memset(&info, 0, sizeof(info));
    TextImageFormatRecord *record = FindTextImageFormat(format);
    if (record == 0)
        return false;
    const int pitch = ((1024 * record->bitsPerPixel / 8 + 3) / 4) * 4;
    info.bV4Size = sizeof(info);
    info.bV4Width = 1024;
    info.bV4Height = -(128 + 1);
    info.bV4Planes = 1;
    info.bV4BitCount = static_cast<WORD>(record->bitsPerPixel);
    info.bV4SizeImage = pitch * 128;
    if (format != D3DFMT_X1R5G5B5 && format != D3DFMT_X8R8G8B8)
    {
        info.bV4V4Compression = BI_BITFIELDS;
        info.bV4RedMask = record->redMask;
        info.bV4GreenMask = record->greenMask;
        info.bV4BlueMask = record->blueMask;
        info.bV4AlphaMask = record->alphaMask;
    }
    void *pixels;
    HBITMAP bitmap = CreateDIBSection(0, reinterpret_cast<BITMAPINFO *>(&info),
        DIB_RGB_COLORS, &pixels, 0, 0);
    if (bitmap == 0)
        return false;
    memset(pixels, 0, info.bV4SizeImage);
    HDC dc = CreateCompatibleDC(0);
    HGDIOBJ previous = SelectObject(dc, bitmap);
    BitmapHandles().bitmap = bitmap;
    g_TextImage.pixels = pixels;
    BitmapHandles().previous = previous;
    g_TextImage.pitch = pitch;
    g_TextImage.dc = dc;
    g_TextImage.bytes = info.bV4SizeImage;
    g_TextImage.format = format;
    g_TextImage.width = 1024;
    g_TextImage.height = 128;
    return true;
}

bool __stdcall InitializeDefaultTextImage()
{
    if (InitializeTextImage(D3DFMT_A4R4G4B4))
        return true;
    return InitializeTextImage(D3DFMT_A8R8G8B8);
}

void __stdcall InitializeTextFonts()
{
    if (!InitializeTextImage(D3DFMT_A4R4G4B4))
        InitializeTextImage(D3DFMT_A8R8G8B8);
    for (unsigned int i = 0; i < 256; ++i)
        g_TextImageNoise[i] = static_cast<unsigned char>(g_TextImageRng.GetU16() >> 9);
    g_TextFontDefault = CreateFontA(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, FIXED_PITCH | FF_ROMAN, g_TextFontFaceGothic);
    g_TextFont4 = CreateFontA(40, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, FIXED_PITCH | FF_ROMAN, g_TextFontFaceGothic);
    g_TextFont1 = CreateFontA(32, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, FIXED_PITCH | FF_ROMAN, g_TextFontFaceMincho);
    g_TextFont5 = CreateFontA(40, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, FIXED_PITCH | FF_ROMAN, g_TextFontFaceMincho);
    g_TextFont2 = CreateFontA(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, FIXED_PITCH | FF_ROMAN, g_TextFontFaceGothic);
    g_TextFont3 = CreateFontA(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, FIXED_PITCH | FF_ROMAN, g_TextFontFaceMincho);
    g_TextFont6 = CreateFontA(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, FIXED_PITCH | FF_ROMAN, g_TextFontFaceGothic);
    g_TextFont7 = CreateFontA(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, FIXED_PITCH | FF_ROMAN, g_TextFontFaceMincho);
}

void __stdcall DestroyTextFonts()
{
    DestroyTextImage();
    DeleteObject(g_TextFontDefault);
    DeleteObject(g_TextFont2);
    DeleteObject(g_TextFont4);
    DeleteObject(g_TextFont6);
    DeleteObject(g_TextFont1);
    DeleteObject(g_TextFont3);
    DeleteObject(g_TextFont5);
    DeleteObject(g_TextFont7);
}
