#pragma once

// Device-loss passes over the manager's 38 loaded ANM resource slots.
void __stdcall ReleaseAnmRenderTargets();
void __stdcall RecreateAnmRenderTargets();
