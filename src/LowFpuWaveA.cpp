#include "LowFpuWaveA.hpp"

#include <string.h>

namespace th128 {

// 1. 0x00433850 (159B)
int LfaClearRenderTargetB() {
    if (g_LfaRenderTargetA == NULL) {
        return 1;
    }
    g_LfaAnmMgr->FlushSpriteBatch();
    g_LfaDevice->SetRenderTarget(0, g_LfaRenderTargetB);
    LfaD3dRect rc;
    rc.x1 = g_LfaViewX;
    rc.y1 = g_LfaViewY;
    rc.x2 = g_LfaViewX + g_LfaViewW;
    rc.y2 = g_LfaViewY + g_LfaViewH;
    g_LfaDevice->Clear(1, &rc, 3, g_LfaClearColor, 1.0f, 0);
    return 1;
}

// 2. 0x004338F0 (159B)
int LfaClearRenderTargetA() {
    if (g_LfaRenderTargetA == NULL) {
        return 1;
    }
    g_LfaAnmMgr->FlushSpriteBatch();
    g_LfaDevice->SetRenderTarget(0, g_LfaRenderTargetA);
    LfaD3dRect rc;
    rc.x1 = g_LfaViewX;
    rc.y1 = g_LfaViewY;
    rc.x2 = g_LfaViewX + g_LfaViewW;
    rc.y2 = g_LfaViewY + g_LfaViewH;
    g_LfaDevice->Clear(1, &rc, 3, g_LfaClearColor, 1.0f, 0);
    return 1;
}

// 3. 0x0042A2E0 (138B)
int LfaLaserMgr::OnDraw() {
    LfaD3dRect rc = { 0x80, 0x10, 0x200, 0x1d0 };
    g_LfaDevice->Clear(1, &rc, 2, 0, 1.0f, 0);
    LfaDrawNode *node = this->head;
    while (node != NULL) {
        LfaDrawNode *next = node->next;
        if (node->skipFlag != 1) {
            node->Draw();
        }
        node = next;
    }
    return 1;
}

// 4. 0x0045E6C0 (133B)
void LfaAnmVmEffect::SetEffectState(int a, unsigned char b, unsigned char c, unsigned char d) {
    this->f160 = a;
    this->f164 = b;
    this->f13c = c;
    this->f144 = 0;
    this->f148 = 0;
    this->f140 = d;
    this->timer14c.ResetToMinusOne();
}

// 5. 0x00414580 (124B)
void LfaEclVm::Reset() {
    this->flags11a8 &= ~1u;
    this->mainFrame.time = 0.0f;
    this->mainFrame.sub = NULL;
    this->selfPtr = this;
    this->f1018 = -1;
    this->f1020 = 0;
    for (int i = 0; i < 8; ++i) {
        this->slots[i].ptr28 = NULL;
    }
    this->curFrame = &this->mainFrame;
    this->f1010 = 0;
    this->f1014 = 0;
    this->framePtr11b0 = &this->mainFrame;
    this->f11b4 = 0;
    this->f11b8 = 0;
}

// 6. 0x00401010 (140B)
LfaAsciiManager::LfaAsciiManager() {
    memset(this, 0, sizeof(LfaAsciiManager));
    this->flags04 |= 2u;
    this->f1c858 = 1.0f;
    this->f1c85c = 1.0f;
    g_LfaAsciiMgr = this;
    this->f1c84c = -1;
    this->f1c864 = 0;
    this->f1c88c = 9;
    this->f1c878 = 1;
    this->f1c87c = 1;
}

// 7. 0x00444010 (210B)
void LfaSpawnScorePopup(const LfaVec3 *pos, int value, int color) {
    LfaPopupMgr *mgr = g_LfaPopupMgr;
    if (mgr->nextIdx >= 10) {
        mgr->nextIdx = 0;
    }
    LfaPopupEntry *e = &mgr->entries[mgr->nextIdx];
    e->active = 1;
    int count = 0;
    if (value >= 0) {
        while (value != 0) {
            e->digits[count] = (char)(value % 10);
            value /= 10;
            ++count;
        }
        if (count == 0) {
            e->digits[0] = 0;
            count = 1;
        }
    } else {
        e->digits[0] = 10;
        count = 1;
    }
    e->digitCount = (unsigned char)count;
    e->color = color;
    e->timer.ResetToMinusOne();
    e->pos = *pos;
    ++mgr->nextIdx;
}

} // namespace th128
