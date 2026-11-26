#include "../src/TextImageLifetime.hpp"

extern "C" int __stdcall TextImageLifetimeProbe(D3DFORMAT format)
{
    bool result = InitializeTextImage(format);
    if (InitializeDefaultTextImage())
        result = true;
    InitializeTextFonts();
    const bool destroyed = DestroyTextImage();
    DestroyTextFonts();
    if (destroyed)
        return 1;
    return result;
}
