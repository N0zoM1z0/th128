#include "EclVmHelpers.hpp"

#include <stddef.h>
#include <string.h>

static inline int EclStackPushRaw4(EclStack *s, int val)
{
    if (s->sp + 4 >= 0x1000)
    {
        return -1;
    }
    *(int *)&s->data[s->sp] = val;
    s->sp += 4;
    return 0;
}

static inline int EclStackPopRaw4(EclStack *s, int *outVal)
{
    if (s->sp - 4 < 0)
    {
        return -1;
    }
    s->sp -= 4;
    *outVal = *(int *)&s->data[s->sp];
    return 0;
}

void *EclStack::GetLocalPtr(int offset)
{
    return data + bp + offset;
}

void *EclStack::GetPtr(int offset)
{
    return data + offset;
}

int EclStack::GetSp()
{
    return sp;
}

void EclStack::SetSp(int newSp)
{
    sp = newSp;
}

int EclStack::Push(const void *val, char typeTag)
{
    if (sp + 4 >= 0x1000)
    {
        return -1;
    }
    if (typeTag != 0)
    {
        data[sp] = (unsigned char)typeTag;
        sp += 4;
    }
    *(int *)(sp + (char *)data) = *(const int *)val;
    sp += 4;
    return 0;
}

int EclStack::Pop(void *outVal, char expectedType)
{
    if (sp - 4 < 0)
    {
        return -1;
    }
    sp -= 4;
    *(int *)outVal = *(int *)&data[sp];
    if (expectedType != 0)
    {
        sp -= 4;
        switch (data[sp])
        {
        case 'f':
            if (expectedType == 'i')
            {
                *(int *)outVal = (int)(*(float *)outVal);
            }
            break;
        case 'i':
            if (expectedType == 'f')
            {
                *(float *)outVal = (float)(*(int *)outVal);
            }
            break;
        }
    }
    return 0;
}

int EclStack::AllocFrame(int size)
{
    int oldSp = sp;
    if (oldSp + size >= 0x1000)
    {
        return -1;
    }
    sp = oldSp + size;
    if (sp + 4 < 0x1000)
    {
        *(int *)&data[sp] = bp;
        sp += 4;
    }
    bp = oldSp;
    return 0;
}

int EclStack::FreeFrame()
{
    int oldBp = bp;
    if (sp - 4 >= 0)
    {
        sp -= 4;
        bp = *(int *)&data[sp];
    }
    sp = oldBp;
    return 0;
}

SptResourceInf *EclVm::GetResource()
{
    return resource;
}

void EclVm::SetResource(SptResourceInf *res)
{
    resource = res;
}

void EclVm::SetCurInstr(EclInstr *instr)
{
    activeContext->curInstr = instr;
}

EclInstr *EclVm::GetCurInstr()
{
    return activeContext->curInstr;
}

void EclVm::SetTime(float t)
{
    activeContext->time = t;
}

void EclVm::OnSubHook()
{
}

static inline void *EclStackLocalPtrInline(EclStack *s, int offset)
{
    int off = s->bp + offset;
    return (unsigned char *)s + off;
}

int EclContext::GetIntVar(int id)
{
    if (id >= 0)
    {
        return *(int *)EclStackLocalPtrInline(&stack, id);
    }
    if (id == -1)
    {
        if (stack.sp - 4 >= 0)
        {
            stack.sp -= 4;
            id = *(int *)&stack.data[stack.sp];
            stack.sp -= 4;
            if (stack.data[stack.sp] == 'f')
            {
                return (int)(*(float *)&id);
            }
        }
        return id;
    }
    return owner->GetIntVar(id);
}

float EclContext::GetFloatVar(float id)
{
    if (id >= 0.0f)
    {
        return *(float *)(stack.data + stack.bp + (int)id);
    }
    if (id == -1.0f)
    {
        float val;
        if (stack.sp - 4 < 0)
        {
            return id;
        }
        stack.sp -= 4;
        *(int *)&val = *(int *)&stack.data[stack.sp];
        stack.sp -= 4;
        unsigned char tag = stack.data[stack.sp];
        if (tag != 'f' && tag == 'i')
        {
            val = (float)(*(int *)&val);
        }
        return val;
    }
    return owner->GetFloatVar((int)id);
}

int EclContext::GetIntArg(int argIndex)
{
    EclInstr *instr = curInstr;
    if ((instr->paramMask & (1 << argIndex)) != 0)
    {
        int id = instr->iargs[argIndex];
        if (id >= 0)
        {
            return *(int *)((unsigned char *)&stack + stack.bp + id);
        }
        if (id == -1)
        {
            int val = id;
            if (stack.sp - 4 >= 0)
            {
                stack.sp -= 4;
                val = *(int *)&stack.data[stack.sp];
                stack.sp -= 4;
                if (stack.data[stack.sp] == 'f')
                {
                    return (int)(*(float *)&val);
                }
            }
            return val;
        }
        return owner->GetIntVar(id);
    }
    return instr->iargs[argIndex];
}

