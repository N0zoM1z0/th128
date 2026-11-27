#include "AsciiInfCore.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AsciiEntryId *AsciiInfCore::AddFormattedInt(int val, const AsciiVec3 *pos, AsciiEntryId *outId)
{
    char buf[256];
    AsciiInfCore *inst = g_AsciiInfCore;
    if (val < 1000)
    {
        sprintf(buf, "%d", val);
    }
    else if (val < 1000000)
    {
        sprintf(buf, "%d,%.3d", val / 1000, val % 1000);
    }
    else if (val < 1000000000)
    {
        sprintf(buf, "%d,%.3d,%.3d", (val / 1000000) % 1000, (val / 1000) % 1000, val % 1000);
    }
    else
    {
        sprintf(buf, "%d,%.3d,%.3d,%.3d",
            (val / 1000000000) % 1000, (val / 1000000) % 1000, (val / 1000) % 1000, val % 1000);
    }
    inst->AddString(buf, pos, outId);
    return outId;
}

AsciiEntryId *AsciiInfCore::AddFormattedStr(AsciiEntryId *outId, const AsciiVec3 *pos, const char *fmt, ...)
{
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    AsciiEntryId tmp;
    *outId = *AddString(buf, pos, &tmp);
    entries[entryCount - 1].unk124 = 1;
    return outId;
}

int AsciiInfCore::DrawLayer(int targetLayer)
{
    int curViewport = 1;
    for (int i = 0; i < entryCount; ++i)
    {
        AsciiPopupEntry *e = &entries[i];
        if (e->blendMode == targetLayer)
        {
            if (curViewport != e->shadow)
            {
                curViewport = e->shadow;
                g_AnmManagerMini->FlushSprites();
                if (curViewport != 0)
                {
                    g_SupervisorMini.currentViewport = &g_SupervisorMini.viewports[0];
                    g_SupervisorMini.ApplyCurrentViewport();
                    g_SupervisorMini.device->vtbl->SetViewport(
                        g_SupervisorMini.device,
                        g_SupervisorMini.currentViewport->d3dViewport);
                    g_SupervisorMini.currentViewportIndex = 0;
                }
                else
                {
                    g_SupervisorMini.currentViewport = &g_SupervisorMini.viewports[1];
                    g_SupervisorMini.ApplyCurrentViewport();
                    g_SupervisorMini.device->vtbl->SetViewport(
                        g_SupervisorMini.device,
                        g_SupervisorMini.currentViewport->d3dViewport);
                    g_SupervisorMini.currentViewportIndex = 1;
                }
            }
            DrawEntry(e);
        }
    }
    if (curViewport != 0)
    {
        g_AnmManagerMini->FlushSprites();
        g_SupervisorMini.currentViewport = &g_SupervisorMini.viewports[1];
        g_SupervisorMini.ApplyCurrentViewport();
        g_SupervisorMini.device->vtbl->SetViewport(
            g_SupervisorMini.device,
            g_SupervisorMini.currentViewport->d3dViewport);
        g_SupervisorMini.currentViewportIndex = 1;
    }
    return 1;
}

float __stdcall AsciiMeasureStringWidth(const char *str, int fontId, float charWidth)
{
    float width = 0.0f;
    switch (fontId)
    {
    case 2:
    case 3:
        while (*str != '\0')
        {
            if (*str == '.')
                width += 4.0;
            else
                width += charWidth;
            ++str;
        }
        break;
    case 4:
    case 5:
        while (*str != '\0')
        {
            if (*str == ',')
                width += 8.0;
            else
                width += charWidth;
            ++str;
        }
        break;
    case 6:
    case 7:
        while (*str != '\0')
        {
            if (*str == '.')
                width += 6.0;
            else
                width += charWidth;
            ++str;
        }
        break;
    case 8:
    case 9:
        while (*str != '\0')
        {
            if (*str == 'A')
                width += 6.0;
            else if (*str == 'E')
                width += 6.0;
            else if (*str == ':')
                width += 3.0;
            else if (*str == '/')
                width += 3.0;
            else if (*str == '.')
                width += 3.0;
            else
                width += charWidth;
            ++str;
        }
        break;
    case 10:
    case 11:
        while (*str != '\0')
        {
            if (*str == '.')
                width += 8.0;
            else
                width += charWidth;
            ++str;
        }
        break;
    default:
        width = (float)(strlen(str) * charWidth + 0.0);
        break;
    }
    return width;
}

