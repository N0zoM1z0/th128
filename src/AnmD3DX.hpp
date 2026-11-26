#pragma once

#include <d3d9.h>

struct _D3DXIMAGE_INFO;

extern "C"
{
__declspec(dllimport) HRESULT WINAPI D3DXCreateTexture(
    IDirect3DDevice9 *device, UINT width, UINT height, UINT levels,
    DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **texture);

__declspec(dllimport) HRESULT WINAPI D3DXCreateTextureFromFileInMemoryEx(
    IDirect3DDevice9 *device, const void *data, UINT size,
    UINT width, UINT height, UINT levels, DWORD usage, D3DFORMAT format,
    D3DPOOL pool, DWORD filter, DWORD mipFilter, D3DCOLOR colorKey,
    _D3DXIMAGE_INFO *info, PALETTEENTRY *palette, IDirect3DTexture9 **texture);

__declspec(dllimport) HRESULT WINAPI D3DXLoadSurfaceFromSurface(
    IDirect3DSurface9 *destination, const PALETTEENTRY *destinationPalette,
    const RECT *destinationRect, IDirect3DSurface9 *source,
    const PALETTEENTRY *sourcePalette, const RECT *sourceRect,
    DWORD filter, D3DCOLOR colorKey);

__declspec(dllimport) HRESULT WINAPI D3DXLoadSurfaceFromMemory(
    IDirect3DSurface9 *destination, const PALETTEENTRY *destinationPalette,
    const RECT *destinationRect, const void *source, D3DFORMAT sourceFormat,
    UINT sourcePitch, const PALETTEENTRY *sourcePalette, const RECT *sourceRect,
    DWORD filter, D3DCOLOR colorKey);
}