float EclContext::GetFloatArg(int argIndex)
{
    EclInstr *instr = curInstr;
    if ((instr->paramMask & (1 << argIndex)) != 0)
    {
        if (instr->fargs[argIndex] >= 0.0f)
        {
            return *(float *)(stack.data + stack.bp + (int)instr->fargs[argIndex]);
        }
        if (instr->fargs[argIndex] == -1.0f)
        {
            float val;
            if (stack.sp - 4 >= 0)
            {
                stack.sp -= 4;
                *(int *)&val = *(int *)&stack.data[stack.sp];
                stack.sp -= 4;
                unsigned char tag = stack.data[stack.sp];
                if (tag != 'f' && tag == 'i')
                {
                    val = (float)(*(int *)&val);
                }
            }
            return val;
        }
        return owner->GetFloatVar((int)instr->fargs[argIndex]);
    }
    return instr->fargs[argIndex];
}

int *EclContext::GetIntArg0Ptr()
{
    EclInstr *instr = curInstr;
    if ((instr->paramMask & 1) != 0)
    {
        int id = instr->iargs[0];
        if (id >= 0)
        {
            return (int *)EclStackLocalPtrInline(&stack, id);
        }
        return owner->GetIntVarPtr(id);
    }
    return NULL;
}

float *EclContext::GetFloatArgPtr(int argIndex)
{
    EclInstr *instr = curInstr;
    if ((instr->paramMask & (1 << argIndex)) != 0)
    {
        if (instr->fargs[argIndex] >= 0.0f)
        {
            return (float *)EclStackLocalPtrInline(&stack, (int)instr->fargs[argIndex]);
        }
        return owner->GetFloatVarPtr((int)instr->fargs[argIndex]);
    }
    return NULL;
}

float *EclContext::GetFloatArg1Ptr(EclInstr *instr)
{
    if ((instr->paramMask & 2) != 0)
    {
        if (instr->fargs[1] >= 0.0f)
        {
            return (float *)EclStackLocalPtrInline(&stack, (int)instr->fargs[1]);
        }
        return owner->GetFloatVarPtr((int)instr->fargs[1]);
    }
    return NULL;
}

int EclContext::CallSub(EclContext *target, int argStartIndex)
{
    unsigned int byteOff = curInstr->iargs[0] + argStartIndex * 4 + 4;
    int savedBp = 0;
    int savedSp = target->stack.sp;
    int dstOff = savedSp + 12;
    if (savedSp == 0)
    {
        EclStackPushRaw4(&target->stack, savedBp);
        dstOff = 16;
    }
    int *dstPtr = (int *)(target->stack.data + dstOff);
    for (int i = argStartIndex + 1; i < curInstr->paramCount; ++i)
    {
        unsigned char srcType = curInstr->bargs[byteOff];
        if (srcType == 'f' || srcType == 'g')
        {
            float val = curInstr->fargs[(byteOff + 4) >> 2];
            if ((curInstr->paramMask & (1 << i)) != 0)
            {
                val = GetFloatVar(val);
            }
            if (curInstr->bargs[byteOff + 1] == 'f')
            {
                *(float *)dstPtr = val;
            }
            else
            {
                *dstPtr = (int)val;
            }
        }
        else
        {
            int val = curInstr->iargs[(byteOff + 4) >> 2];
            if ((curInstr->paramMask & (1 << i)) != 0)
            {
                if (val >= 0)
                {
                    val = *(int *)((unsigned char *)&stack + stack.bp + val);
                }
                else if (val == -1)
                {
                    if (stack.sp - 4 >= 0)
                    {
                        stack.sp -= 4;
                        val = *(int *)&stack.data[stack.sp];
                        stack.sp -= 4;
                        if (stack.data[stack.sp] == 'f')
                        {
                            val = (int)(*(float *)&val);
                        }
                    }
                }
                else
                {
                    val = owner->GetIntVar(val);
                }
            }
            if (curInstr->bargs[byteOff + 1] == 'f')
            {
                *(float *)dstPtr = (float)val;
            }
            else
            {
                *dstPtr = val;
            }
        }
        ++dstPtr;
        byteOff += 8;
    }

    int newSp = target->stack.sp;
    if (savedSp != 0)
    {
        EclStackPopRaw4(&target->stack, &savedBp);
        target->stack.sp = savedSp;
        *(int *)(target->stack.data + savedSp - 4) = savedBp;
    }
    else
    {
        target->stack.sp = 4;
    }

    EclStackPushRaw4(&target->stack, newSp);
    if (savedSp != 0)
    {
        EclStackPushRaw4(&target->stack, *(int *)&time);
        EclStackPushRaw4(&target->stack, (int)curInstr);
    }
    else
    {
        EclStackPushRaw4(&target->stack, 0);
        EclStackPushRaw4(&target->stack, 0);
    }

    EclContext *prevActive = owner->activeContext;
    owner->activeContext = target;
    owner->JumpToSub((const char *)&curInstr->iargs[1]);
    if (owner->activeContext->curInstr == NULL)
    {
        EclSubNotFoundLog((const char *)&curInstr->iargs[1]);
        curInstr = NULL;
        return -1;
    }
    owner->activeContext = prevActive;
    return 0;
}

