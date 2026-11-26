#include "AnmResource.hpp"
#include "AnmD3DX.hpp"
#include "Supervisor.hpp"
#include "Chain.hpp"

// Shared format indices used by archive entries and embedded texture headers.
extern const D3DFORMAT g_AnmTextureFormats[9] = {
    D3DFMT_UNKNOWN, D3DFMT_A8R8G8B8, D3DFMT_A1R5G5B5,
    D3DFMT_R5G6B5, D3DFMT_R8G8B8, D3DFMT_A4R4G4B4,
    D3DFMT_A8R3G3B2, D3DFMT_A8, D3DFMT_R3G3B2
};

extern const int g_AnmTextureBytesPerPixel[9] = { 4, 4, 2, 2, 3, 2, 2, 1, 1 };

namespace AnmHelpers
{
    void __stdcall FillTransparentTextureColors(AnmTextureEntry *entry);
}

namespace
{
    template <typename T>
    inline void ReleaseTextureObject(T *&object)
    {
        if (object != 0)
        {
            object->Release();
            object = 0;
        }
    }
}

namespace AnmHelpers
{
    struct TextureColor8888
    {
        unsigned char blue, green, red, alpha;
    };

    struct TextureColor1555
    {
        unsigned short blue : 5;
        unsigned short green : 5;
        unsigned short red : 5;
        unsigned short alpha : 1;
    };

    struct TextureColor4444
    {
        unsigned short blue : 4;
        unsigned short green : 4;
        unsigned short red : 4;
        unsigned short alpha : 4;
    };

    struct TextureColor8332
    {
        unsigned short blue : 2;
        unsigned short green : 3;
        unsigned short red : 3;
        unsigned short alpha : 8;
    };

inline int __stdcall NormalizeTextureFormat(int format)
{
    if ((g_Supervisor.textureFormatFlags & 1) != 0)
    {
        D3DFORMAT value = g_AnmTextureFormats[format];
        if (value == D3DFMT_A8R8G8B8 || value == D3DFMT_UNKNOWN)
            return 5;
        if (value == D3DFMT_R8G8B8)
            return 3;
    }
    return format;
}

inline void __stdcall AccumulateTextureColor8888(
    unsigned int *sums, const TextureColor8888 *pixel, unsigned int *count)
{
    if (pixel->alpha != 0)
    {
        sums[0] += pixel->red;
        sums[1] += pixel->green;
        sums[2] += pixel->blue;
        ++*count;
    }
}

inline void __stdcall AccumulateTextureColor1555(
    unsigned int *sums, const unsigned short *pixel, unsigned int *count)
{
    unsigned short value = *pixel;
    if ((value & 0x8000) != 0)
    {
        sums[0] += (value >> 10) & 31;
        sums[1] += (value >> 5) & 31;
        sums[2] += value & 31;
        ++*count;
    }
}

inline void __stdcall AccumulateTextureColor4444(
    unsigned int *sums, const unsigned short *pixel, unsigned int *count)
{
    unsigned short value = *pixel;
    if ((value & 0xF000) != 0)
    {
        sums[0] += (value >> 8) & 15;
        sums[1] += (value >> 4) & 15;
        sums[2] += value & 15;
        ++*count;
    }
}

inline void __stdcall AccumulateTextureColor8332(
    unsigned int *sums, const TextureColor8332 *pixel, unsigned int *count)
{
    if (pixel->alpha != 0)
    {
        unsigned short value = *reinterpret_cast<const unsigned short *>(pixel);
        sums[0] += (value >> 5) & 7;
        sums[1] += (value >> 2) & 7;
        sums[2] += value & 3;
        ++*count;
    }
}
}

