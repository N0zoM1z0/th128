#include "EndingMeshHelpers.hpp"

#include <math.h>
#include <stdlib.h>
#include <string.h>

int EndVmMini::OnMeshUpdate()
{
    ownerMesh->CopyToVms();
    return 0;
}

void EndMesh::SyncVerts()
{
    if (cols == 0)
    {
        return;
    }
    EndMeshVert *v = verts;
    EndVec3 *p = positions;
    for (int i = 0; i < cols; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            v->pos = *p;
            v->u = (float)(p->x / 640.0);
            v->v = (float)(p->y / 480.0);
            if (v->u < 0.0f)
            {
                v->u = 0.0f;
            }
            if (v->v < 0.0f)
            {
                v->v = 0.0f;
            }
            ++p;
            ++v;
        }
    }
}

void EndMesh::CopyToVms()
{
    if (cols == 0)
    {
        return;
    }
    EndMeshVert *v = verts;
    for (int i = 0; i < cols - 1; ++i)
    {
        EndMeshVert *dst = (EndMeshVert *)vms[i]->extraData;
        for (int j = 0; j < rows; ++j)
        {
            dst[0] = v[0];
            dst[1] = v[rows];
            dst += 2;
            ++v;
        }
    }
}

void EndMesh::HideAll()
{
    for (int i = 0; i < cols - 1; ++i)
    {
        vms[i]->flags4B0 &= ~2U;
    }
}

void EndMesh::ShowAll()
{
    for (int i = 0; i < cols - 1; ++i)
    {
        vms[i]->flags4B0 |= 2U;
    }
}

void EndMesh::SetBlendMode(int mode)
{
    for (int i = 0; i < cols - 1; ++i)
    {
        vms[i]->flags4B0 = (vms[i]->flags4B0 & ~0xE0U) | ((mode & 7) << 5);
    }
}

int EndGet0::Get() { return v; }
void EndSet0::Set(int x) { v = x; }

void EndVec2::Add(const EndVec2 &other)
{
    x += other.x;
    y += other.y;
}

void EndVec2::FromAngleRadius(float angle, float radius)
{
    x = cosf(angle) * radius;
    y = sinf(angle) * radius;
}

int EndGet04::Get() { return v; }

int EndPair::HasChanged()
{
    return cur != prev;
}

void EndVmMini::SetMode0D() { mode20 = 0xD; }
void EndVmMini::SetFlag2() { flags4B0 |= 2U; }
void EndVmMini::ClearFlag2() { flags4B0 &= ~2U; }

void EndPosToUv(const EndVec2 *pos, float *u, float *v)
{
    *u = (float)(pos->x / 640.0);
    *v = (float)(pos->y / 480.0);
    if (*u < 0.0f)
    {
        *u = 0.0f;
    }
    if (*v < 0.0f)
    {
        *v = 0.0f;
    }
}

void EndCopyFloat(float *dst, const float *src)
{
    *dst = *src;
}

void EndVmMini::SetPos464(const EndVec3 *pos)
{
    pos464 = *pos;
}

void EndListNode::Append(EndListNode *node)
{
    EndListNode *cur = this;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    if (cur->next != NULL)
    {
        node->next = cur->next;
        cur->next->prev = node;
    }
    cur->next = node;
    node->prev = cur;
}

void EndClear0::Clear() { v = 0; }
void EndNoOp412FB0() {}

EndSub::~EndSub()
{
    EndSubDtor46ACF0(&vtblD0);
    EndAnmMgrMini *mgr = g_EndAnmMgr;
    for (int i = 0; i < 5; ++i)
    {
        int id = anmIds[i];
        if (id != 0)
        {
            EndVmMini *found = NULL;
            for (EndAnmNode *n = mgr->listA; n != NULL; n = n->next)
            {
                if (n->vm->id == id)
                {
                    found = n->vm;
                    break;
                }
            }
            if (found == NULL)
            {
                for (EndAnmNode *n = mgr->listB; n != NULL; n = n->next)
                {
                    if (n->vm->id == id)
                    {
                        found = n->vm;
                        break;
                    }
                }
            }
            if (found != NULL)
            {
                found->flags4B0 |= 0x20000000U;
                if (found->field18 == 0)
                {
                    for (EndAnmNode *c = found->children; c != NULL; c = c->next)
                    {
                        c->vm->flags4B0 |= 0x20000000U;
                    }
                }
            }
        }
        anmIds[i] = 0;
    }
    vtblD0 = &g_EndSubVtbl;
    EndSubDtor46ACF0(&vtblD0);
}

