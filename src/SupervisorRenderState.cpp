#include "SupervisorRenderState.hpp"
#include "Supervisor.hpp"
#include "AnmManager.hpp"

#include <d3d9.h>

namespace
{
    inline DWORD FloatState(float value)
    {
        return *reinterpret_cast<DWORD *>(&value);
    }

    // Only the cache fields written by 0x00455A70 are exposed here.
    struct AnmInitialRenderCache
    {
        unsigned char unknown000[0x4E9688];
        void *textureSlot;
        unsigned char blendMode;
        unsigned char unknown4E968D;
        unsigned char vertexFormatMode;
        unsigned char unknown4E968F;
        unsigned char unknown4E9690;
    };
}

void __stdcall InitializeSupervisorRenderState()
{
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_ALPHAREF, 1);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_FOGDENSITY, FloatState(1.0f));
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0xFFA0A0A0);
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_FOGSTART, FloatState(1000.0f));
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_FOGEND, FloatState(5000.0f));
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

    g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
    g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

    if (g_AnmManager != 0)
    {
        AnmInitialRenderCache *cache = reinterpret_cast<AnmInitialRenderCache *>(g_AnmManager);
        cache->blendMode = 7;
        cache->unknown4E968D = 0xFF;
        cache->vertexFormatMode = 0xFF;
        cache->textureSlot = 0;
        cache->unknown4E9690 = 0xFF;
    }
}
