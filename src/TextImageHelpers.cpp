#include "TextImageHelpers.hpp"
#include "AnmD3DX.hpp"

#include <string.h>

extern IDirect3DDevice9 *g_D3DDevice;

extern "C" __declspec(dllimport) HRESULT __stdcall D3DXCreateTexture(
    IDirect3DDevice9 *device, UINT width, UINT height, UINT mipLevels, DWORD usage,
    D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **texture);

void FillTextImage4444(COLORREF color)
{
    const unsigned int packed =
        (((((color >> 20) & 15) << 4) | ((color >> 12) & 15)) << 4) | ((color >> 4) & 15);
    unsigned short *pixel = static_cast<unsigned short *>(g_TextImage.pixels);
    for (int i = 0; i < g_TextImage.bytes; i += 2, ++pixel)
        *pixel = static_cast<unsigned short>(packed);
}

TextImageFormatRecord *FindTextImageFormat(D3DFORMAT format)
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

void AddTextColor8888(unsigned int *sums, const TextPixel8888 *pixel, unsigned int *count)
{
    if (pixel->alpha != 0)
    {
        sums[0] += pixel->red;
        sums[1] += pixel->green;
        sums[2] += pixel->blue;
        ++*count;
    }
}

void AddTextColor4444(const TextPixel4444 *pixel, unsigned int *sums, unsigned int *count)
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

bool TextImage::SetAlpha(unsigned char alpha)
{
    const int totalBytes = this->pitch * static_cast<int>(this->height);
    unsigned char *dst = static_cast<unsigned char *>(this->pixels);
    switch (this->format)
    {
    case D3DFMT_A8R8G8B8:
        for (int i = 2; i < totalBytes; i += 4)
            dst[i] = alpha;
        break;
    case D3DFMT_A1R5G5B5:
    {
        const unsigned char mask = alpha ? 0x80 : 0;
        for (int i = 1; i < totalBytes; i += 2)
            dst[i] = (dst[i] & 0x7F) | mask;
        break;
    }
    case D3DFMT_A4R4G4B4:
    {
        const unsigned char mask = alpha & 0xF0;
        for (int i = 1; i < totalBytes; i += 2)
            dst[i] = (dst[i] & 0x0F) | mask;
        break;
    }
    default:
        return false;
    }
    return true;
}

bool TextImage::DrawOutlinedText(HFONT font, const char *text, int x, int y, COLORREF foreground, COLORREF outline)
{
    if (text == 0)
        return false;
    const int w = static_cast<int>(this->width);
    const int h = static_cast<int>(this->height);
    HDC dcHandle = this->dc;
    HGDIOBJ oldFont = SelectObject(dcHandle, font);
    SetBkMode(dcHandle, TRANSPARENT);
    if (outline != static_cast<COLORREF>(-1))
    {
        SetTextColor(dcHandle, outline);
        RECT rc0 = { x, y, x + w - 2, y + h - 2 };
        DrawTextA(dcHandle, text, -1, &rc0, 0);
        RECT rc1 = { x + 2, y, x + w + 2, y + h };
        RECT rc2 = { x + 1, y + 2, x + w + 1, y + h + 2 };
        RECT rc3 = { x + 2, y, x + w + 2, y + h };
        RECT rc4 = { x, y + 2, x + w, y + h + 2 };
        RECT rc5 = { x + 2, y + 2, x + w + 2, y + h + 2 };
        DrawTextA(dcHandle, text, -1, &rc1, 0);
        DrawTextA(dcHandle, text, -1, &rc2, 0);
        DrawTextA(dcHandle, text, -1, &rc3, 0);
        DrawTextA(dcHandle, text, -1, &rc4, 0);
        DrawTextA(dcHandle, text, -1, &rc5, 0);
    }
    SetTextColor(dcHandle, foreground);
    RECT rcFg = { x + 1, y + 1, x + w + 1, y + h + 1 };
    DrawTextA(dcHandle, text, -1, &rcFg, 0);
    SelectObject(dcHandle, oldFont);
    return true;
}

bool TextImage::CopyFromSurface(IDirect3DSurface9 *surface)
{
    if (this->bitmap == 0)
        return false;
    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);
    RECT rc = { 0, 0, static_cast<LONG>(this->width), static_cast<LONG>(this->height) };
    D3DLOCKED_RECT locked;
    surface->LockRect(&locked, &rc, D3DLOCK_READONLY);
    const int rowBytes = this->pitch;
    const int srcPitch = locked.Pitch;
    unsigned char *dst = static_cast<unsigned char *>(this->pixels);
    unsigned char *src = static_cast<unsigned char *>(locked.pBits);
    if (desc.Format == this->format)
    {
        for (int y = 0; y < static_cast<int>(this->height); ++y)
        {
            memcpy(dst, src, rowBytes);
            src += srcPitch;
            dst += rowBytes;
        }
    }
    surface->UnlockRect();
    return true;
}

