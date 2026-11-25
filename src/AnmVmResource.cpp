#include "AnmResource.hpp"

#include <string.h>

// Converts the VM's pixel-space coordinates to normalized texture space.
// The target divides X by 640 and Y by 480, then clamps only the lower bound.
void NormalizeAnmCoordinates(float *normalized, const float *pixels)
{
    normalized[0] = pixels[0] / 640.0f;
    normalized[1] = pixels[1] / 480.0f;

    if (normalized[0] < 0.0f)
    {
        normalized[0] = 0.0f;
    }
    if (normalized[1] < 0.0f)
    {
        normalized[1] = 0.0f;
    }
}
