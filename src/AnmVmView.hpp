#pragma once

struct AnmVm;

// Four corners are written as twelve consecutive floats in TL, TR, BL, BR order.
void BuildAnmVmViewCornersMode0(const AnmVm *vm, float *corners);
void BuildAnmVmViewCornersMode1(const AnmVm *vm, float *corners);
void BuildAnmVmViewCornersMode23(const AnmVm *vm, float *corners);
void BuildAnmVmViewCorners(const AnmVm *vm, float *corners);