int EclVm::RunAllContexts(float deltaTime)
{
    EclContextNode *node = &listHead;
    int isMain = 1;
    while (node != NULL)
    {
        EclContextNode *next = node->next;
        activeContext = node->ctx;
        int res = activeContext->Run(deltaTime);
        if (isMain != 0)
        {
            if (res != 0)
            {
                return -1;
            }
            isMain = 0;
        }
        else if (res != 0)
        {
            delete activeContext;
            if (node->next != NULL)
            {
                node->next->prev = node->prev;
            }
            if (node->prev != NULL)
            {
                node->prev->next = node->next;
            }
            node->next = NULL;
            node->prev = NULL;
            delete node;
        }
        node = next;
    }
    activeContext = &mainContext;
    return 0;
}

int EclVm::SpawnAsyncContext(int asyncId, int argStartIndex)
{
    EclContext *ctx = new EclContext();
    EclContextNode *node = new EclContextNode();
    ctx->time = 0.0f;
    ctx->asyncId = asyncId;
    ctx->owner = this;
    ctx->curInstr = NULL;
    ctx->difficultyMask = activeContext->difficultyMask;

    node->ctx = ctx;
    node->next = NULL;
    node->prev = NULL;
    if (listHead.next != NULL)
    {
        node->next = listHead.next;
        listHead.next->prev = node;
    }
    listHead.next = node;
    node->prev = &listHead;

    return activeContext->CallSub(ctx, argStartIndex);
}

EclContextNode *EclVm::FindAsyncContext(int asyncId)
{
    for (EclContextNode *node = &listHead; node != NULL; node = node->next)
    {
        if (node->ctx->asyncId == asyncId)
        {
            return node;
        }
    }
    return NULL;
}

void EclVm::StopAsyncContexts()
{
    EclContextNode *node = listHead.next;
    while (node != NULL)
    {
        EclContextNode *next = node->next;
        node->ctx->curInstr = NULL;
        node = next;
    }
}

float __stdcall NormalizeAngleDiff(float a, float b)
{
    if (a - b > 3.1415927410125732)
    {
        return (float)(a - (b + 6.2831854820251465));
    }
    if (b - a > 3.1415927410125732)
    {
        return (float)(a - (b - 6.2831854820251465));
    }
    return a - b;
}

EclInstr *SptResourceInf::FindSub(const char *name)
{
    int low = 0;
    int high = subCount - 1;
    while (low <= high)
    {
        int mid = (high - low) / 2 + low;
        int cmp = strcmp(name, subs[mid].name);
        if (cmp == 0)
        {
            return (EclInstr *)((char *)subs[mid].code + 0x10);
        }
        if (cmp < 0)
        {
            high = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }
    return NULL;
}

EclVm::EclVm()
{
    for (int i = 0; i < 8; ++i)
    {
        mainContext.slots[i].ptr = NULL;
    }
}

EclVm *SptResourceInf::CreateVm(const char *subName)
{
    EclVm *vm = new EclVm();
    vm->resource = this;
    vm->activeContext->curInstr = FindSub(subName);
    vm->activeContext->time = 0.0f;
    return vm;
}

int EclVm::BindSub(SptResourceInf *res, const char *subName)
{
    resource = res;
    activeContext->curInstr = res->FindSub(subName);
    activeContext->time = 0.0f;
    return 0;
}

int EclVm::ResetToSub(SptResourceInf *res, const char *subName)
{
    activeContext->curInstr = res->FindSub(subName);
    activeContext->time = 0.0f;
    return activeContext->curInstr == NULL ? 1 : 0;
}

int EclVm::JumpToSub(const char *subName)
{
    activeContext->curInstr = resource->FindSub(subName);
    activeContext->time = 0.0f;
    return 0;
}
