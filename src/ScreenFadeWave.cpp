#include "ScreenFadeWave.hpp"

namespace th128 {

void SfwDrawSolidRect(const SfwRectF *rect, DWORD color) {
    SfwFlushAnmSpriteBatch(g_SfwAnmManager);
    SfwVertex v[4];
    v[0].pos = SfwVec3(rect->left, rect->top, 0.0f);
    v[1].pos = SfwVec3(rect->right, rect->top, 0.0f);
    v[2].pos = SfwVec3(rect->left, rect->bottom, 0.0f);
    v[3].pos = SfwVec3(rect->right, rect->bottom, 0.0f);
    v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;
    v[0].color = v[1].color = v[2].color = v[3].color = color;

    g_SfwD3dDevice->SetTextureStageState(0, 4, 2);
    g_SfwD3dDevice->SetTextureStageState(0, 1, 2);
    g_SfwD3dDevice->SetTextureStageState(0, 5, 0);
    g_SfwD3dDevice->SetTextureStageState(0, 2, 0);
    g_SfwD3dDevice->SetRenderState(0x14, 6);
    g_SfwD3dDevice->SetFVF(0x44);
    g_SfwD3dDevice->DrawPrimitiveUP(5, 2, v, sizeof(SfwVertex));

    g_SfwAnmManager->f4e9694 = 0;
    g_SfwAnmManager->f4e9688 = 0;
    g_SfwAnmManager->f4e968d = 0xff;
    g_SfwAnmManager->f4e968e = 0xff;
    g_SfwAnmManager->f4e968c = 7;
    g_SfwAnmManager->f4e968f = 0xff;

    g_SfwD3dDevice->SetTextureStageState(0, 4, 4);
    g_SfwD3dDevice->SetTextureStageState(0, 1, 4);
    g_SfwD3dDevice->SetTextureStageState(0, 5, 2);
    g_SfwD3dDevice->SetTextureStageState(0, 2, 2);
}

void __stdcall SfwDrawGradientRect(const SfwRectF *rect, DWORD c0, DWORD c1, DWORD c2, DWORD c3) {
    SfwFlushAnmSpriteBatch(g_SfwAnmManager);
    SfwVertex v[4];
    v[0].pos = SfwVec3(rect->left, rect->top, 0.0f);
    v[1].pos = SfwVec3(rect->right, rect->top, 0.0f);
    v[2].pos = SfwVec3(rect->left, rect->bottom, 0.0f);
    v[3].pos = SfwVec3(rect->right, rect->bottom, 0.0f);
    v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;
    v[0].color = c0;
    v[1].color = c1;
    v[2].color = c2;
    v[3].color = c3;

    g_SfwD3dDevice->SetTextureStageState(0, 4, 2);
    g_SfwD3dDevice->SetTextureStageState(0, 1, 2);
    g_SfwD3dDevice->SetTextureStageState(0, 5, 0);
    g_SfwD3dDevice->SetTextureStageState(0, 2, 0);
    g_SfwD3dDevice->SetRenderState(0x14, 6);
    g_SfwD3dDevice->SetFVF(0x44);
    g_SfwD3dDevice->DrawPrimitiveUP(5, 2, v, sizeof(SfwVertex));

    g_SfwAnmManager->f4e9694 = 0;
    g_SfwAnmManager->f4e9688 = 0;
    g_SfwAnmManager->f4e968d = 0xff;
    g_SfwAnmManager->f4e968e = 0xff;
    g_SfwAnmManager->f4e968c = 7;
    g_SfwAnmManager->f4e968f = 0xff;

    g_SfwD3dDevice->SetTextureStageState(0, 4, 4);
    g_SfwD3dDevice->SetTextureStageState(0, 1, 4);
    g_SfwD3dDevice->SetTextureStageState(0, 5, 2);
    g_SfwD3dDevice->SetTextureStageState(0, 2, 2);
}

int SfwScreenEffect::OnDrawFade() {
    SfwRectF r;
    r.left = 0.0f;
    r.top = 0.0f;
    r.right = 640.0f;
    r.bottom = 480.0f;
    SfwFlushAnmSpriteBatch(g_SfwAnmManager);
    g_SfwViewport.X = 0;
    g_SfwViewport.Y = 0;
    g_SfwViewport.Width = 640;
    g_SfwViewport.Height = 480;
    g_SfwD3dDevice->SetViewport(&g_SfwViewport);
    SfwDrawSolidRect(&r, ((DWORD)alpha18 << 24) | (DWORD)param20);
    return 1;
}

int SfwScreenEffect::OnCalcFlash() {
    unsigned int baseAlpha = (unsigned int)((unsigned char *)&param24)[3];
    if (g_SfwPaused != 0) {
        return 7;
    }
    if (timer.current < duration1c) {
        alpha18 = (int)(baseAlpha - (unsigned int)(timer.currentF * (float)baseAlpha / (float)duration1c));
        if (alpha18 < 0) {
            alpha18 = 0;
        }
    } else {
        param20--;
        alpha18 = 0;
        if (param20 <= 0) {
            return 7;
        }
        timer.Reset();
    }
    timer.Increment();
    return 1;
}

int SfwScreenEffect::OnCalcShakeA() {
    if (g_SfwPaused != 0) {
        return 7;
    }
    timer.Increment();
    if (timer.current >= duration1c) {
        return 7;
    }
    float t1 = (float)(param24 - param20) * timer.currentF;
    float t2 = t1 / (float)duration1c;
    float amp = t2 + (float)param20;
    switch (g_SfwRng.GetU32() % 3) {
    case 0: g_SfwShakeX = 0.0f; break;
    case 1: g_SfwShakeX = amp; break;
    case 2: g_SfwShakeX = -amp; break;
    }
    switch (g_SfwRng.GetU32() % 3) {
    case 0: g_SfwShakeY = 0.0f; break;
    case 1: g_SfwShakeY = amp; break;
    case 2: g_SfwShakeY = -amp; break;
    }
    return 1;
}

int SfwScreenEffect::OnCalcShakeB() {
    if (g_SfwPaused != 0) {
        return 7;
    }
    if (g_SfwStage == NULL || (g_SfwStage->bits.b0 | g_SfwStage->bits.b2) != 0 || (g_SfwStage->flags60 & 0x72) != 0) {
        return 1;
    }
    timer.Increment();
    float ratio;
    if (timer.current < param20) {
        ratio = timer.currentF / (float)param20;
    } else if (timer.current < param20 + param24) {
        ratio = 1.0f;
    } else if (timer.current < param20 + param24 + param28) {
        float rem = (float)(unsigned int)(param20 + param24 + param28) - timer.currentF;
        ratio = rem / (float)(unsigned int)param28;
    } else {
        return 7;
    }
    float amp = (float)duration1c * ratio;
    switch (g_SfwRng.GetU32() % 3) {
    case 0: g_SfwShakeX = 0.0f; break;
    case 1: g_SfwShakeX = amp; break;
    case 2: g_SfwShakeX = -amp; break;
    }
    switch (g_SfwRng.GetU32() % 3) {
    case 0: g_SfwShakeY = 0.0f; break;
    case 1: g_SfwShakeY = amp; break;
    case 2: g_SfwShakeY = -amp; break;
    }
    return 1;
}

} // namespace th128
