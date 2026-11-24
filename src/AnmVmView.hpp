#pragma once

struct AnmVm;

// The target view paths write four independent 3D corner vectors in TL, TR,
// BL, BR order. Keep the four output pointers explicit rather than pretending
// they are a single packed array; the original caller owns four separate
// temporary vectors.
void BuildAnmVmViewCornersMode0(
    const AnmVm *vm,
    float *topLeft,
    float *topRight,
    float *bottomLeft,
    float *bottomRight);
void BuildAnmVmViewCornersMode1(
    const AnmVm *vm,
    float *topLeft,
    float *topRight,
    float *bottomLeft,
    float *bottomRight);
void BuildAnmVmViewCornersMode23(
    const AnmVm *vm,
    float *topLeft,
    float *topRight,
    float *bottomLeft,
    float *bottomRight);
void BuildAnmVmViewCorners(const AnmVm *vm, float *corners);
