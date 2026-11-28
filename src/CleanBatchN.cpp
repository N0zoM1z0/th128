#include "CleanBatchN.hpp"

namespace th128 {

CbnEnemy::~CbnEnemy() {
    CbnEnemyMgr* mgr = g_CbnEnemyMgr;
    if (mgr->head == &listEntry145c) {
        mgr->head = listEntry145c.next;
    }
    if (mgr->tail == &listEntry145c) {
        mgr->tail = listEntry145c.prev;
    }
    CbnListNode* node = &listEntry145c;
    if (node->next != 0) {
        node->next->prev = node->prev;
    }
    if (node->prev != 0) {
        node->prev->next = node->next;
    }
    node->next = 0;
    node->prev = 0;
    --mgr->count;
    if ((flags50af & 1) != 0) {
        mgr->bossSlots[bossIdx50bc] = 0;
    }
    for (int i = 0; i < 16; ++i) {
        if (anmIds[i] != 0) {
            CbnAnmVm* vm = g_CbnAnmMgr->FindVmInline(anmIds[i]);
            if (vm != 0) {
                vm->flags4b0 |= 0x20000000u;
                if (vm->parent18 == 0) {
                    for (CbnAnmNode* c = vm->children14; c != 0; c = c->next) {
                        c->vm->flags4b0 |= 0x20000000u;
                    }
                }
            }
        }
        anmIds[i] = 0;
    }
    CbnEnemySub* sub = sub5148;
    if (sub != 0) {
        sub->Cleanup();
        ::operator delete(sub);
    }
    sub5148 = 0;
}

bool __stdcall CbnTextTex::BlitToSurface(CbnSurface* surf, int y, int w, int h, int x) {
    if (ready11c == 0) {
        return false;
    }
    CbnSurfaceDesc desc;
    surf->GetDesc(&desc);
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = width104;
    rc.bottom = height108;
    CbnLockedRect lr;
    if (surf->LockRect(&lr, &rc, 0) != 0) {
        return false;
    }
    int srcPitch = srcPitch110;
    int dstPitch = lr.Pitch;
    unsigned char* src = srcBits120;
    if (desc.Format == format100) {
        if (format100 == 0x15) {
            unsigned char* dst = (unsigned char*)lr.pBits + y * dstPitch + x * 4;
            for (int row = 0; row < h; ++row) {
                for (int col = 0; col < w; ++col) {
                    int a = src[3];
                    dst[0] = (unsigned char)(dst[0] + (((src[0] - dst[0]) * a) >> 8));
                    dst[1] = (unsigned char)(dst[1] + (((src[1] - dst[1]) * a) >> 8));
                    dst[2] = (unsigned char)(dst[2] + (((src[2] - dst[2]) * a) >> 8));
                    dst += 4;
                    src += 4;
                }
                src += srcPitch - w * 4;
                dst += dstPitch - w * 4;
            }
        } else if (format100 == 0x19) {
            unsigned char* dst = (unsigned char*)lr.pBits + y * dstPitch + x * 2;
            for (int row = 0; row < h; ++row) {
                for (int col = 0; col < w; ++col) {
                    unsigned short px = *(unsigned short*)src;
                    if ((px & 0x8000) != 0) {
                        *(unsigned short*)dst = px;
                    }
                    dst += 2;
                    src += 2;
                }
                src += srcPitch - w * 2;
                dst += dstPitch - w * 2;
            }
        } else if (format100 == 0x1a) {
            unsigned char* dst = (unsigned char*)lr.pBits + y * dstPitch + x * 2;
            for (int row = 0; row < h; ++row) {
                for (int col = 0; col < w; ++col) {
                    int a = src[1] >> 4;
                    int d0 = dst[0] >> 4;
                    int s0 = src[0] >> 4;
                    dst[0] = (unsigned char)((d0 + (((s0 - d0) * a) >> 4)) << 4);
                    int d1 = dst[0] & 0xf;
                    int s1 = src[0] & 0xf;
                    dst[0] |= (unsigned char)(d1 + (((s1 - d1) * a) >> 4));
                    int da = (dst[1] >> 2) & 0x3c;
                    int na = da + a;
                    if (na >= 16) {
                        na = 15;
                    }
                    int d2 = dst[1] & 0xf;
                    int s2 = src[1] & 0xf;
                    dst[1] = (unsigned char)((d2 + (((s2 - d2) * a) >> 4)) | (na << 4));
                    dst += 2;
                    src += 2;
                }
                src += srcPitch - w * 2;
                dst += dstPitch - w * 2;
            }
        } else {
            return false;
        }
    }
    surf->UnlockRect();
    return true;
}

int CbnPlayBgmSlot(int slot) {
    if (g_CbnDsound == 0 || g_CbnBgmEnabled == 0 || g_CbnSoundMgr == 0) {
        return -1;
    }
    if ((g_CbnSoundFlags & 0x10) == 0) {
        return CbnPlayBgmFallback(g_CbnBgmNames[slot]);
    }
    if (g_CbnBgmBufs[slot] == 0) {
        return -1;
    }
    CbnLogPrintf("bgm %d", slot);
    CbnWaveHdr* hdr = g_CbnBgmHdrs[slot];
    unsigned int blockAlign = hdr->blockAlign2c;
    unsigned int bytes = (hdr->sampleRate24 * blockAlign * 4) >> 4;
    g_CbnBgmEvent = CreateEventA(0, FALSE, FALSE, 0);
    g_CbnBgmThread = CreateThread(0, 0, CbnBgmThreadProc, g_CbnSoundParam, 0, &g_CbnBgmThreadId);
    bytes -= bytes % blockAlign;
    if (g_CbnStreaming.CreateFromMem(g_CbnDsound, g_CbnBgmBufs[slot], g_CbnBgmHdrs[slot], g_CbnGuid, g_CbnBgmEvent, bytes, g_CbnBgmSizes[slot]) < 0) {
        return -1;
    }
    g_CbnCurBgmSlot = slot;
    return 0;
}

} // namespace th128
