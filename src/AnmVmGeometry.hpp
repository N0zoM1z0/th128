#pragma once

struct AnmVmEffect10Vertex
{
    float x;
    float y;
    float z;
    float reciprocalW;
    unsigned int color;
    float u;
    float v;

    void SetPolar(float angle, float radius);
    void SetPolarForUpdate(float angle, float radius);
};
typedef char AnmVmEffect10VertexSize[
    (sizeof(AnmVmEffect10Vertex) == 0x1C) ? 1 : -1
];
