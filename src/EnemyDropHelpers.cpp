#include "EnemyDropHelpers.hpp"

#include <string.h>

int DropIsPos::IsPositive() { return v > 0; }
void DropSet0::Set(int x) { v = x; }
int DropGet0::Get() { return v; }
void DropArr::Set(int idx, int v) { a[idx] = v; }
int *DropOff34::Get() { return &f; }
int *DropOff3E3C::Get() { return &f; }
int *DropOff5004::Get() { return &f; }
int *DropOff11C8::Get() { return &f; }
void DropFlag30::ClearBit0() { flags &= ~1U; }
int DropObj17C::Get() { return v; }
int DropDerefOuter::Get() { return sub->val; }

void DropFlag14::Reset()
{
    flags14 &= ~2U;
    a = 0;
    b = 0;
    c = 0;
}

void DropState::Commit()
{
    if (state == 2)
    {
        state = 4;
    }
    saved = cur;
    cur = 0;
}

void DropArea::SetRadius(float rx, float ry)
{
    radiusX = rx;
    radiusY = ry;
}

void DropArea::Reset()
{
    memset(this, 0, sizeof(DropArea));
    radiusY = 32.0f;
    radiusX = 32.0f;
    mainItem = 0;
}

void DropArea::ClearCounts()
{
    memset(counts, 0, sizeof(counts));
}

DropEnemySub380::DropEnemySub380()
{
    memset(this, 0, sizeof(DropEnemySub380));
    field374 = -1;
}

DropEnemyCore::DropEnemyCore()
{
    fl27C &= ~1U;
    fl290 &= ~1U;
    fl2E0 &= ~1U;
    fl32C &= ~1U;
    fl368 &= ~1U;
    fl3A4 &= ~1U;
    fl3E0 &= ~1U;
    fl41C &= ~1U;
    fl458 &= ~1U;
    fl494 &= ~1U;
    fl3EB0 &= ~1U;
    fl3EC4 &= ~1U;
    fl3EDC &= ~1U;
}

int DropEclFile::Load(const char *name)
{
    g_DropPathBuf[0] = '\0';
    strcat(g_DropPathBuf, name);
    void *buf = DropReadFile(g_DropPathBuf, 0, 0);
    return (Parse(buf) >= 0) ? 0 : -1;
}

void DropEnemyMgr::SetArr80(int idx, int val)
{
    g_DropEnemyMgr->arr80[idx] = val;
}

void DropEnemyMgr::SetArr5C(int idx, int val)
{
    g_DropEnemyMgr->arr5C[idx] = val;
}

void __stdcall DropEnemyMgr::SetFlag7CBit0(int bit)
{
    g_DropEnemyMgr->flags7C ^= (g_DropEnemyMgr->flags7C ^ (unsigned int)bit) & 1U;
}

int DropEnemyMgr::HasEnemyId(int id)
{
    if (id != 0)
    {
        for (DropEnemyNode *n = g_DropEnemyMgr->headB8; n != NULL; n = n->next)
        {
            if (n->obj->id == id)
            {
                return 1;
            }
        }
    }
    return 0;
}

DropEnemyObj *DropEnemyMgr::FindEnemyById(int id)
{
    if (id != 0)
    {
        for (DropEnemyNode *n = g_DropEnemyMgr->headB8; n != NULL; n = n->next)
        {
            if (n->obj->id == id)
            {
                return n->obj;
            }
        }
    }
    return NULL;
}

int DropEnemyMgr::GetLastId() { return g_DropEnemyMgr->lastId; }
int DropEnemyMgr::GetNextId() { return g_DropEnemyMgr->nextId; }

int DropEnemyMgr::AllocId()
{
    DropEnemyMgr *m = g_DropEnemyMgr;
    m->lastId = m->nextId;
    ++m->nextId;
    if (m->nextId == 0)
    {
        m->nextId = 1;
    }
    return m->lastId;
}

int Drop4B8A80::GetF78() { return g_Drop4B8A80->fieldF78; }

void DropGuiMini::SetEntry(int row, int col, float f, int v)
{
    DropGuiRow *rows = (DropGuiRow *)g_DropGui->table5918;
    rows[row].cols[col].f = f;
    rows[row].cols[col].v = v;
}

int DropGuiMini::HasActiveObj() { return activeObj != NULL; }
int DropGuiMini::GetActiveObjField17C() { return g_DropGui->activeObj->v; }

void DropGuiMini::SetTimerPair(int a, int b)
{
    DropGuiMini *g = g_DropGui;
    g->field5A24 = (a > 99) ? 99 : a;
    g->field5A28 = (a > 99) ? 99 : b;
}

int Drop4B8934::IsFlag0And6()
{
    unsigned int fl = g_Drop4B8934->flags7C;
    if ((fl & 1U) != 0 && (fl & 0x40U) != 0)
    {
        return 1;
    }
    return 0;
}

void Drop4B8934::SetFlag4() { g_Drop4B8934->flags7C |= 0x10U; }
int Drop4B8934::GetFlag4() { return (g_Drop4B8934->flags7C >> 4) & 1U; }

void DropObj50::SetVec3(const DropVec3 *v) { pos50 = *v; }
void DropObj50::SetF68(float v) { f68 = v; }
void DropObj50::SetF70(float v) { f70 = v; }
void DropObj50::SetF74(float v) { f74 = v; }
void DropObj50::Clear80() { field80 = 0; }

DropZero354A::DropZero354A() { memset(this, 0, sizeof(DropZero354A)); }
DropZero354B::DropZero354B() { memset(this, 0, sizeof(DropZero354B)); }
DropZero354C::DropZero354C() { memset(this, 0, sizeof(DropZero354C)); }

DropZero374::DropZero374()
{
    memset(this, 0, sizeof(DropZero374));
    f2C = 8.0f;
}

void DropObj5CC::SetVec3(const DropVec3 *v) { pos5CC = *v; }
void DropObj5CC::SetF5DC(float v) { f5DC = v; }