void __stdcall AnmHelpers::FillTransparentTextureColors(AnmTextureEntry *entry)
{
    IDirect3DSurface9 *surface = 0;
    reinterpret_cast<IDirect3DTexture9 *>(entry->texture)->GetSurfaceLevel(0, &surface);
    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);
    D3DLOCKED_RECT locked;
    surface->LockRect(&locked, 0, 0);
    unsigned int sums[3];
    switch (desc.Format)
    {
    case D3DFMT_UNKNOWN:
    case D3DFMT_A8R8G8B8:
    {
        for (unsigned int y = 0; y < desc.Height; ++y)
        {
            TextureColor8888 *pixel = reinterpret_cast<TextureColor8888 *>(
                static_cast<unsigned char *>(locked.pBits) + y * locked.Pitch);
            for (unsigned int x = 0; x < desc.Width; ++x, ++pixel)
            {
                if (pixel->alpha == 0)
                {
                    sums[0] = sums[1] = sums[2] = 0;
                    unsigned int count = 0;
                    if (x > 0)
                        AnmHelpers::AccumulateTextureColor8888(sums, pixel - 1, &count);
                    if (x < desc.Width - 1)
                        AnmHelpers::AccumulateTextureColor8888(sums, pixel + 1, &count);
                    if (y > 0)
                        AnmHelpers::AccumulateTextureColor8888(sums,
                            pixel - locked.Pitch / 4, &count);
                    if (y < desc.Height - 1)
                        AnmHelpers::AccumulateTextureColor8888(sums,
                            pixel + locked.Pitch / 4, &count);
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
    case D3DFMT_A1R5G5B5:
    {
        for (unsigned int y = 0; y < desc.Height; ++y)
        {
            TextureColor1555 *pixel = reinterpret_cast<TextureColor1555 *>(
                static_cast<unsigned char *>(locked.pBits) + y * locked.Pitch);
            for (unsigned int x = 0; x < desc.Width; ++x, ++pixel)
            {
                if (pixel->alpha == 0)
                {
                    sums[0] = sums[1] = sums[2] = 0;
                    unsigned int count = 0;
                    if (x > 0)
                        AnmHelpers::AccumulateTextureColor1555(sums, reinterpret_cast<const unsigned short *>(pixel - 1), &count);
                    if (x < desc.Width - 1)
                        AnmHelpers::AccumulateTextureColor1555(sums, reinterpret_cast<const unsigned short *>(pixel + 1), &count);
                    if (y > 0)
                        AnmHelpers::AccumulateTextureColor1555(sums,
                            reinterpret_cast<const unsigned short *>(pixel - locked.Pitch / 2), &count);
                    if (y < desc.Height - 1)
                        AnmHelpers::AccumulateTextureColor1555(sums,
                            reinterpret_cast<const unsigned short *>(pixel + locked.Pitch / 2), &count);
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
        for (unsigned int y = 0; y < desc.Height; ++y)
        {
            TextureColor4444 *pixel = reinterpret_cast<TextureColor4444 *>(
                static_cast<unsigned char *>(locked.pBits) + y * locked.Pitch);
            for (unsigned int x = 0; x < desc.Width; ++x, ++pixel)
            {
                if (pixel->alpha == 0)
                {
                    sums[0] = sums[1] = sums[2] = 0;
                    unsigned int count = 0;
                    if (x > 0)
                        AnmHelpers::AccumulateTextureColor4444(sums, reinterpret_cast<const unsigned short *>(pixel - 1), &count);
                    if (x < desc.Width - 1)
                        AnmHelpers::AccumulateTextureColor4444(sums, reinterpret_cast<const unsigned short *>(pixel + 1), &count);
                    if (y > 0)
                        AnmHelpers::AccumulateTextureColor4444(sums,
                            reinterpret_cast<const unsigned short *>(pixel - locked.Pitch / 2), &count);
                    if (y < desc.Height - 1)
                        AnmHelpers::AccumulateTextureColor4444(sums,
                            reinterpret_cast<const unsigned short *>(pixel + locked.Pitch / 2), &count);
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
    case D3DFMT_A8R3G3B2:
    {
        for (unsigned int y = 0; y < desc.Height; ++y)
        {
            TextureColor8332 *pixel = reinterpret_cast<TextureColor8332 *>(
                static_cast<unsigned char *>(locked.pBits) + y * locked.Pitch);
            for (unsigned int x = 0; x < desc.Width; ++x, ++pixel)
            {
                if (pixel->alpha == 0)
                {
                    sums[0] = sums[1] = sums[2] = 0;
                    unsigned int count = 0;
                    if (x > 0)
                        AnmHelpers::AccumulateTextureColor8332(sums, pixel - 1, &count);
                    if (x < desc.Width - 1)
                        AnmHelpers::AccumulateTextureColor8332(sums, pixel + 1, &count);
                    if (y > 0)
                        AnmHelpers::AccumulateTextureColor8332(sums,
                            pixel - locked.Pitch / 2, &count);
                    if (y < desc.Height - 1)
                        AnmHelpers::AccumulateTextureColor8332(sums,
                            pixel + locked.Pitch / 2, &count);
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
    }
    surface->UnlockRect();
    surface->Release();
}

int AnmTextureEntry::CreateEmptyTexture(unsigned int width, unsigned int height, int format)
{
    flags &= ~1U;
    D3DXCreateTexture(g_Supervisor.d3dDevice, width, height, 1, 0,
        g_AnmTextureFormats[format], D3DPOOL_MANAGED,
        reinterpret_cast<IDirect3DTexture9 **>(&texture));
    bytesPerPixel = g_AnmTextureBytesPerPixel[format];
    return width * height * g_AnmTextureBytesPerPixel[format];
}

int AnmTextureEntry::CreateRenderTarget()
{
    flags |= 1U;
    g_Supervisor.d3dDevice->CreateTexture(
        g_Supervisor.backBufferWidth, g_Supervisor.backBufferHeight,
        1, D3DUSAGE_RENDERTARGET,
        static_cast<D3DFORMAT>(g_Supervisor.backBufferFormat),
        D3DPOOL_DEFAULT, reinterpret_cast<IDirect3DTexture9 **>(&texture), 0);
    bytesPerPixel = g_Supervisor.backBufferFormat == D3DFMT_X8R8G8B8 ? 4 : 2;
    return 0;
}

int AnmTextureEntry::LoadExternalTexture(int format, unsigned int width, unsigned int height, int offsetX, int offsetY)
{
    int actualFormat = AnmHelpers::NormalizeTextureFormat(format);

    flags &= ~1U;
    IDirect3DTexture9 *sourceTexture;
    if (D3DXCreateTextureFromFileInMemoryEx(g_Supervisor.d3dDevice,
        rawData, rawDataSize, 0, 0, 0, 0, g_AnmTextureFormats[actualFormat],
        D3DPOOL_MANAGED, 1, 0xFFFFFFFFU, 0, 0, 0, &sourceTexture) != D3D_OK)
    {
        return -1;
    }

    IDirect3DSurface9 *sourceSurface;
    sourceTexture->GetSurfaceLevel(0, &sourceSurface);
    D3DSURFACE_DESC desc;
    sourceSurface->GetDesc(&desc);
    if (desc.Width == static_cast<UINT>(width) && desc.Height == static_cast<UINT>(height))
    {
        texture = sourceTexture;
        ReleaseTextureObject(sourceSurface);
    }
    else
    {
        D3DXCreateTexture(g_Supervisor.d3dDevice, width, height, 0, 0,
            g_AnmTextureFormats[actualFormat], D3DPOOL_MANAGED,
            reinterpret_cast<IDirect3DTexture9 **>(&texture));
        IDirect3DSurface9 *destinationSurface;
        reinterpret_cast<IDirect3DTexture9 *>(texture)->GetSurfaceLevel(0, &destinationSurface);

        int copyWidth = width;
        if (copyWidth + offsetX > static_cast<int>(desc.Width))
        {
            copyWidth = static_cast<int>(desc.Width) - offsetX;
        }
        int copyHeight = height;
        if (copyHeight + offsetY > static_cast<int>(desc.Height))
        {
            copyHeight = static_cast<int>(desc.Height) - offsetY;
        }
        RECT sourceRect;
        sourceRect.left = offsetX;
        sourceRect.top = offsetY;
        sourceRect.right = copyWidth + offsetX;
        sourceRect.bottom = copyHeight + offsetY;
        if (D3DXLoadSurfaceFromSurface(destinationSurface, 0, 0,
            sourceSurface, 0, &sourceRect, 1, 0) != D3D_OK)
        {
            ChainDebugLog("D3DXLoadSurfaceFromSurface error\n", g_AnmTextureFormats[actualFormat]);
        }
        ReleaseTextureObject(sourceSurface);
        ReleaseTextureObject(destinationSurface);
        ReleaseTextureObject(sourceTexture);
    }

    AnmHelpers::FillTransparentTextureColors(this);
    bytesPerPixel = g_AnmTextureBytesPerPixel[actualFormat];
    return g_AnmTextureBytesPerPixel[actualFormat] * width * height;
}

int AnmTextureEntry::LoadEmbeddedTexture(const AnmEmbeddedTexture *data, int format, unsigned int width, unsigned int height)
{
    IDirect3DSurface9 *surface = 0;
    format = AnmHelpers::NormalizeTextureFormat(format);

    flags &= ~1U;
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = data->width;
    rect.bottom = data->height;
    if (D3DXCreateTexture(g_Supervisor.d3dDevice, width, height, 1, 0,
        g_AnmTextureFormats[format], D3DPOOL_MANAGED,
        reinterpret_cast<IDirect3DTexture9 **>(&texture)) != D3D_OK)
    {
        ReleaseTextureObject(surface);
        return -1;
    }

    reinterpret_cast<IDirect3DTexture9 *>(texture)->GetSurfaceLevel(0, &surface);
    D3DXLoadSurfaceFromMemory(surface, 0, &rect, data + 1,
        g_AnmTextureFormats[data->format], data->width * g_AnmTextureBytesPerPixel[data->format],
        0, &rect, 1, 0);
    bytesPerPixel = g_AnmTextureBytesPerPixel[format];
    ReleaseTextureObject(surface);
    return g_AnmTextureBytesPerPixel[format] * width * height;
}
