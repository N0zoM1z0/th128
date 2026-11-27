#include "ZeroCallMedium3.hpp"

#include <math.h>
#include <mmsystem.h>
#include <string.h>

namespace th128 {

static inline Zc3AnmVm *Zc3FindVmById(int id) {
    if (id == 0) return 0;
    for (Zc3AnmListNode *n = g_Zc3AnmMgr->worldListHead; n != 0; n = n->next) {
        if (n->vm->id == id) return n->vm;
    }
    for (Zc3AnmListNode *n = g_Zc3AnmMgr->uiListHead; n != 0; n = n->next) {
        if (n->vm->id == id) return n->vm;
    }
    return 0;
}

static inline void Zc3InterruptVmAndChildren(Zc3AnmVm *vm, short intr) {
    if (vm == 0) return;
    vm->Interrupt(intr);
    if (vm->childNode.prev == 0) {
        for (Zc3AnmListNode *c = vm->childNode.next; c != 0; c = c->next) {
            c->vm->Interrupt(intr);
        }
    }
}

void __stdcall Zc3PlayerStartAllOptionAnms(Zc3Player *player) {
    player->flags &= ~0x10u;
    for (int i = 0; i < 8; ++i) {
        Zc3InterruptVmAndChildren(Zc3FindVmById(player->options[i].anmIdA), 2);
        Zc3InterruptVmAndChildren(Zc3FindVmById(player->options[i].anmIdB), 2);
    }
    player->field438f0 = 0;
}

void Zc3TimingThreadProc() {
    g_Zc3TimerFlags |= 2;
    while ((g_Zc3TimerFlags & 1) == 0) {
        if (g_Zc3SupFlags3f8 & 0x8000) {
            EnterCriticalSection(&g_Zc3CritSec);
            g_Zc3CritDepth += 1;
        }
        double t;
        if (g_Zc3QpcFreq.QuadPart != 0) {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            double elapsed = (double)(now.QuadPart - g_Zc3QpcStart.QuadPart) / (double)g_Zc3QpcFreq.QuadPart;
            if (elapsed < g_Zc3TimeBase) {
                g_Zc3TimeBase = elapsed;
            }
            if (g_Zc3SupFlags3f8 & 0x8000) {
                LeaveCriticalSection(&g_Zc3CritSec);
                g_Zc3CritDepth -= 1;
            }
            t = elapsed - g_Zc3TimeBase;
        } else {
            double ms = (double)timeGetTime();
            if (ms < g_Zc3TimeBase) {
                g_Zc3TimeBase = ms;
            }
            t = (ms - g_Zc3TimeBase * 1000.0) / 1000.0;
            if (g_Zc3SupFlags3f8 & 0x8000) {
                LeaveCriticalSection(&g_Zc3CritSec);
                g_Zc3CritDepth -= 1;
            }
        }
        g_Zc3CurTime = t;
        if (t < g_Zc3PrevTime) {
            g_Zc3NextFrameTime = t;
        }
        g_Zc3PrevTime = t;
        if (t >= g_Zc3NextFrameTime) {
            do {
                g_Zc3NextFrameTime += 1.0 / 60.0;
            } while (g_Zc3NextFrameTime <= t);
            g_Zc3TickCount += 1;
            Sleep(13);
        }
    }
}

void Zc3ReplayStageInit() {
    Zc3ReplayMgr *mgr = g_Zc3ReplayMgr;
    if (mgr->mode == 0) {
        Zc3ReplayStageData *d = new Zc3ReplayStageData;
        if (d != 0) {
            memset(d, 0, sizeof(*d));
        } else {
            d = 0;
        }
        mgr->recStages[g_Zc3StageNum] = d;
        Zc3ReplayStageData *cur = mgr->recStages[g_Zc3StageNum];
        cur->rngSeed = g_Zc3RngSeed;
        g_Zc3RngCount = 0;
        cur->stageNum = (unsigned short)g_Zc3StageNum;
        cur->flags = (cur->flags & ~1u) | (g_Zc3SupFlags3cc & 1u);
    } else if (mgr->mode == 1) {
        Zc3ReplayStageSlot *slot = &mgr->playStages[g_Zc3StageNum];
        Zc3ReplayStageData *d = slot->data;
        slot->f04 = slot->f00;
        slot->f0c = slot->f08;
        slot->f14 = -1;
        g_Zc3RngSeed = d->rngSeed;
        g_Zc3RngCount = 0;
        g_Zc3Score = d->score;
        g_Zc3Power = d->power;
        if (g_Zc3Power > g_Zc3PowerMax) {
            g_Zc3Power = g_Zc3PowerMax;
        } else if (g_Zc3Power < g_Zc3PowerMin) {
            g_Zc3Power = g_Zc3PowerMin;
        }
        g_Zc3Graze = d->graze;
        g_Zc3Continues = d->continues;
        g_Zc3FreezeVal = d->freezeVal;
        g_Zc3FreezeSub = 0;
        if (g_Zc3FreezeVal > 100000) {
            g_Zc3FreezeVal = 100000;
        }
        g_Zc3FieldD70 = d->field84;
        g_Zc3FieldD60 = d->field88;
    }
}

bool __fastcall Zc3BlitSurface(Zc3FontSurface *self, int x, Zc3D3DSurf *surf, int y, int w, int h) {
    if (self->initialized == 0) {
        return false;
    }
    Zc3SurfDesc desc;
    surf->vtbl->GetDesc(surf, &desc);
    RECT r;
    r.left = 0;
    r.top = 0;
    r.right = self->width;
    r.bottom = self->height;
    Zc3LockedRect locked;
    if (surf->vtbl->LockRect(surf, &locked, &r, 0) != 0) {
        return false;
    }
    int srcPitch = self->pitch;
    unsigned char *src = self->pixels;
    int dstPitch = locked.pitch;
    if (desc.format == self->format) {
        switch (self->format) {
        case 26: {
            unsigned char *dst = (unsigned char *)locked.pBits + y * dstPitch + x * 2;
            for (int row = 0; row < h; ++row) {
                int col = 0;
                for (; col < w; ++col) {
                    int da = dst[0] >> 4;
                    int sa = src[1] >> 4;
                    int sb = src[0] >> 4;
                    dst[0] = (unsigned char)((da + (((sb - da) * sa) >> 4)) << 4);
                    int dg = dst[0] & 0xf;
                    int sg = src[0] & 0xf;
                    dst[0] |= (unsigned char)(dg + (((sg - dg) * sa) >> 4));
                    int dstHi = dst[1];
                    int newA = ((dstHi >> 2) & 0x3c) + sa;
                    if (newA >= 16) newA = 15;
                    int dr = dstHi & 0xf;
                    int sr = src[1] & 0xf;
                    dst[1] = (unsigned char)((dr + (((sr - dr) * sa) >> 4)) | (newA << 4));
                    dst += 2;
                    src += 2;
                }
                src += srcPitch - col * 2;
                dst += dstPitch - col * 2;
            }
            break;
        }
        case 25: {
            unsigned short *dst = (unsigned short *)((unsigned char *)locked.pBits + y * dstPitch + x * 2);
            unsigned short *s16 = (unsigned short *)src;
            for (int row = 0; row < h; ++row) {
                int col = 0;
                for (; col < w; ++col) {
                    unsigned short px = *s16;
                    if (px & 0x8000) {
                        *dst = px;
                    }
                    ++dst;
                    ++s16;
                }
                s16 = (unsigned short *)((unsigned char *)s16 + (srcPitch - col * 2));
                dst = (unsigned short *)((unsigned char *)dst + (dstPitch - col * 2));
            }
            break;
        }
        case 21: {
            unsigned char *dst = (unsigned char *)locked.pBits + y * dstPitch + x * 4;
            for (int row = 0; row < h; ++row) {
                int col = 0;
                for (; col < w; ++col) {
                    int a = src[3];
                    dst[0] = (unsigned char)(dst[0] + (((src[0] - dst[0]) * a) >> 8));
                    dst[1] = (unsigned char)(dst[1] + (((src[1] - dst[1]) * a) >> 8));
                    dst[2] = (unsigned char)(dst[2] + (((src[2] - dst[2]) * a) >> 8));
                    dst += 4;
                    src += 4;
                }
                src += srcPitch - col * 4;
                dst += dstPitch - col * 4;
            }
            break;
        }
        default:
            return false;
        }
    }
    surf->vtbl->UnlockRect(surf);
    return true;
}

int __stdcall Zc3HitTestCircleOrientedRect(float cx, float cy, float w, float h, float angle, float px, float py, float r) {
    float dx = px - cx;
    float dy = py - cy;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float rx = c * dx - s * dy;
    float ry = s * dx + c * dy;
    float halfW = (float)(w * 0.5);
    float halfH = (float)(h * 0.5);
    if (fabsf(rx) <= halfW + r && fabsf(ry) <= halfH) {
        return 1;
    }
    if (fabsf(rx) <= halfW && fabsf(ry) <= halfH + r) {
        return 1;
    }
    float rr = r * r;
    float vx = rx - halfW;
    float vy = ry - halfH;
    if (vx * vx + vy * vy <= rr) return 1;
    vx = rx + halfW;
    vy = ry - halfH;
    if (vx * vx + vy * vy <= rr) return 1;
    vx = rx - halfW;
    vy = ry + halfH;
    if (vx * vx + vy * vy <= rr) return 1;
    vx = rx + halfW;
    vy = ry + halfH;
    if (vx * vx + vy * vy <= rr) return 1;
    return 0;
}

unsigned int __fastcall Zc3PollJoystickButtons(unsigned int keys) {
    if ((g_Zc3SupFlags3f8 & 0x800) == 0) {
        JOYINFOEX ji;
        memset(&ji, 0, sizeof(ji));
        ji.dwSize = sizeof(ji);
        ji.dwFlags = JOY_RETURNALL;
        if (joyGetPosEx(0, &ji) != 0) {
            return keys;
        }
        unsigned int btns = ji.dwButtons;
        if (g_Zc3JoyBtnShot >= 0) {
            keys |= (btns & (1u << g_Zc3JoyBtnShot)) ? 1u : 0u;
        }
        if (g_Zc3JoyBtnBomb >= 0) {
            keys |= (btns & (1u << g_Zc3JoyBtnBomb)) ? 2u : 0u;
        }
        if (g_Zc3JoyBtnSkip >= 0) {
            keys |= (btns & (1u << g_Zc3JoyBtnSkip)) ? 0x100u : 0u;
        }
        if (g_Zc3JoyBtnSlow >= 0) {
            keys |= (btns & (1u << g_Zc3JoyBtnSlow)) ? 8u : 0u;
        }
        if (g_Zc3JoyBtnPause >= 0) {
            keys |= (btns & (1u << g_Zc3JoyBtnPause)) ? 0x200u : 0u;
        }
        unsigned int xCenter = (g_Zc3JoyXMax + g_Zc3JoyXMin) >> 1;
        unsigned int xRange = (g_Zc3JoyXMax - g_Zc3JoyXMin) >> 2;
        if (ji.dwXpos > xCenter + xRange) keys |= 0x80u;
        if (ji.dwXpos < xCenter - xRange) keys |= 0x40u;
        unsigned int yCenter = (g_Zc3JoyYMax + g_Zc3JoyYMin) >> 1;
        unsigned int yRange = (g_Zc3JoyYMax - g_Zc3JoyYMin) >> 2;
        if (ji.dwYpos > yCenter + yRange) keys |= 0x20u;
        if (ji.dwYpos < yCenter - yRange) keys |= 0x10u;
        return keys;
    }

    if (g_Zc3JoyDev->vtbl->Poll(g_Zc3JoyDev) < 0) {
        int retries = 0;
        HRESULT hr = g_Zc3JoyDev->vtbl->Acquire(g_Zc3JoyDev);
        while (hr == (HRESULT)0x8007001e) {
            hr = g_Zc3JoyDev->vtbl->Acquire(g_Zc3JoyDev);
            Zc3DebugLog("error : DIERR_INPUTLOST %d\r\n", retries);
            if (++retries >= 400) {
                return keys;
            }
        }
        return keys;
    }

    Zc3JoyState state;
    memset(&state, 0, sizeof(state));
    if (g_Zc3JoyDev->vtbl->GetDeviceState(g_Zc3JoyDev, sizeof(state), &state) < 0) {
        return keys;
    }
    if (g_Zc3JoyBtnShot >= 0) {
        keys |= (state.rgbButtons[g_Zc3JoyBtnShot] & 0x80) ? 1u : 0u;
    }
    if (g_Zc3JoyBtnBomb >= 0) {
        keys |= (state.rgbButtons[g_Zc3JoyBtnBomb] & 0x80) ? 2u : 0u;
    }
    if (g_Zc3JoyBtnSkip >= 0) {
        keys |= (state.rgbButtons[g_Zc3JoyBtnSkip] & 0x80) ? 0x100u : 0u;
    }
    if (g_Zc3JoyBtnSlow >= 0) {
        keys |= (state.rgbButtons[g_Zc3JoyBtnSlow] & 0x80) ? 8u : 0u;
    }
    if (g_Zc3JoyBtnPause >= 0) {
        keys |= (state.rgbButtons[g_Zc3JoyBtnPause] & 0x80) ? 0x200u : 0u;
    }
    if (state.lX < -g_Zc3JoyDeadX) keys |= 0x40u;
    if (state.lY < -g_Zc3JoyDeadY) keys |= 0x10u;
    if (state.lX > g_Zc3JoyDeadX) keys |= 0x80u;
    if (state.lX > g_Zc3JoyDeadY) keys |= 0x20u;
    return keys;
}

} // namespace th128
