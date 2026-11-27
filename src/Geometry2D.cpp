#include "Geometry2D.hpp"

#include <math.h>

int __stdcall HitTestPointOrientedRect(
    float rx, float ry, float rw, float rh, float angle, float px, float py)
{
    float dx = px - rx;
    float dy = py - ry;
    float s = sinf(-angle);
    float c = cosf(-angle);
    float lx = dx * c - dy * s;
    if (fabsf(lx) <= rw * 0.5)
    {
        float ly = dx * s + dy * c;
        if (fabsf(ly) <= rh * 0.5)
        {
            return 1;
        }
    }
    return 0;
}

int __stdcall HitTestPointParallelogram(
    float ax, float ay, float bx, float by, float cx, float cy, float px, float py)
{
    float vx = bx - ax;
    float vy = by - ay;
    float wx = cx - bx;
    float wy = cy - by;
    float dx = px - bx;
    float dy = py - by;

    float u = (vx * dx + vy * dy) / (vx * vx + vy * vy);
    float v = (wx * dx + wy * dy) / (wx * wx + wy * wy);
    if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f)
    {
        return 1;
    }
    return 0;
}

int __stdcall HitTestCircleOrientedRect(
    float rx, float ry, float rw, float rh, float angle, float cx, float cy, float radius)
{
    GeoFloat2 d;
    d.x = cx - rx;
    d.y = cy - ry;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float lx = d.x * c - d.y * s;
    float ly = d.x * s + d.y * c;

    float absX = fabsf(lx);
    double hw = rw * 0.5;
    if (absX <= hw + radius && fabsf(ly) <= rh * 0.5)
    {
        return 1;
    }
    if (absX <= hw && fabsf(ly) <= rh * 0.5 + radius)
    {
        return 1;
    }

    float r2 = radius * radius;
    float dx1 = (float)(lx - hw);
    float dy1 = (float)(ly - rh * 0.5);
    if (dx1 * dx1 + dy1 * dy1 < r2)
    {
        return 1;
    }
    float dx2 = (float)(lx + hw);
    if (dx2 * dx2 + dy1 * dy1 < r2)
    {
        return 1;
    }
    float dy2 = (float)(ly + rh * 0.5);
    if (dx1 * dx1 + dy2 * dy2 < r2)
    {
        return 1;
    }
    if (dx2 * dx2 + dy2 * dy2 < r2)
    {
        return 1;
    }
    return 0;
}

int __stdcall HitTestCircleInsideOrientedRect(
    float rx, float ry, float rw, float rh, float angle, float cx, float cy, float radius)
{
    float dx = cx - rx;
    float dy = cy - ry;
    float s = sinf(-angle);
    float c = cosf(-angle);
    float lx = dx * c - dy * s;
    if (fabsf(lx) <= rw * 0.5 - radius)
    {
        float ly = dx * s + dy * c;
        if (fabsf(ly) <= rh * 0.5 - radius)
        {
            return 1;
        }
    }
    return 0;
}

int __stdcall IntersectSegments2D(
    float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy)
{
    float t1 = (ax - bx) * (cy - ay) + (ay - by) * (ax - cx);
    float t2 = (ax - bx) * (dy - ay) + (ay - by) * (ax - dx);
    if (t2 * t1 > 0.0f)
    {
        return 0;
    }
    if (t1 == 0.0f && t2 == 0.0f)
    {
        if (bx < ax)
        {
            float tmp = ax; ax = bx; bx = tmp;
            tmp = ay; ay = by; by = tmp;
        }
        if (dx < cx)
        {
            float tmp = cx; cx = dx; dx = tmp;
            tmp = cy; cy = dy; dy = tmp;
        }
        if (bx >= cx && dx >= ax && by >= cy && dy >= ay)
        {
            return 1;
        }
        return 0;
    }
    float t3 = (cx - dx) * (ay - cy) + (cy - dy) * (cx - ax);
    float t4 = (cx - dx) * (by - cy) + (cy - dy) * (cx - bx);
    if (t3 * t4 > 0.0f)
    {
        return 0;
    }
    return 1;
}

