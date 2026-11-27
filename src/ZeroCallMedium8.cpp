#include "ZeroCallMedium8.hpp"

#include <string.h>

namespace th128 {

static inline void Zc8MatrixIdentity(D3DMATRIX *m) {
    m->_12 = m->_13 = m->_14 =
    m->_21 = m->_23 = m->_24 =
    m->_31 = m->_32 = m->_34 =
    m->_41 = m->_42 = m->_43 = 0.0f;
    m->_11 = m->_22 = m->_33 = m->_44 = 1.0f;
}

int Zc8AnmSetSprite(Zc8AnmVm *vm, Zc8AnmLoaded *loaded, int spriteId) {
    if (loaded->data108 == 0 || loaded->flag124 != 0) {
        return -1;
    }
    vm->spriteId = (short)spriteId;
    vm->sprite = &loaded->sprites[spriteId];

    vm->uv84 = vm->uv94 = vm->sprite->u0;
    vm->uv8c = vm->uv9c = vm->sprite->u1;
    vm->uv88 = vm->uv90 = vm->sprite->v0;
    vm->uv98 = vm->uvA0 = vm->sprite->v1;

    vm->spriteW = vm->sprite->w;
    vm->spriteH = vm->sprite->h;

    Zc8MatrixIdentity(&vm->mat1);
    Zc8MatrixIdentity(&vm->mat3);

    vm->mat1._11 = (float)(vm->spriteW * (1.0 / 256.0));
    vm->mat1._22 = (float)(vm->spriteH * (1.0 / 256.0));
    vm->mat3._11 = (vm->spriteW / vm->sprite->texW) * vm->sprite->uvScaleX;
    vm->mat3._22 = (vm->spriteH / vm->sprite->texH) * vm->sprite->uvScaleY;
    vm->mat2 = vm->mat1;
    return 0;
}

int Zc8AnmDrawLineStrip(int count, const Zc8Vec2 *pts, const unsigned int *colors) {
    Zc8AnmMgrLine *mgr = g_Zc8AnmMgr;
    Zc8LineVert *cur = mgr->vertCursor;
    if ((unsigned char *)(cur + count + 1) >= (unsigned char *)&mgr->vertCursor) {
        return 0;
    }
    for (int i = 0; i < count; ++i) {
        cur[i].x = pts[i].x;
        cur[i].y = pts[i].y;
        cur[i].z = 0.0f;
        cur[i].rhw = 1.0f;
        cur[i].color = colors[i];
    }
    if (g_Zc8AnmMgr->state93 != 0) {
        g_Zc8D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 3);
        g_Zc8D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, 3);
        g_Zc8AnmMgr->state93 = 0;
    }
    if (mgr->state8e != 1) {
        g_Zc8D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
        g_Zc8D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
        mgr->state8e = 1;
    }
    g_Zc8D3DDevice->SetFVF(0x44);
    g_Zc8D3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, count - 1, mgr->vertCursor, sizeof(Zc8LineVert));
    mgr->vertCursor += count;
    ++mgr->drawCount;
    return 0;
}

} // namespace th128