int AsciiInfCore::DrawLayer1()
{
    g_AnmManagerMini->FlushSprites();
    DrawLayer(1);
    g_AnmManagerMini->FlushSprites();
    g_SupervisorMini.currentViewport = &g_SupervisorMini.viewports[1];
    g_SupervisorMini.ApplyCurrentViewport();
    g_SupervisorMini.device->vtbl->SetViewport(
        g_SupervisorMini.device,
        g_SupervisorMini.currentViewport->d3dViewport);
    g_SupervisorMini.currentViewportIndex = 1;
    return 1;
}

int AsciiInfCore::DrawLayer2()
{
    DrawLayer(2);
    return 1;
}

int AsciiInfCore::OnDraw0()
{
    return DrawLayer1();
}

int AsciiInfCore::OnDraw2()
{
    DrawLayer(2);
    return 1;
}

void AsciiInfCore::DrawFormattedNow(const AsciiVec3 *pos, const char *fmt, ...)
{
    char buf[256];
    AsciiPopupEntry entry;
    entry.timer.flags &= ~1U;

    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    strcpy(entry.text, buf);
    entry.pos = *pos;
    entry.color = color;
    entry.scaleX = scaleX;
    entry.scaleY = scaleY;
    entry.shadow = shadow;
    entry.layer = layer;
    entry.fontId = fontId;
    entry.blendMode = blendMode;
    entry.duration = duration;
    entry.unk13C = unk1C878;
    entry.unk140 = unk1C87C;
    entry.blinkPeriod = blinkPeriod;
    entry.color2 = color2;
    if ((entry.timer.flags & 1U) == 0)
    {
        entry.timer.flags |= 1U;
        entry.timer.gameSpeed = &g_DefaultTimeScaleCore;
    }
    entry.timer.currentF = 0.0f;
    entry.timer.current = 0;
    entry.timer.previous = -1;
    entry.unk148 = unk1C880;
    entry.pauseAffected = pauseAffected;
    DrawEntry(&entry);
}

void AsciiInfCore::DrawIntNow(int val, const AsciiVec3 *pos)
{
    char buf[256];
    if (val < 1000)
    {
        sprintf(buf, "%d", val);
    }
    else if (val < 1000000)
    {
        sprintf(buf, "%d,%.3d", val / 1000, val % 1000);
    }
    else if (val < 1000000000)
    {
        sprintf(buf, "%d,%.3d,%.3d", (val / 1000000) % 1000, (val / 1000) % 1000, val % 1000);
    }
    else
    {
        sprintf(buf, "%d,%.3d,%.3d,%.3d",
            (val / 1000000000) % 1000, (val / 1000000) % 1000, (val / 1000) % 1000, val % 1000);
    }
    DrawFormattedNow(pos, buf);
}

void AsciiInfCore::UpdatePopups()
{
    int outCount = 0;
    for (int i = 0; i < entryCount; ++i)
    {
        AsciiPopupEntry *e = &entries[i];
        AsciiGameManagerCore *gm = g_GameManagerCore;
        if (e->pauseAffected == 0 || gm == NULL || gm->paused == 0)
        {
            if (e->duration < 1000000)
            {
                --e->duration;
            }
        }
        if (e->duration >= 0)
        {
            if (e->blinkPeriod > 0)
            {
                if (e->timer.current % (e->blinkPeriod * 2) < e->blinkPeriod)
                {
                    e->color = e->color1;
                }
                else
                {
                    e->color = e->color2;
                }
            }
            if (e->pauseAffected == 0 || gm == NULL || gm->paused == 0)
            {
                int cur = e->timer.current;
                float *speed = e->timer.gameSpeed;
                e->timer.previous = cur;
                if (*speed > (double)0.99f && *speed < 1.01f)
                {
                    e->timer.current = cur + 1;
                    e->timer.currentF += 1.0f;
                }
                else
                {
                    float nextF = *speed + e->timer.currentF;
                    e->timer.currentF = nextF;
                    e->timer.current = (int)nextF;
                }
            }
            if (outCount != i)
            {
                entries[outCount] = entries[i];
                entries[i].id = 0;
            }
            ++outCount;
        }
    }
    entryCount = outCount;
}

