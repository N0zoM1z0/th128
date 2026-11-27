#pragma once

#include <d3d9.h>
#include "ZunMath.hpp"

extern "C" {
__declspec(dllimport) D3DMATRIX * WINAPI D3DXMatrixTranslation(D3DMATRIX *pOut, float x, float y, float z);
__declspec(dllimport) ZunVec3 * WINAPI D3DXVec3ProjectArray(
    ZunVec3 *pOut, UINT OutStride,
    const ZunVec3 *pV, UINT VStride,
    const D3DVIEWPORT9 *pViewport,
    const D3DMATRIX *pProjection,
    const D3DMATRIX *pView,
    const D3DMATRIX *pWorld,
    UINT n);
}

namespace th128 {

struct Zc5Camera {
    ZunVec3 pos;            // +0x00..+0x0b
    unsigned char pad0c[0x3c - 0x0c];
    ZunVec3 offset;         // +0x3c..+0x47
    int pad48;              // +0x48
    D3DMATRIX view;         // +0x4c..+0x8b
    D3DMATRIX proj;         // +0x8c..+0xcb
    D3DVIEWPORT9 viewport;  // +0xcc..+0xe3
};

struct Zc5BackgroundQuad {
    int pad00;              // +0x00
    ZunVec3 pos;            // +0x04..+0x0f
    ZunVec3 size;           // +0x10..+0x1b

    int IsOffscreen(const ZunVec3 *offset, float maxDistSq, const Zc5Camera *camera);
};

struct Zc5ItemEntry {
    unsigned char pad00[0xa2c];
    int state;              // +0xa2c
    int padA30[2];
    float val;              // +0xa38
    int padA3c;             // +0xa3c (total 0xa40)
};

struct Zc5ItemPool {
    Zc5ItemEntry items[700];
};

struct Zc5PlayerSub {
    int pad00[2];
    float val08;            // +0x08
};

struct Zc5Player {
    unsigned char pad00[0xf7c];
    Zc5PlayerSub *subF7C;   // +0xf7c
};

extern Zc5Player *g_Zc5Player;

void Zc5ClearActiveItems(Zc5ItemPool *pool);
int __stdcall Zc5RotatedRectCircleHit(
    float rectX, float rectY, float rectW, float rectH,
    float angle, float circX, float circY, float circR);
int __stdcall Zc5SegmentSegmentHit(
    float x1, float y1, float x2, float y2,
    float x3, float y3, float x4, float y4);

} // namespace th128