int EndSub::GetFlag1() { return (flags74 >> 1) & 1U; }
int EndSub::GetFlag2() { return (flags74 >> 2) & 1U; }

void *EndMgr::LoadFile(const char *name)
{
    EndMgr *mgr = g_EndMgr;
    g_EndPathBuf[0] = '\0';
    strcat(g_EndPathBuf, name);
    void *buf = EndReadFile(g_EndPathBuf, 0, 0);
    if (mgr->fileBuf != NULL)
    {
        free(mgr->fileBuf);
        mgr->fileBuf = NULL;
    }
    mgr->fileBuf = buf;
    return buf;
}

int EndMgr::IsFlag0() { return g_EndMgr->flags20 & 1U; }
int EndMgr::IsFlag1() { return (flags20 >> 1) & 1U; }
int *EndOffset3368::Get() { return &field3368; }

EndMgr::EndMgr()
{
    memset(this, 0, sizeof(EndMgr));
    flags00 |= 2U;
    g_EndMgr = this;
}

EndMgr::~EndMgr()
{
    if (calcElem != NULL)
    {
        if ((g_EndSupFlags & 0x8000U) != 0)
        {
            g_EndEnterCS(g_EndCsObj);
            ++g_EndCsDepth;
        }
        g_EndChainMgr->Cut(calcElem);
        if ((g_EndSupFlags & 0x8000U) != 0)
        {
            g_EndLeaveCS(g_EndCsObj);
            --g_EndCsDepth;
        }
    }
    if (drawElem != NULL)
    {
        if ((g_EndSupFlags & 0x8000U) != 0)
        {
            g_EndEnterCS(g_EndCsObj);
            ++g_EndCsDepth;
        }
        g_EndChainMgr->Cut(drawElem);
        if ((g_EndSupFlags & 0x8000U) != 0)
        {
            g_EndLeaveCS(g_EndCsObj);
            --g_EndCsDepth;
        }
    }
    if (sub != NULL)
    {
        delete sub;
    }
    sub = NULL;
    for (int i = 0; i < 4; ++i)
    {
        EndAnmMgrMini *mgr = g_EndAnmMgr;
        if (mgr->endingAnms[i] != NULL)
        {
            EndAnmRelease465F60(mgr->endingAnms[i]);
            operator delete(mgr->endingAnms[i]);
            mgr->endingAnms[i] = NULL;
        }
    }
    if (fileBuf != NULL)
    {
        free(fileBuf);
        fileBuf = NULL;
    }
    fileBuf = NULL;
    g_EndMgr = NULL;
    g_End4D2AD4 = 0;
}

EndSub *EndSub::DestroySelf()
{
    this->~EndSub();
    operator delete(this);
    return this;
}

EndMgr *EndMgr::Create()
{
    EndMgr *mgr = new EndMgr();
    if (EndMgrInit(mgr) != 0)
    {
        if (mgr != NULL)
        {
            delete mgr;
        }
        return NULL;
    }
    return mgr;
}

EndMgr *EndMgr::DestroySelf()
{
    this->~EndMgr();
    operator delete(this);
    return this;
}

void EndMgr::DestroyGlobal()
{
    if (g_EndMgr != NULL)
    {
        delete g_EndMgr;
    }
}

int EndMgr::OnCalc()
{
    EndSub *s = sub;
    if (EndSubUpdate(s) == 0)
    {
        EndTimerTick(s->timer04);
        ++frameCount;
        unsigned int fl = sub->flags74;
        if ((fl & 4U) == 0 && (flags20 & 2U) == 0 && (fl & 2U) != 0 && (g_EndInputMask & 0x200U) != 0)
        {
            if (frameCount % 12 != 0)
            {
                return 6;
            }
        }
        return 1;
    }
    g_EndNextScene = ((g_EndSupFlags & 0x2000U) != 0) ? 2 : 16;
    return 1;
}
