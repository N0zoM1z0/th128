#include "ZeroCallMedium11.hpp"

#include <stdio.h>
#include <string.h>

float g_Zc11TimeScale;
Zc11Float3 g_Zc11CamPos;
Zc11AnmMgr *g_Zc11AnmMgr;

int Zc11PopcountIceBitmap(const Zc11IceField *field) {
    int total = 0;
    const unsigned int *p = field->bitmap;
    for (int i = 0; i < 448 * 12; ++i) {
        unsigned int a = p[i];
        a = a - ((a >> 1) & 0x55555555u);
        a = (a & 0x33333333u) + ((a >> 2) & 0x33333333u);
        a = (a + (a >> 4)) & 0x0f0f0f0fu;
        a = a + (a >> 8);
        total += (a + (a >> 16)) & 0x3fu;
    }
    return total;
}

Zc11EffectSlot *Zc11EffectMgr::AllocSlot(const Zc11Float3 *pos, float a, float b, int startTime, int param60, int param70) {
    int idx = cursor;
    Zc11EffectSlot *slot = &slots[idx];
    idx = (idx + 1) % 2000;
    cursor = idx;
    for (int i = 0; i < 2000; ++i) {
        if ((slot->flags & 1) == 0) {
            memset(slot, 0, sizeof(Zc11EffectSlot));
            slot->flags |= 3;
            memset(&slot->pos, 0, 0x34);
            slot->pos = *pos;
            slot->f00 = a;
            slot->f04 = b;
            if ((slot->timerFlags & 1) == 0) {
                slot->timerCurF = 0.0f;
                slot->timerCur = 0;
                slot->timerPrev = -999999;
                slot->timerScale = &g_Zc11TimeScale;
                slot->timerFlags |= 1;
            }
            slot->timerCur = startTime;
            slot->timerCurF = (float)startTime;
            slot->timerPrev = startTime - 1;
            slot->f64 = 0;
            slot->f60 = param60;
            slot->f68 = 999999;
            slot->f6c = 4;
            slot->f70 = param70;
            return slot;
        }
        if (idx == 0) {
            slot = &slots[0];
        } else {
            ++slot;
        }
        idx = (idx + 1) % 2000;
        cursor = idx;
    }
    return slot;
}

int __stdcall Zc11CheckDebugKey(Zc11DebugBuf *buf, const char *key, int v1, int v2) {
    const char *p = buf->text;
    if (p == 0) {
        return 0;
    }
    int rem = buf->len;
    if (strncmp(key, "debug", 5) == 0 || strcmp("0100a", "debug") == 0) {
        return 0;
    }
    while (rem != 0) {
        if (strncmp(key, p, 5) == 0) {
            int a, b;
            p += 6;
            sscanf(p, "%d %d", &a, &b);
            if (a == v1 && b == v2) {
                return 0;
            }
        }
        const char *nl = strchr(p, '\n');
        const char *next = nl + 1;
        rem += (int)(p - next);
        p = next;
    }
    return -1;
}

void __stdcall Zc11InitBulletTimer(Zc11BulletObj *obj, const Zc11Float3 *v1, const Zc11Float3 *v2, int paramE8, unsigned char paramEC) {
    obj->fE8 = paramE8;
    obj->fBC = g_Zc11CamPos;
    obj->fC8 = g_Zc11CamPos;
    obj->fEC = paramEC;
    obj->fA4 = *v1;
    obj->fB0 = *v2;
    if ((obj->timerFlags & 1) == 0) {
        obj->timerCurF = 0.0f;
        obj->timerCur = 0;
        obj->timerPrev = -999999;
        obj->timerScale = &g_Zc11TimeScale;
        obj->timerFlags |= 1;
    }
    obj->timerCurF = 0.0f;
    obj->timerCur = 0;
    obj->timerPrev = -1;
}

bool Zc11BlitToSurface(Zc11ImgBuf *img, Zc11Surface *surf) {
    if (img->valid == 0) {
        return false;
    }
    Zc11SurfaceDesc desc;
    surf->vtbl->GetDesc(surf, &desc);
    Zc11Rect rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = img->width;
    rc.bottom = img->height;
    Zc11LockedRect locked;
    if (surf->vtbl->LockRect(surf, &locked, &rc, 0) != 0) {
        return false;
    }
    int dstPitch = locked.Pitch;
    int srcPitch = img->pitch;
    unsigned char *src = img->pixels;
    unsigned char *dst = (unsigned char *)locked.pBits;
    if (desc.Format == img->format) {
        for (int y = 0; y < img->height; ++y) {
            memcpy(dst, src, srcPitch);
            src += srcPitch;
            dst += dstPitch;
        }
    }
    surf->vtbl->UnlockRect(surf);
    return true;
}

bool Zc11CopyFromSurface(Zc11ImgBuf *img, Zc11Surface *surf) {
    if (img->valid == 0) {
        return false;
    }
    Zc11SurfaceDesc desc;
    surf->vtbl->GetDesc(surf, &desc);
    Zc11Rect rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = img->width;
    rc.bottom = img->height;
    Zc11LockedRect locked;
    surf->vtbl->LockRect(surf, &locked, &rc, 0x10);
    int srcPitch = locked.Pitch;
    int dstPitch = img->pitch;
    unsigned char *dst = img->pixels;
    unsigned char *src = (unsigned char *)locked.pBits;
    if (desc.Format == img->format) {
        for (int y = 0; y < img->height; ++y) {
            memcpy(dst, src, dstPitch);
            src += srcPitch;
            dst += dstPitch;
        }
    }
    surf->vtbl->UnlockRect(surf);
    return true;
}

int __stdcall Zc11FindSoundByName(const char *path, Zc11SoundTable *tbl) {
    char name[128];
    const char *p = strchr(path, '/');
    if (p == 0) {
        p = strchr(path, '\\');
    }
    if (p != 0) {
        strcpy(name, p + 1);
    } else {
        strcpy(name, path);
    }
    Zc11SoundEntry *entries = tbl->entries;
    int i = 0;
    while (entries[i].name[0] != 0) {
        if (strcmp(entries[i].name, name) == 0) {
            break;
        }
        ++i;
    }
    if (entries[i].name[0] == 0) {
        return 0;
    }
    return i;
}

void __stdcall Zc11CleanupStageAnms(Zc11StageObj *obj) {
    for (int i = 0; i < 8; ++i) {
        int id = g_Zc11AnmMgr->ids[i];
        for (Zc11AnmNode *n = g_Zc11AnmMgr->list1; n != 0;) {
            Zc11AnmNode *next = n->next;
            Zc11AnmVm *vm = n->vm;
            if (vm->anmId == id) {
                vm->flags |= 0x20000000u;
            }
            n = next;
        }
        for (Zc11AnmNode *n = g_Zc11AnmMgr->list2; n != 0;) {
            Zc11AnmNode *next = n->next;
            Zc11AnmVm *vm = n->vm;
            if (vm->anmId == id) {
                vm->flags |= 0x20000000u;
            }
            n = next;
        }
    }
    for (Zc11VirtNode *n = obj->head; n != 0;) {
        Zc11VirtNode *next = n->next;
        if (n->obj != 0) {
            n->obj->v5(1);
        }
        n = next;
    }
    obj->fC4 = 0;
    obj->f5C = 0;
    if (obj->node1 != 0) {
        obj->node1->flags &= ~2u;
    }
    if (obj->node2 != 0) {
        obj->node2->flags &= ~2u;
    }
}
