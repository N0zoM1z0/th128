#include "AnmDrawChainWave.hpp"

#include <process.h>
#include <string.h>

namespace th128 {

int AdcwAnmManager::DrawMeshStrip(AdcwAnmVm *vm, const void *verts, int vertCount) {
    if ((vm->flags4b0 & 1u) == 0) {
        return -1;
    }
    if ((vm->flags4b0 & 2u) == 0) {
        return -1;
    }
    if (vm->alpha3f3 == 0) {
        return -1;
    }
    if (batchCount4e96ec != 0) {
        FlushSpriteBatch();
    }
    IDirect3DTexture9 **texSlot = vm->sprite428->texSlot8;
    if (curTex4e9688 != texSlot) {
        curTex4e9688 = texSlot;
        g_AdcwDevice->SetTexture(0, *texSlot);
    }
    if (fvfMode4e968e != 3) {
        g_AdcwDevice->SetFVF(0x144);
        g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
        g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
        fvfMode4e968e = 3;
    }
    ApplyState(vm);
    if (g_AdcwAnmMgr->stageMode4e9693 != 1) {
        g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
        g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLOROP, 4);
        g_AdcwAnmMgr->stageMode4e9693 = 1;
    }
    g_AdcwDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, vertCount - 2, verts, 0x1c);
    return 0;
}

int AdcwAnmManager::DrawTriangleStrip2D(const void *verts, int vertCount) {
    if (batchCount4e96ec != 0) {
        FlushSpriteBatch();
    }
    if (fvfMode4e968e != 4) {
        g_AdcwDevice->SetFVF(0x44);
        fvfMode4e968e = 4;
    }
    ApplyStateNoVm();
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 2);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLOROP, 2);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, 0);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLORARG1, 0);
    g_AdcwAnmMgr->FlushSpriteBatch();
    g_AdcwDevice->SetRenderState(D3DRS_ZWRITEENABLE, 0);
    g_AdcwDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, vertCount - 2, verts, 0x14);
    g_AdcwAnmMgr->colorOp4e968d = 0xff;
    g_AdcwAnmMgr->fvfMode4e968e = 0xff;
    g_AdcwAnmMgr->blendMode4e968c = 7;
    g_AdcwAnmMgr->zWrite4e968f = 0xff;
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLOROP, 4);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, 2);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLORARG1, 2);
    return 0;
}

int AdcwAnmManager::DrawTriangleFan2D(const void *verts, int vertCount) {
    if (batchCount4e96ec != 0) {
        FlushSpriteBatch();
    }
    if (fvfMode4e968e != 4) {
        g_AdcwDevice->SetFVF(0x44);
        fvfMode4e968e = 4;
    }
    ApplyStateNoVm();
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 2);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLOROP, 2);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, 0);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLORARG1, 0);
    g_AdcwAnmMgr->FlushSpriteBatch();
    g_AdcwDevice->SetRenderState(D3DRS_ZWRITEENABLE, 0);
    g_AdcwDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertCount - 2, verts, 0x14);
    g_AdcwAnmMgr->colorOp4e968d = 0xff;
    g_AdcwAnmMgr->fvfMode4e968e = 0xff;
    g_AdcwAnmMgr->blendMode4e968c = 7;
    g_AdcwAnmMgr->zWrite4e968f = 0xff;
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLOROP, 4);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, 2);
    g_AdcwDevice->SetTextureStageState(0, D3DTSS_COLORARG1, 2);
    return 0;
}

void AdcwAnmManager::CopyBackBufferToTexture(const AdcwCopyBackBufReq *req) {
    if (slots4e90c0[req->anmSlot0]->textures120[req->texIdx4].tex0 == NULL) {
        return;
    }
    FlushSpriteBatch();
    IDirect3DSurface9 *backBuf;
    if (g_AdcwDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuf) != 0) {
        return;
    }
    IDirect3DSurface9 *dstSurf;
    if (slots4e90c0[req->anmSlot0]->textures120[req->texIdx4].tex0->GetSurfaceLevel(0, &dstSurf) != 0) {
        backBuf->Release();
        return;
    }
    RECT srcRect;
    srcRect.left = req->srcX8;
    srcRect.top = req->srcYC;
    srcRect.right = req->srcX8 + req->srcW10;
    srcRect.bottom = req->srcYC + req->srcH14;
    RECT dstRect;
    dstRect.left = req->dstX18;
    dstRect.top = req->dstY1C;
    dstRect.right = req->dstX18 + req->dstW20;
    dstRect.bottom = req->dstY1C + req->dstH24;
    if (D3DXLoadSurfaceFromSurface(dstSurf, NULL, &dstRect, backBuf, NULL, &srcRect, 2, 0) == 0) {
        slots4e90c0[req->anmSlot0]->textures120[req->texIdx4].tex0->AddDirtyRect(NULL);
    }
    dstSurf->Release();
    backBuf->Release();
}

