#include "LowFpuWaveC.hpp"

#include <string.h>

namespace th128 {

int LfcEnding::Init(int arg) {
    f80 = g_LfcGameGlobal->f8a7f9c;
    LfcEndingSub *sub = new LfcEndingSub();
    subB4 = sub;
    sub->Load(arg);

    LfcChainNode *calc = new LfcChainNode();
    calc->flags = (calc->flags & ~2u) | 1u;
    calc->callback = (void *)&LfcEndingOnCalc;
    calc->onRegister = NULL;
    calc->onDelete = NULL;
    calc->arg = this;
    LfcRegisterCalc(calc, 0x15);
    calcNode = calc;

    LfcChainNode *draw = new LfcChainNode();
    draw->flags = (draw->flags & ~2u) | 1u;
    draw->callback = (void *)&LfcEndingOnDraw;
    draw->onRegister = NULL;
    draw->onDelete = NULL;
    draw->arg = this;
    LfcRegisterDraw(draw, 0x15);
    drawNode = draw;

    timerA0.Reset();
    return 0;
}

int LfcObj36470::Register() {
    LfcChainNode *calc = new LfcChainNode();
    calc->flags = (calc->flags & ~2u) | 1u;
    calc->callback = (void *)&LfcObj36470OnCalc;
    calc->onRegister = NULL;
    calc->onDelete = NULL;
    calc->arg = this;
    LfcRegisterCalc(calc, 9);
    calcNode = calc;

    LfcChainNode *draw = new LfcChainNode();
    draw->flags = (draw->flags & ~2u) | 1u;
    draw->callback = (void *)&LfcObj36470OnDraw;
    draw->onRegister = NULL;
    draw->onDelete = NULL;
    draw->arg = this;
    LfcRegisterDraw(draw, 0x42);
    drawNode = draw;

    timer10.Reset();
    timer24.Reset();
    return 0;
}

void LfcSpawnPopupNumber(const LfcVec3 *pos, int value, int color) {
    LfcPopupMgr *mgr = g_LfcPopupMgr;
    if (mgr->nextIdx >= 10) {
        mgr->nextIdx = 0;
    }
    LfcPopup *p = &mgr->popups[mgr->nextIdx];
    p->active = 1;
    int count = 0;
    if (value >= 0) {
        while (value != 0) {
            p->digits[count] = (unsigned char)(value % 10);
            value /= 10;
            count++;
        }
        if (count == 0) {
            p->digits[0] = 0;
            count = 1;
        }
    } else {
        p->digits[0] = 10;
        count = 1;
    }
    p->digitCount = (unsigned char)count;
    p->color = color;
    p->timer.Reset();
    p->pos.x = pos->x;
    p->pos.y = pos->y;
    p->pos.z = pos->z;
    mgr->nextIdx++;
}

void LfcHud::SetBarFill(float fill, int index) {
    LfcAnmVm *vm = g_LfcAnmManager->GetVmById(anmId68);
    if (vm == NULL) {
        anmId68 = 0;
    }
    vm->FindChildByScript(index + 0x8a)->f440 = fill;

    vm = g_LfcAnmManager->GetVmById(anmId68);
    if (vm == NULL) {
        anmId68 = 0;
    }
    vm->FindChildByScript(index + 0x96)->f440 = fill;
}

int LfcPauseDrawTargetA() {
    if (g_LfcSurfaceA != NULL) {
        LfcFlushAnmSpriteBatch(g_LfcAnmManager);
        g_LfcD3dDevice->SetRenderTarget(0, g_LfcSurfaceB);
        LfcD3dRect rect[1];
        rect[0].x1 = g_LfcViewportX;
        rect[0].y1 = g_LfcViewportY;
        rect[0].x2 = g_LfcViewportX + g_LfcViewportW;
        rect[0].y2 = g_LfcViewportY + g_LfcViewportH;
        g_LfcD3dDevice->Clear(1, rect, 3, g_LfcClearColor, 1.0f, 0);
    }
    return 1;
}

int LfcPauseDrawTargetB() {
    if (g_LfcSurfaceA != NULL) {
        LfcFlushAnmSpriteBatch(g_LfcAnmManager);
        g_LfcD3dDevice->SetRenderTarget(0, g_LfcSurfaceA);
        LfcD3dRect rect[1];
        rect[0].x1 = g_LfcViewportX;
        rect[0].y1 = g_LfcViewportY;
        rect[0].x2 = g_LfcViewportX + g_LfcViewportW;
        rect[0].y2 = g_LfcViewportY + g_LfcViewportH;
        g_LfcD3dDevice->Clear(1, rect, 3, g_LfcClearColor, 1.0f, 0);
    }
    return 1;
}

LfcAbilityMgr::LfcAbilityMgr() {
    memset(this, 0, sizeof(LfcAbilityMgr));
    flags04 |= 2u;
    f1c858 = 1.0f;
    f1c85c = 1.0f;
    g_LfcAbilityMgr = this;
    f1c84c = -1;
    f1c864 = 0;
    f1c88c = 9;
    f1c878 = 1;
    f1c87c = 1;
}

int LfcLaserMgr::OnDraw() {
    LfcD3dRect rect = { 0x80, 0x10, 0x200, 0x1d0 };
    g_LfcD3dDevice->Clear(1, &rect, 2, 0, 1.0f, 0);
    LfcLaser *it = head18;
    while (it != NULL) {
        LfcLaser *next = it->next08;
        if (it->state0c != 1) {
            it->OnDraw();
        }
        it = next;
    }
    return 1;
}

void LfcAnmVm::SetEffectState(int p1, unsigned char p2, unsigned char p3, unsigned char p4) {
    f160 = p1;
    f164 = p2;
    f13c = p3;
    f140 = p4;
    f144 = 0;
    f148 = 0;
    timer14c.Reset();
}

LfcEclVm *LfcSptResourceInf::CreateVm(const char *name) {
    LfcEclVm *vm = new LfcEclVm();
    vm->resource11ac = this;
    vm->curFrame->ip = FindSub(name);
    vm->curFrame->time = 0.0f;
    return vm;
}

} // namespace th128
