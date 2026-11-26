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
    // Reviewed external dependency at 0x004645F0; implementation is pending.
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
    int actualFormat = format;
    if ((g_Supervisor.textureFormatFlags & 1) != 0)
    {
        if (g_AnmTextureFormats[format] == D3DFMT_A8R8G8B8 ||
            g_AnmTextureFormats[format] == D3DFMT_UNKNOWN)
        {
            actualFormat = 5;
        }
        else if (g_AnmTextureFormats[format] == D3DFMT_R8G8B8)
        {
            actualFormat = 3;
        }
    }

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
    if ((g_Supervisor.textureFormatFlags & 1) != 0)
    {
        if (g_AnmTextureFormats[format] == D3DFMT_A8R8G8B8 ||
            g_AnmTextureFormats[format] == D3DFMT_UNKNOWN)
        {
            format = 5;
        }
        else if (g_AnmTextureFormats[format] == D3DFMT_R8G8B8)
        {
            format = 3;
        }
    }

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
