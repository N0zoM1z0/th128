#include "../src/TextImageHelpers.hpp"

extern "C" int __stdcall TextImageHelpersProbe(
    TextImage *image, IDirect3DSurface9 *surface, HFONT font, const char *text,
    COLORREF color, D3DFORMAT format, unsigned int *sums, const TextPixel8888 *p8888,
    const TextPixel4444 *p4444, unsigned int *count, UINT w, UINT h, unsigned char alpha)
{
    int result = 0;
    FillTextImage4444(color);
    if (FindTextImageFormat(format) != 0)
        result += 1;
    AddTextColor8888(sums, p8888, count);
    AddTextColor4444(p4444, sums, count);
    if (image->SetAlpha(alpha))
        result += 2;
    if (image->DrawOutlinedText(font, text, 1, 2, color, color))
        result += 4;
    if (image->CopyFromSurface(surface))
        result += 8;
    if (image->CopyToSurface(surface))
        result += 16;
    if (image->AlphaBlendToSurface(surface, 1, 2, static_cast<int>(w), static_cast<int>(h)))
        result += 32;
    if (CreateOffscreenA1R5G5B5Texture(w, h) != 0)
        result += 64;
    return result;
}
