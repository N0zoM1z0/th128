#pragma once

// Consume the four pending back-buffer-to-ANM texture copy requests.
void __stdcall FlushAnmSurfaceCopies();
