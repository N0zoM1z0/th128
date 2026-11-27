#include "TitleArchiveHelpers.hpp"

#include <stdlib.h>
#include <string.h>

void TaAnmVm::SetFloat60Flag3(float v)
{
    flags4B0 |= 8U;
    field60 = v;
}

void TaAnmVm::SetFloat468(float v) { field468 = v; }
void TaAnmVm::SetFloat464(float v) { field464 = v; }
void TaNoOp444130() {}
int TaNode4::IsNextNonZero() { return sub04->field04 != 0; }

void TaStringList::FreeAll()
{
    if (arr34 != NULL)
    {
        for (int i = 0; i < count38; ++i)
        {
            if (arr34[i] != NULL)
            {
                free(arr34[i]);
                arr34[i] = NULL;
            }
        }
        if (arr34 != NULL)
        {
            free(arr34);
            arr34 = NULL;
        }
        arr34 = NULL;
    }
}

TaStaffMgr::TaStaffMgr()
{
    memset(this, 0, sizeof(TaStaffMgr));
    flags00 |= 2U;
    g_TaStaffMgr = this;
}

TaBuf8C *TaBuf8C::DeleteSelf()
{
    vtbl = &g_TaBuf8CVtbl;
    if (buf8C != NULL)
    {
        free(buf8C);
        buf8C = NULL;
    }
    operator delete(this);
    return this;
}

TaStaffMgr *TaStaffMgr::Create()
{
    TaStaffMgr *m = new TaStaffMgr();
    if (TaStaffInit(m) != 0)
    {
        if (m != NULL)
        {
            TaStaffDtor(m);
            operator delete(m);
        }
        return NULL;
    }
    return m;
}

TaStaffMgr *TaStaffMgr::DestroySelf()
{
    TaStaffDtor(this);
    operator delete(this);
    return this;
}

void TaStaffMgr::DestroyPtr(TaStaffMgr *m)
{
    if (m != NULL)
    {
        TaStaffDtor(m);
        operator delete(m);
    }
}

void TaNoOp444CB0() {}

int TaRouteStageToIndex(int route, int stage)
{
    if (route == 0)
    {
        return stage + 1;
    }
    if (route == 1)
    {
        return stage == 0 ? 1 : stage + 3;
    }
    if (route == 2)
    {
        return stage + 6;
    }
    if (route == 3)
    {
        return stage == 0 ? 6 : stage + 8;
    }
    if (route == 4)
    {
        return stage + 11;
    }
    return stage == 0 ? 11 : stage + 13;
}

int TaGetTitleSize() { return 0x5BEC; }

void TaTitleMgrExact::SetStateAndResetTimer(int newState)
{
    prevState20 = state1C;
    state1C = newState;
    subState24 = 0;
    if ((timer2B4.flags & 1U) == 0)
    {
        timer2B4.currentF = 0.0f;
        timer2B4.current = 0;
        timer2B4.previous = -999999;
        timer2B4.gameSpeed = &g_TaDefaultTimeScale;
        timer2B4.flags |= 1U;
    }
    timer2B4.currentF = 0.0f;
    timer2B4.current = 0;
    timer2B4.previous = -1;
}

void TaTitleMgrExact::SetSubStateAndResetTimer(int newSub)
{
    subState24 = newSub;
    if ((timer2B4.flags & 1U) == 0)
    {
        timer2B4.currentF = 0.0f;
        timer2B4.current = 0;
        timer2B4.previous = -999999;
        timer2B4.gameSpeed = &g_TaDefaultTimeScale;
        timer2B4.flags |= 1U;
    }
    timer2B4.currentF = 0.0f;
    timer2B4.current = 0;
    timer2B4.previous = -1;
}

int TaMother::Get528() { return g_TaMother->field528; }
int TaScoreMgrExact::IsClearFlag(int idx) { return clearFlags337E[idx] & 0x10; }

int TaScoreMgrExact::AreAll6Cleared()
{
    TaScoreMgrExact *s = g_TaScoreMgr;
    if ((s->clearFlags337E[0] & 0x10) && (s->clearFlags337E[1] & 0x10)
        && (s->clearFlags337E[2] & 0x10) && (s->clearFlags337E[3] & 0x10)
        && (s->clearFlags337E[4] & 0x10) && (s->clearFlags337E[5] & 0x10))
    {
        return 1;
    }
    return 0;
}

int __stdcall TaScoreMgrExact::GetTable660(int row, int col)
{
    return g_TaScoreMgr->table660[row * 0x19B + col];
}

void TaScoreMgrExact::SetAllClearFlags()
{
    TaScoreMgrExact *s = g_TaScoreMgr;
    for (int i = 0; i < 16; ++i)
    {
        s->clearFlags337E[i] = 0x11;
    }
}

void __stdcall TaHelpMgr::SetFloat12C(float v) { g_TaHelpMgr->field12C = v; }

