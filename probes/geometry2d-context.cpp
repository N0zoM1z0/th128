#include "../src/Geometry2D.hpp"

extern "C" int __stdcall Geometry2DProbe(
    float a, float b, float c, float d, float e, float f, float g, float h,
    float *out1, float *out2, const GeoFloat2 *p1, const GeoFloat2 *p2)
{
    int acc = 0;
    acc += HitTestPointOrientedRect(a, b, c, d, e, f, g);
    acc += HitTestPointParallelogram(a, b, c, d, e, f, g, h);
    acc += HitTestCircleOrientedRect(a, b, c, d, e, f, g, h);
    acc += HitTestCircleInsideOrientedRect(a, b, c, d, e, f, g, h);
    acc += IntersectSegments2D(a, b, c, d, e, f, g, h);
    acc += IntersectSegmentOrientedRect(a, b, c, d, e, f, g, h, a);
    acc += IntersectSegmentsPoint2D(a, b, c, d, e, f, g, h, out1, out2);
    acc += LineEquationFromPoints(a, b, c, d, out1, out2);
    acc += RayCircleIntersect2D(p1, p2, out1, out2, e, f);
    return acc;
}