int __stdcall IntersectSegmentOrientedRect(
    float rx, float ry, float rw, float rh, float angle,
    float ax, float ay, float bx, float by)
{
    if (HitTestPointOrientedRect(rx, ry, rw, rh, angle, ax, ay) == 0)
    {
        GeoFloat3 corners[4];
        corners[0].x = (float)(-rw * 0.5);
        corners[0].y = (float)(-rh * 0.5);
        corners[0].z = 0.0f;
        corners[1].x = (float)(rw * 0.5);
        corners[1].y = (float)(-rh * 0.5);
        corners[1].z = 0.0f;
        corners[2].x = (float)(rw * 0.5);
        corners[2].y = (float)(rh * 0.5);
        corners[2].z = 0.0f;
        corners[3].x = (float)(-rw * 0.5);
        corners[3].y = (float)(rh * 0.5);
        corners[3].z = 0.0f;

        GeoFloat3 *p = corners;
        for (int i = 0; i < 4; ++i, ++p)
        {
            if (angle != 0.0f)
            {
                float s = sinf(angle);
                float c = cosf(angle);
                float nx = p->x * c - s * p->y;
                p->y = p->x * s + c * p->y;
                p->x = nx;
            }
            p->x = p->x + rx;
            p->y = ry + p->y;
        }

        for (int i = 0; i < 4; ++i)
        {
            int next = (i + 1) % 4;
            if (IntersectSegments2D(
                    corners[i].x, corners[i].y,
                    corners[next].x, corners[next].y,
                    ax, ay, bx, by) != 0)
            {
                return 1;
            }
        }
        return 0;
    }
    return 1;
}

int __stdcall LineEquationFromPoints(
    float ax, float ay, float bx, float by, float *outM, float *outB)
{
    if (fabsf(bx - ax) < 0.01f)
    {
        *outM = 0.0f;
        *outB = ax;
        return 1;
    }
    *outM = (by - ay) / (bx - ax);
    *outB = ay - (by - ay) * ax / (bx - ax);
    return 0;
}

int __stdcall IntersectSegmentsPoint2D(
    float ax, float ay, float bx, float by,
    float cx, float cy, float dx, float dy,
    float *outX, float *outY)
{
    if (IntersectSegments2D(ax, ay, bx, by, cx, cy, dx, dy) == 0)
    {
        return 0;
    }

    float m1, b1;
    int v1 = LineEquationFromPoints(ax, ay, bx, by, &m1, &b1);
    float m2, b2;
    int v2 = LineEquationFromPoints(cx, cy, dx, dy, &m2, &b2);

    if (v1 == 0)
    {
        if (v2 == 0)
        {
            *outX = (b2 - b1) / (m1 - m2);
            *outY = (m1 * (b2 - b1)) / (m1 - m2) + b1;
            return 1;
        }
        *outX = b2;
        *outY = b2 * m1 + b1;
        return 1;
    }
    if (v2 == 0)
    {
        *outX = b1;
        *outY = b1 * m2 + b2;
        return 1;
    }
    if (fabsf(b1 - b2) < 0.001f)
    {
        *outX = ax;
        *outY = ay;
        return 1;
    }
    return 0;
}

int __stdcall RayCircleIntersect2D(
    const GeoFloat2 *rayOrigin, const GeoFloat2 *circlePos,
    float *outT0, float *outT1, float rayAngle, float radius)
{
    float dx = circlePos->x - rayOrigin->x;
    float dy = circlePos->y - rayOrigin->y;
    float s = sinf(-rayAngle);
    float c = cosf(-rayAngle);
    float lx = dx * c - dy * s;
    float ly = dx * s + dy * c;

    if (fabsf(ly) > radius)
    {
        return 0;
    }
    if (lx < -radius)
    {
        return 0;
    }
    if (lx < 0.0f && lx * lx + ly * ly > radius * radius)
    {
        return 0;
    }
    float k = ly / radius;
    float dHalf = sqrtf(1.0f - k * k) * radius;
    *outT0 = lx - dHalf;
    *outT1 = lx + dHalf;
    return 1;
}
