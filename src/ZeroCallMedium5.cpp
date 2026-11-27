#include "ZeroCallMedium5.hpp"

#include <math.h>

namespace th128 {

static inline void Zc5MatrixIdentity(D3DMATRIX *m) {
    m->_12 = m->_13 = m->_14 =
    m->_21 = m->_23 = m->_24 =
    m->_31 = m->_32 = m->_34 =
    m->_41 = m->_42 = m->_43 = 0.0f;
    m->_11 = m->_22 = m->_33 = m->_44 = 1.0f;
}

static inline ZunVec3 Zc5VecAdd(const ZunVec3 &a, const ZunVec3 &b) {
    ZunVec3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

static inline ZunVec3 Zc5VecSub(const ZunVec3 &a, const ZunVec3 &b) {
    ZunVec3 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

int Zc5BackgroundQuad::IsOffscreen(const ZunVec3 *offset, float maxDistSq, const Zc5Camera *camera) {
    ZunVec3 worldPos = Zc5VecAdd(pos, *offset);
    ZunVec3 camPos = Zc5VecAdd(camera->pos, camera->offset);
    ZunVec3 delta = Zc5VecSub(worldPos, camPos);

    ZunVec3 pts[16];
    pts[0] = delta;

    float distSq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
    if (distSq > maxDistSq) {
        return 1;
    }

    ZunVec3 h;
    h.x = (float)(size.x * 0.5);
    h.y = (float)(size.y * 0.5);
    h.z = (float)(size.z * 0.5);

    pts[0].x = h.x + pos.x; pts[0].y = h.y + pos.y; pts[0].z = h.z + pos.z;
    pts[1].x = h.x + pos.x; pts[1].y = h.y + pos.y; pts[1].z = pos.z - h.z;
    pts[2].x = h.x + pos.x; pts[2].y = pos.y - h.y; pts[2].z = h.z + pos.z;
    pts[3].x = h.x + pos.x; pts[3].y = pos.y - h.y; pts[3].z = pos.z - h.z;
    pts[4].x = pos.x - h.x; pts[4].y = h.y + pos.y; pts[4].z = h.z + pos.z;
    pts[5].x = pos.x - h.x; pts[5].y = h.y + pos.y; pts[5].z = pos.z - h.z;
    pts[6].x = pos.x - h.x; pts[6].y = pos.y - h.y; pts[6].z = h.z + pos.z;
    pts[7].x = pos.x - h.x; pts[7].y = pos.y - h.y; pts[7].z = pos.z - h.z;

    pts[8].x  = pos.x; pts[8].y  = h.y + pos.y; pts[8].z  = h.z + pos.z;
    pts[9].x  = pos.x; pts[9].y  = h.y + pos.y; pts[9].z  = pos.z - h.z;
    pts[10].x = pos.x; pts[10].y = pos.y - h.y; pts[10].z = h.z + pos.z;
    pts[11].x = pos.x; pts[11].y = pos.y - h.y; pts[11].z = pos.z - h.z;

    pts[12].x = pos.x; pts[12].y = h.y + pos.y; pts[12].z = pos.z;
    pts[13].x = pos.x; pts[13].y = pos.y - h.y; pts[13].z = pos.z;
    pts[14].x = pos.x; pts[14].y = h.y + pos.y; pts[14].z = (float)(pos.z - h.z * 0.5);
    pts[15].x = pos.x; pts[15].y = pos.y - h.y; pts[15].z = (float)(pos.z + h.z * 0.5);

    D3DMATRIX world;
    Zc5MatrixIdentity(&world);
    D3DXMatrixTranslation(&world, offset->x, offset->y, offset->z);

    ZunVec3 projected[16];
    D3DXVec3ProjectArray(
        projected, sizeof(ZunVec3),
        pts, sizeof(ZunVec3),
        &camera->viewport, &camera->proj, &camera->view, &world, 16);

    float minX = 520.0f;
    float minY = 472.0f;
    float maxX = 120.0f;
    float maxY = 8.0f;
    for (int i = 0; i < 16; ++i) {
        if (projected[i].z >= 0.0f && projected[i].z <= 1.0f) {
            if (projected[i].x < minX) {
                minX = projected[i].x;
            }
            if (projected[i].x > maxX) {
                maxX = projected[i].x;
            }
            if (projected[i].y < minY) {
                minY = projected[i].y;
            }
            if (projected[i].y > maxY) {
                maxY = projected[i].y;
            }
        }
    }

    if (maxX >= 128.0f && minX <= 512.0f && maxY >= 16.0f && minY <= 464.0) {
        return 0;
    }
    return 1;
}

void Zc5ClearActiveItems(Zc5ItemPool *pool) {
    Zc5Player *p = g_Zc5Player;
    for (int i = 0; i < 700; ++i) {
        if (pool->items[i].state != 0) {
            if (pool->items[i].state == 1) {
                pool->items[i].val = p->subF7C->val08;
                pool->items[i].state = 3;
            }
        }
    }
}

int __stdcall Zc5RotatedRectCircleHit(
    float rectX, float rectY, float rectW, float rectH,
    float angle, float circX, float circY, float circR
) {
    float dx = circX - rectX;
    float dy = circY - rectY;
    float negAngle = -angle;
    float s = (float)sin(negAngle);
    float c = (float)cos(negAngle);
    float rx = dx * c - dy * s;
    float ry = dx * s + dy * c;

    if (fabs(rx) <= rectW * 0.5 + circR && fabs(ry) <= rectH * 0.5) {
        return 1;
    }
    if (fabs(rx) <= rectW * 0.5 && fabs(ry) <= rectH * 0.5 + circR) {
        return 1;
    }

    float hw = (float)(rectW * 0.5);
    float hh = (float)(rectH * 0.5);
    float r2 = circR * circR;

    float d1x = rx - hw;
    float d1y = ry - hh;
    if (d1x * d1x + d1y * d1y <= r2) {
        return 1;
    }
    float d2x = rx + hw;
    float d2y = ry - hh;
    if (d2x * d2x + d2y * d2y <= r2) {
        return 1;
    }
    float d3x = rx - hw;
    float d3y = ry + hh;
    if (d3x * d3x + d3y * d3y <= r2) {
        return 1;
    }
    float d4x = rx + hw;
    float d4y = ry + hh;
    if (d4x * d4x + d4y * d4y <= r2) {
        return 1;
    }
    return 0;
}

int __stdcall Zc5SegmentSegmentHit(
    float x1, float y1, float x2, float y2,
    float x3, float y3, float x4, float y4
) {
    float ta = (x1 - x2) * (y3 - y1) + (y1 - y2) * (x1 - x3);
    float tb = (x1 - x2) * (y4 - y1) + (y1 - y2) * (x1 - x4);
    if (ta * tb > 0.0f) {
        return 0;
    }
    if (ta == 0.0f && tb == 0.0f) {
        if (x2 < x1) {
            float t = x1; x1 = x2; x2 = t;
            t = y1; y1 = y2; y2 = t;
        }
        if (x4 < x3) {
            float t = x3; x3 = x4; x4 = t;
            t = y3; y3 = y4; y4 = t;
        }
        if (x1 <= x4 && y1 <= y4 && x3 <= x2 && y3 <= y2) {
            return 1;
        }
        return 0;
    }
    float tc = (x3 - x4) * (y1 - y3) + (y3 - y4) * (x3 - x1);
    float td = (x3 - x4) * (y2 - y3) + (y3 - y4) * (x3 - x2);
    if (tc * td > 0.0f) {
        return 0;
    }
    return 1;
}

} // namespace th128
