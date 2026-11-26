#include "AnmViewport.hpp"
#include "AnmManager.hpp"
#include "Supervisor.hpp"

#include <math.h>

// ABI declarations for the two imported D3DX9_42 matrix functions. Both
// matrix/vector arguments have the SDK's contiguous float representation.
extern "C"
{
    __declspec(dllimport) D3DMATRIX *WINAPI D3DXMatrixLookAtLH(
        D3DMATRIX *, const ZunVec3 *, const ZunVec3 *, const ZunVec3 *);
    __declspec(dllimport) D3DMATRIX *WINAPI D3DXMatrixPerspectiveFovLH(
        D3DMATRIX *, float, float, float, float);
}

float __stdcall AnmViewportTan(float angle)
{
    return (float)tan((double)angle);
}

void AnmSpriteViewport::Set2DView()
{
    if (g_AnmManager != 0)
        g_AnmManager->FlushSpriteBatch();

    const float width = (float)viewport.Width;
    const float halfWidth = (float)(width * 0.5);
    const float height = (float)viewport.Height;
    const float halfHeight = (float)(height * 0.5);
    const float aspect = width / height;
    ZunVec3 eye = { halfWidth, halfHeight,
        halfHeight / AnmViewportTan(0.15707963705062866f) };
    ZunVec3 target = { halfWidth, halfHeight, 0.0f };
    ZunVec3 up = { 0.0f, -1.0f, 0.0f };
    D3DXMatrixLookAtLH(&viewMatrix, &eye, &target, &up);
    D3DXMatrixPerspectiveFovLH(&projectionMatrix, 0.3141592741012573f,
        aspect, 1.0f, 10000.0f);
    g_Supervisor.d3dDevice->SetTransform(D3DTS_VIEW, &viewMatrix);
    g_Supervisor.d3dDevice->SetTransform(D3DTS_PROJECTION, &projectionMatrix);
    AnmManager *manager = g_AnmManager;
    if (manager != 0)
    {
        manager->viewOffsetX = viewOffsetX;
        manager->viewOffsetY = viewOffsetY;
    }
}
