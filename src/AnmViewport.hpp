#pragma once

#include <stddef.h>
#include <d3d9.h>

// Partial camera/viewport rooted at 0x004D3184 for the frame's default view.
struct AnmSpriteViewport
{
    unsigned char unknown000[0x4C];
    D3DMATRIX viewMatrix;                 // +0x04C
    D3DMATRIX projectionMatrix;           // +0x08C
    D3DVIEWPORT9 viewport;                // +0x0CC
    unsigned char unknown0E4[4];
    float viewOffsetX;                    // +0x0E8
    float viewOffsetY;                    // +0x0EC

    void Set2DView();
};

typedef char AnmViewportOffset[(offsetof(AnmSpriteViewport, viewport) == 0xCC) ? 1 : -1];
typedef char AnmViewportViewOffset[(offsetof(AnmSpriteViewport, viewOffsetX) == 0xE8) ? 1 : -1];

extern AnmSpriteViewport *g_AnmSpriteViewport;
float __stdcall AnmViewportTan(float angle);