bool TextImage::CopyToSurface(IDirect3DSurface9 *surface)
{
    if (this->bitmap == 0)
        return false;
    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);
    RECT rc = { 0, 0, static_cast<LONG>(this->width), static_cast<LONG>(this->height) };
    D3DLOCKED_RECT locked;
    if (surface->LockRect(&locked, &rc, 0) != D3D_OK)
        return false;
    const int dstPitch = locked.Pitch;
    const int rowBytes = this->pitch;
    unsigned char *src = static_cast<unsigned char *>(this->pixels);
    unsigned char *dst = static_cast<unsigned char *>(locked.pBits);
    if (desc.Format == this->format)
    {
        for (int y = 0; y < static_cast<int>(this->height); ++y)
        {
            memcpy(dst, src, rowBytes);
            src += rowBytes;
            dst += dstPitch;
        }
    }
    surface->UnlockRect();
    return true;
}

bool TextImage::AlphaBlendToSurface(IDirect3DSurface9 *surface, int x, int y, int w, int h)
{
    if (this->bitmap == 0)
        return false;
    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);
    RECT rc = { 0, 0, static_cast<LONG>(this->width), static_cast<LONG>(this->height) };
    D3DLOCKED_RECT locked;
    if (surface->LockRect(&locked, &rc, 0) != D3D_OK)
        return false;
    const int srcPitch = this->pitch;
    const int dstPitch = locked.Pitch;
    unsigned char *src = static_cast<unsigned char *>(this->pixels);
    if (desc.Format == this->format)
    {
        switch (this->format)
        {
        case D3DFMT_A4R4G4B4:
        {
            unsigned char *dst = static_cast<unsigned char *>(locked.pBits) + y * dstPitch + x * 2;
            for (int row = 0; row < h; ++row)
            {
                for (int col = 0; col < w; ++col)
                {
                    const int srcA = src[1] >> 4;
                    const int dstG = dst[0] >> 4;
                    const int srcG = src[0] >> 4;
                    dst[0] = static_cast<unsigned char>((dstG + (((srcG - dstG) * srcA) >> 4)) << 4);
                    const int dstB = dst[0] & 0xF;
                    const int srcB = src[0] & 0xF;
                    dst[0] |= static_cast<unsigned char>(dstB + (((srcB - dstB) * srcA) >> 4));
                    const int dstR = dst[1] & 0xF;
                    const int srcR = src[1] & 0xF;
                    int outA = ((dst[1] >> 2) & 0x3C) + srcA;
                    if (outA >= 16)
                        outA = 15;
                    dst[1] = static_cast<unsigned char>((dstR + (((srcR - dstR) * srcA) >> 4)) | (outA << 4));
                    dst += 2;
                    src += 2;
                }
                src += srcPitch - w * 2;
                dst += dstPitch - w * 2;
            }
            break;
        }
        case D3DFMT_A1R5G5B5:
        {
            unsigned short *dst = reinterpret_cast<unsigned short *>(
                static_cast<unsigned char *>(locked.pBits) + y * dstPitch + x * 2);
            unsigned short *src16 = reinterpret_cast<unsigned short *>(src);
            for (int row = 0; row < h; ++row)
            {
                for (int col = 0; col < w; ++col)
                {
                    if (*src16 & 0x8000)
                        *dst = *src16;
                    ++dst;
                    ++src16;
                }
                src16 = reinterpret_cast<unsigned short *>(
                    reinterpret_cast<unsigned char *>(src16) + srcPitch - w * 2);
                dst = reinterpret_cast<unsigned short *>(
                    reinterpret_cast<unsigned char *>(dst) + dstPitch - w * 2);
            }
            break;
        }
        case D3DFMT_A8R8G8B8:
        {
            unsigned char *dst = static_cast<unsigned char *>(locked.pBits) + y * dstPitch + x * 4;
            for (int row = 0; row < h; ++row)
            {
                for (int col = 0; col < w; ++col)
                {
                    const int srcA = src[3];
                    dst[0] = static_cast<unsigned char>(dst[0] + (((src[0] - dst[0]) * srcA) >> 8));
                    dst[1] = static_cast<unsigned char>(dst[1] + (((src[1] - dst[1]) * srcA) >> 8));
                    dst[2] = static_cast<unsigned char>(dst[2] + (((src[2] - dst[2]) * srcA) >> 8));
                    dst += 4;
                    src += 4;
                }
                src += srcPitch - w * 4;
                dst += dstPitch - w * 4;
            }
            break;
        }
        default:
            return false;
        }
    }
    surface->UnlockRect();
    return true;
}

IDirect3DTexture9 *CreateOffscreenA1R5G5B5Texture(UINT width, UINT height)
{
    IDirect3DTexture9 *texture;
    D3DXCreateTexture(g_D3DDevice, width, height, 1, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &texture);
    return texture;
}
