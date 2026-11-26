#pragma once

#include <d3d9.h>

bool __stdcall DestroyTextImage();
bool __stdcall InitializeTextImage(D3DFORMAT format);
bool __stdcall InitializeDefaultTextImage();
void __stdcall InitializeTextFonts();
void __stdcall DestroyTextFonts();
