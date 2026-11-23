#pragma once

// Wraps an angle into [-pi, pi] with at most 33 corrections.
float __stdcall NormalizeAngle(float angle);
// NormalizeAngle(angle + offset), rounding the sum to float first.
float __stdcall AddAngles(float angle, float offset);
// Returns the target-minus-current angular delta along the shorter arc.
float __stdcall ShortestAngleDifference(float target, float current);