void AsciiSupervisorMini::SetViewport(int index)
{
    currentViewport = &viewports[index];
    ApplyCurrentViewport();
    device->vtbl->SetViewport(device, currentViewport->d3dViewport);
    currentViewportIndex = index;
}

void AsciiInfCore::SetUnk1C878_7C(int a, int b)
{
    unk1C878 = a;
    unk1C87C = b;
}

void AsciiInfCore::ResetUnk1C88C()
{
    unk1C88C = 9;
}

void AsciiNoOp403050() {}

void AsciiFreePtr(void *p)
{
    free(p);
}

void AsciiAnmVmCore::FreeExtraInternal()
{
    if (extraData != NULL)
    {
        free(extraData);
    }
    extraData = NULL;
}

void AsciiAnmVmCore::FreeExtraThiscall()
{
    if (extraData != NULL)
    {
        free(extraData);
    }
    extraData = NULL;
}

AsciiRet0 *AsciiRet0::Self() { return this; }
void AsciiFlag40A::Clear() { flags &= ~1U; }
void AsciiNoOp4030E0() {}
void AsciiFlag20A::Clear() { flags &= ~1U; }
void AsciiFlag30::Clear()  { flags &= ~1U; }
void AsciiNoOp403110() {}
void AsciiFlag40B::Clear() { flags &= ~1U; }
void AsciiFlag20B::Clear() { flags &= ~1U; }
AsciiFlag20R *AsciiFlag20R::Clear() { flags &= ~1U; return this; }
void AsciiNoOp403150() {}
AsciiRet1 *AsciiRet1::Self() { return this; }
void AsciiNoOp403190() {}

void AsciiTimerMini::Reset()
{
    current = 0;
    currentF = 0.0f;
    previous = -999999;
}

static inline AsciiAnmVmCore *FindAnmVmById(int id)
{
    for (AsciiAnmNodeMini *n = g_AnmManagerMini->listA; n != NULL; n = n->next)
    {
        if (n->vm->id == id)
        {
            return n->vm;
        }
    }
    for (AsciiAnmNodeMini *n = g_AnmManagerMini->listB; n != NULL; n = n->next)
    {
        if (n->vm->id == id)
        {
            return n->vm;
        }
    }
    return NULL;
}

static inline void MarkAnmStopped(int *idSlot)
{
    int id = *idSlot;
    if (id != 0)
    {
        AsciiAnmVmCore *found = FindAnmVmById(id);
        if (found != NULL)
        {
            found->flags4B0 |= 0x20000000U;
            if (found->parentPtr == NULL)
            {
                for (AsciiAnmNodeMini *c = found->childHead; c != NULL; c = c->next)
                {
                    c->vm->flags4B0 |= 0x20000000U;
                }
            }
        }
    }
    *idSlot = 0;
}

void StageBgMini::Cleanup()
{
    if (buf10 != NULL)
    {
        free(buf10);
        buf10 = NULL;
    }
    if (buf14 != NULL)
    {
        free(buf14);
        buf14 = NULL;
    }
    for (int i = 0; i < count - 1; ++i)
    {
        MarkAnmStopped(&anmIds[i]);
    }
    if (anmIds != NULL)
    {
        free(anmIds);
        anmIds = NULL;
    }
    if (buf0C != NULL)
    {
        free(buf0C);
        buf0C = NULL;
    }
}

unsigned int StageFlag7C::GetBit0()
{
    return flags & 1U;
}

void StageFlag80::ClearBit0()
{
    flags &= ~1U;
}

StageBgMini *StageBgMini::Destroy(StageBgMini *self)
{
    self->Cleanup();
    operator delete(self);
    return self;
}
