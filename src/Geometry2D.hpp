#pragma once

struct GeoFloat2
{
    float x;
    float y;
};

struct GeoFloat3
{
    float x;
    float y;
    float z;
};

int __stdcall HitTestPointOrientedRect(
    float rx, float ry, float rw, float rh, float angle, float px, float py);

int __stdcall HitTestPointParallelogram(
    float ax, float ay, float bx, float by, float cx, float cy, float px, float py);

int __stdcall HitTestCircleOrientedRect(
    float rx, float ry, float rw, float rh, float angle, float cx, float cy, float radius);

int __stdcall HitTestCircleInsideOrientedRect(
    float rx, float ry, float rw, float rh, float angle, float cx, float cy, float radius);

int __stdcall IntersectSegments2D(
    float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy);

int __stdcall IntersectSegmentOrientedRect(
    float rx, float ry, float rw, float rh, float angle,
    float ax, float ay, float bx, float by);

int __stdcall IntersectSegmentsPoint2D(
    float ax, float ay, float bx, float by,
    float cx, float cy, float dx, float dy,
    float *outX, float *outY);

int __stdcall LineEquationFromPoints(
    float ax, float ay, float bx, float by, float *outM, float *outB);

int __stdcall RayCircleIntersect2D(
    const GeoFloat2 *rayOrigin, const GeoFloat2 *circlePos,
    float *outT0, float *outT1, float rayAngle, float radius);