TaTitleMgrExact::TaTitleMgrExact()
{
    memset(this, 0, sizeof(TaTitleMgrExact));
    flags04 |= 2U;
    g_TaTitleMgr = this;
}

void TaTitleMgrExact::DestroyGlobal()
{
    TaTitleMgrExact *m = g_TaTitleMgr;
    if (m != NULL)
    {
        TaTitleDtor(m);
        operator delete(m);
    }
}

TaTitleMgrExact *TaTitleMgrExact::DestroySelf()
{
    TaTitleDtor(this);
    operator delete(this);
    return this;
}

int TaRet1_44B3B0() { return 1; }
int TaRet1_44B3C0() { return 1; }
int TaRet1_44B3D0() { return 1; }

const char *TaSkipLine(const char *p, int *rem)
{
    while (*p != '\n' && *p != '\r')
    {
        if (*rem == 0)
        {
            return p;
        }
        ++p;
        --(*rem);
    }
    if (*rem == 0)
    {
        return p;
    }
    while ((*p == '\n' || *p == '\r') && *rem != 0)
    {
        ++p;
        --(*rem);
    }
    return p;
}

char *__stdcall TaReadLine(char *p, int *rem, char *out)
{
    char *start = p;
    while (*p != '\n' && *p != '\r')
    {
        if (*rem == 0)
        {
            return p;
        }
        ++p;
        --(*rem);
    }
    if (*rem == 0)
    {
        return p;
    }
    *p = '\0';
    strcpy(out, start);
    ++p;
    --(*rem);
    while ((*p == '\n' || *p == '\r') && *rem != 0)
    {
        ++p;
        --(*rem);
    }
    return p;
}

int TaObj04::GetField04() { return f04; }
int TaObj04::IsPos0() { return f00 > 0; }
void TaObjD4::ZeroD4() { fieldD4 = 0; }

int TaRouteStageToIndexSwitch(int route, int stage)
{
    switch (route)
    {
    case 0: return stage + 1;
    case 1: return stage == 0 ? 1 : stage + 3;
    case 2: return stage + 6;
    case 3: return stage == 0 ? 6 : stage + 8;
    case 4: return stage + 11;
    case 5: return stage == 0 ? 11 : stage + 13;
    case 6: return 16;
    default: return 0;
    }
}

void TaClearFlag4B4D50Bit4() { g_Ta4B4D50 &= ~0x10U; }
int TaObj04::GetPlus7() { return f00 + 7; }
int TaAnmMgr::GetSlot4E90C0(int idx) { return slots4E90C0[idx]; }
void TaNoOp44F590() {}

TaArchive::TaArchive()
{
    f00 = 0;
    f04 = 0;
    f08 = 0;
    f0C = 0;
}

int TaEntryTable::FindSize(const char *name)
{
    TaEntryItem *it = items00;
    if (it != NULL)
    {
        for (int i = count04; i > 0; --i, ++it)
        {
            if (strcmp(name, it->name00) == 0)
            {
                return it->size08;
            }
        }
    }
    return 0;
}

TaEntryItem *TaEntryTable::FindEntry(const char *name)
{
    TaEntryItem *it = items00;
    if (it == NULL)
    {
        return NULL;
    }
    for (int i = count04; i > 0; --i, ++it)
    {
        if (strcmp(name, it->name00) == 0)
        {
            return it;
        }
    }
    return NULL;
}

char *TaStrDupMalloc(const char *s)
{
    char *p = (char *)malloc(strlen(s) + 1);
    if (p != NULL)
    {
        strcpy(p, s);
    }
    return p;
}

int TaStreamAdvance4(int **pp)
{
    *pp += 1;
    return **pp;
}

char *TaStreamSkipAlignedStr(char **pp)
{
    int len = (int)strlen(*pp) + 1;
    if ((len % 4) != 0)
    {
        len += 4 - (len % 4);
    }
    *pp += len;
    return *pp;
}

void TaNoOp44FD40() {}
void TaNoOp44FD50() {}

TaStreamBase::TaStreamBase()
{
    vtbl = &g_TaStreamBaseVtbl;
    f04 = -1;
    f08 = 0;
}

void TaResolvePath(char *dst, const char *src)
{
    if (strchr(src, ':') != NULL)
    {
        strcpy(dst, src);
        return;
    }
    g_TaGetModuleFileNameA(NULL, dst, 0x104);
    char *slash = strrchr(dst, '\\');
    if (slash == NULL)
    {
        *dst = '\0';
    }
    slash[1] = '\0';
    strcat(dst, src);
}

void TaVtbl850A::SetVtblA() { vtbl = &g_TaVtbl850; }
void TaVtbl850B::SetVtblB() { vtbl = &g_TaVtbl850; }

const char *TaStripDirSlash(const char *s)
{
    const char *p = strchr(s, '/');
    return p != NULL ? p + 1 : s;
}