void AdcwAnmManager::CopyTextureToTexture(int dstSlot, int dstTex, int srcSlot, int srcTex, const RECT *dstRect, const RECT *srcRect) {
    if (slots4e90c0[dstSlot]->textures120[dstTex].tex0 == NULL) {
        return;
    }
    if (slots4e90c0[srcSlot]->textures120[srcTex].tex0 == NULL) {
        return;
    }
    FlushSpriteBatch();
    IDirect3DSurface9 *dstSurf;
    if (slots4e90c0[dstSlot]->textures120[dstTex].tex0->GetSurfaceLevel(0, &dstSurf) != 0) {
        return;
    }
    IDirect3DSurface9 *srcSurf;
    if (slots4e90c0[srcSlot]->textures120[srcTex].tex0->GetSurfaceLevel(0, &srcSurf) != 0) {
        dstSurf->Release();
        return;
    }
    if (D3DXLoadSurfaceFromSurface(dstSurf, NULL, dstRect, srcSurf, NULL, srcRect, (DWORD)-1, 0) == 0) {
        dstSurf->Release();
        srcSurf->Release();
        return;
    }
    dstSurf->Release();
    srcSurf->Release();
}

void __stdcall AdcwClearTextureBlackOpaque(IDirect3DTexture9 **texPtr) {
    IDirect3DSurface9 *surf;
    (*texPtr)->GetSurfaceLevel(0, &surf);
    D3DSURFACE_DESC desc;
    surf->GetDesc(&desc);
    D3DLOCKED_RECT locked;
    surf->LockRect(&locked, NULL, 0);
    switch (desc.Format) {
    case D3DFMT_R8G8B8:
    case D3DFMT_R5G6B5:
        memset(locked.pBits, 0, locked.Pitch * desc.Height);
        break;
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8: {
        unsigned char *p = (unsigned char *)locked.pBits;
        for (unsigned int i = 0; i < desc.Width * desc.Height; ++i) {
            p[3] = 0xff;
            p[2] = 0;
            p[1] = 0;
            p[0] = 0;
            p += 4;
        }
        break;
    }
    case D3DFMT_A4R4G4B4: {
        unsigned short *p = (unsigned short *)locked.pBits;
        for (unsigned int i = 0; i < desc.Width * desc.Height; ++i, ++p) {
            *p = 0xf000;
        }
        break;
    }
    default:
        break;
    }
    surf->UnlockRect();
    if (surf != NULL) {
        surf->Release();
    }
}

int AdcwBulletMgr::RegisterChains() {
    bulletAnm8a7f9c = g_AdcwAnmMgr->PreloadAnm(6, "bullet.anm");
    if (bulletAnm8a7f9c == NULL) {
        g_AdcwErrorCtx.Log("error : bullet.anm\n");
        return -1;
    }
    firstBullet10 = bullets64;
    sentinel8a780e = 6;
    AdcwChainElem *calcElem = new AdcwChainElem();
    calcElem->cb8 = (void *)&AdcwBulletCalcCb;
    calcElem->onRegC = NULL;
    calcElem->onCut10 = NULL;
    calcElem->flags4 = (calcElem->flags4 & ~2u) | 1u;
    calcElem->arg20 = this;
    g_AdcwChain->RegisterCalc(calcElem, 0x18);
    chain8 = calcElem;

    AdcwChainElem *drawElem = new AdcwChainElem();
    drawElem->cb8 = (void *)&AdcwBulletDrawCb;
    drawElem->onRegC = NULL;
    drawElem->onCut10 = NULL;
    drawElem->flags4 = (drawElem->flags4 & ~2u) | 1u;
    drawElem->arg20 = this;
    g_AdcwChain->RegisterDraw(drawElem, 0x20);
    chainC = drawElem;
    return 0;
}

int __stdcall AdcwMother::RegisterChainsAndThread() {
    AdcwChainElem *calcElem = new AdcwChainElem();
    calcElem->cb8 = (void *)&AdcwMotherCalcCb;
    calcElem->onRegC = NULL;
    calcElem->onCut10 = NULL;
    calcElem->flags4 = (calcElem->flags4 & ~2u) | 1u;
    calcElem->arg20 = this;
    g_AdcwChain->RegisterCalc(calcElem, 3);
    chain8 = calcElem;

    AdcwChainElem *drawElem = new AdcwChainElem();
    drawElem->cb8 = (void *)&AdcwMotherDrawCb;
    drawElem->onRegC = NULL;
    drawElem->onCut10 = NULL;
    drawElem->flags4 = (drawElem->flags4 & ~2u) | 1u;
    drawElem->arg20 = this;
    g_AdcwChain->RegisterDraw(drawElem, 0x3c);
    chainC = drawElem;

    thread10.Stop();
    thread10.entry18 = &AdcwMotherThreadProc;
    thread10.running10 = 1;
    thread10.flagsC = 0;
    thread10.handle4 = (HANDLE)_beginthreadex(NULL, 0, &AdcwMotherThreadProc, this, 0, &thread10.threadId8);
    return 0;
}

} // namespace th128
