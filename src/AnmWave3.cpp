#include "AnmWave3.hpp"

#include <stdlib.h>

namespace th128 {

int Aw3AnmManager::DrawFilledCircle(
    float cx, float cy, float radius, float startAngle, int segments, unsigned int innerColor, unsigned int outerColor) {
    Aw3Vertex *v = this->vertexCursor;
    float angle = startAngle;

    v->x = cx;
    v->y = cy;
    v->z = 0.0f;
    v->rhw = 1.0f;
    v->diffuse = innerColor;
    ++v;

    startAngle = (float)(6.2831854820251465 / (double)segments);
    for (int i = 0; i < segments + 1; ++i) {
        v->SetPolar(angle, radius);
        v->x += cx;
        v->y += cy;
        v->z = 0.0f;
        v->rhw = 1.0f;
        v->diffuse = outerColor;
        ++v;
        angle = AddAngles(angle, startAngle);
    }

    g_Aw3AnmManager->FlushAnmSpriteBatch();
    g_Aw3D3dDevice->SetRenderState(0x0e, 0);
    if (g_Aw3AnmManager->alphaOpState != 0) {
        g_Aw3D3dDevice->SetTextureStageState(0, 4, 3);
        g_Aw3D3dDevice->SetTextureStageState(0, 1, 3);
        g_Aw3AnmManager->alphaOpState = 0;
    }
    if (this->colorOpState != 1) {
        g_Aw3D3dDevice->SetTextureStageState(0, 6, 0);
        g_Aw3D3dDevice->SetTextureStageState(0, 3, 0);
        this->colorOpState = 1;
    }
    g_Aw3D3dDevice->SetFVF(0x44);
    g_Aw3D3dDevice->DrawPrimitiveUP(6, segments, this->vertexCursor, 0x14);
    ++this->drawCallCount;
    this->vertexCursor += segments + 2;
    return 0;
}

int Aw3AnmManager::DrawPolyLine(const Aw3Vec2 *points, const unsigned int *colors, int count) {
    Aw3AnmManager *mgr = g_Aw3AnmManager;
    Aw3Vertex *v = mgr->vertexCursor;
    if ((unsigned char *)(v + count + 1) >= (unsigned char *)&mgr->vertexCursor) {
        return 0;
    }
    for (int i = 0; i < count; ++i) {
        v[i].x = points[i].x;
        v[i].y = points[i].y;
        v[i].z = 0.0f;
        v[i].rhw = 1.0f;
        v[i].diffuse = colors[i];
    }
    if (g_Aw3AnmManager->alphaOpState != 0) {
        g_Aw3D3dDevice->SetTextureStageState(0, 4, 3);
        g_Aw3D3dDevice->SetTextureStageState(0, 1, 3);
        g_Aw3AnmManager->alphaOpState = 0;
    }
    if (mgr->colorOpState != 1) {
        g_Aw3D3dDevice->SetTextureStageState(0, 6, 0);
        g_Aw3D3dDevice->SetTextureStageState(0, 3, 0);
        mgr->colorOpState = 1;
    }
    g_Aw3D3dDevice->SetFVF(0x44);
    g_Aw3D3dDevice->DrawPrimitiveUP(3, count - 1, mgr->vertexCursor, 0x14);
    mgr->vertexCursor += count;
    ++mgr->drawCallCount;
    return 0;
}

int Aw3AnmManager::DrawCircleOutline(
    float cx, float cy, float radius, float startAngle, int segments, unsigned int color) {
    Aw3Vertex *v = this->vertexCursor;
    if ((unsigned char *)v + segments * sizeof(Aw3Vertex) + sizeof(Aw3Vertex) >= (unsigned char *)&this->vertexCursor) {
        return 0;
    }
    float angle = startAngle;
    startAngle = (float)(6.2831854820251465 / (double)segments);
    for (int i = 0; i < segments + 1; ++i) {
        v->SetPolar(angle, radius);
        v->x += cx;
        v->y += cy;
        v->z = 0.0f;
        v->rhw = 1.0f;
        v->diffuse = color;
        ++v;
        angle = AddAngles(angle, startAngle);
    }
    if (g_Aw3AnmManager->alphaOpState != 0) {
        g_Aw3D3dDevice->SetTextureStageState(0, 4, 3);
        g_Aw3D3dDevice->SetTextureStageState(0, 1, 3);
        g_Aw3AnmManager->alphaOpState = 0;
    }
    if (this->colorOpState != 1) {
        g_Aw3D3dDevice->SetTextureStageState(0, 6, 0);
        g_Aw3D3dDevice->SetTextureStageState(0, 3, 0);
        this->colorOpState = 1;
    }
    g_Aw3D3dDevice->SetFVF(0x44);
    g_Aw3D3dDevice->DrawPrimitiveUP(3, segments, this->vertexCursor, 0x14);
    this->vertexCursor += segments + 1;
    ++this->drawCallCount;
    return 0;
}

int Aw3AnmManager::DrawRing(
    float cx, float cy, float radius, float thickness, float startAngle, int segments, unsigned int color) {
    Aw3Vertex *v = this->vertexCursor;
    if ((unsigned char *)(v + 2) + segments * (2 * sizeof(Aw3Vertex)) >= (unsigned char *)&this->vertexCursor) {
        return 0;
    }
    float angle = startAngle;
    float halfThick = (float)((double)thickness * 0.5);
    float innerR = radius - halfThick;
    float outerR = radius + halfThick;
    startAngle = (float)(6.2831854820251465 / (double)segments);
    for (int i = 0; i < segments + 1; ++i) {
        v->SetPolar(angle, innerR);
        v->x += cx;
        v->y += cy;
        v->z = 0.0f;
        v->rhw = 1.0f;
        v->diffuse = color;

        (v + 1)->SetPolar(angle, outerR);
        v[1].x += cx;
        v[1].y += cy;
        v[1].z = 0.0f;
        v[1].rhw = 1.0f;
        v[1].diffuse = color;

        v += 2;
        angle = AddAngles(angle, startAngle);
    }
    if (g_Aw3AnmManager->alphaOpState != 0) {
        g_Aw3D3dDevice->SetTextureStageState(0, 4, 3);
        g_Aw3D3dDevice->SetTextureStageState(0, 1, 3);
        g_Aw3AnmManager->alphaOpState = 0;
    }
    if (this->colorOpState != 1) {
        g_Aw3D3dDevice->SetTextureStageState(0, 6, 0);
        g_Aw3D3dDevice->SetTextureStageState(0, 3, 0);
        this->colorOpState = 1;
    }
    g_Aw3D3dDevice->SetFVF(0x44);
    g_Aw3D3dDevice->DrawPrimitiveUP(5, segments * 2, this->vertexCursor, 0x14);
    this->vertexCursor += segments * 2 + 2;
    ++this->drawCallCount;
    return 0;
}

void GetAnmVmDrawPosition(const Aw3AnmVm *vm, Aw3Vec3 *outPos) {
    Aw3Vec3 sum;
    sum.x = vm->offset.x + vm->pos.x;
    sum.y = vm->offset.y + vm->pos.y;
    sum.z = vm->offset.z + vm->pos.z;
    Aw3Vec3 res;
    res.x = vm->shake.x + sum.x;
    res.y = sum.y + vm->shake.y;
    res.z = sum.z + vm->shake.z;
    *outPos = res;
}

} // namespace th128
