#include "EclVmRun.hpp"

#include <math.h>
#include <string.h>

namespace th128 {

static inline int EclRunPushRaw4(EclRunStack *s, int val) {
    if (s->sp + 4 >= 0x1000) {
        return -1;
    }
    *(int *)&s->data[s->sp] = val;
    s->sp += 4;
    return 0;
}

static inline int EclRunPopRaw4(EclRunStack *s, int *outVal) {
    if (s->sp - 4 < 0) {
        return -1;
    }
    s->sp -= 4;
    *outVal = *(int *)&s->data[s->sp];
    return 0;
}

static inline void *EclRunLocalPtr(EclRunStack *s, int offset) {
    int off = s->bp + offset;
    return (unsigned char *)s + off;
}

int EclRunStack::Push(const void *val, char typeTag) {
    if (sp + 4 >= 0x1000) {
        return -1;
    }
    if (typeTag != 0) {
        data[sp] = (unsigned char)typeTag;
        sp += 4;
    }
    *(int *)&data[sp] = *(const int *)val;
    sp += 4;
    return 0;
}

int EclRunStack::Pop(void *outVal, char expectedType) {
    if (sp - 4 < 0) {
        return -1;
    }
    sp -= 4;
    *(int *)outVal = *(int *)&data[sp];
    if (expectedType != 0) {
        sp -= 4;
        switch (data[sp]) {
        case 'f':
            if (expectedType == 'i') {
                *(int *)outVal = (int)(*(float *)outVal);
            }
            break;
        case 'i':
            if (expectedType == 'f') {
                *(float *)outVal = (float)(*(int *)outVal);
            }
            break;
        }
    }
    return 0;
}

int EclRunStack::AllocFrame(int size) {
    int oldSp = sp;
    if (oldSp + size >= 0x1000) {
        return -1;
    }
    sp = oldSp + size;
    if (sp + 4 < 0x1000) {
        *(int *)&data[sp] = bp;
        sp += 4;
    }
    bp = oldSp;
    return 0;
}

int EclRunStack::FreeFrame() {
    int oldBp = bp;
    if (sp - 4 >= 0) {
        sp -= 4;
        bp = *(int *)&data[sp];
    }
    sp = oldBp;
    return 0;
}

void EclRunRotateVec2(EclRunVec3 *out, const EclRunVec3 *in, float angle) {
    float s = (float)sin(angle);
    float c = (float)cos(angle);
    float nx = in->x * c - in->y * s;
    float ny = in->x * s + in->y * c;
    out->x = nx;
    out->y = ny;
}

int EclRunContext::GetIntVar(int id) {
    if (id >= 0) {
        return *(int *)EclRunLocalPtr(&stack, id);
    }
    if (id == -1) {
        if (stack.sp - 4 >= 0) {
            stack.sp -= 4;
            id = *(int *)&stack.data[stack.sp];
            stack.sp -= 4;
            if (stack.data[stack.sp] == 'f') {
                return (int)(*(float *)&id);
            }
        }
        return id;
    }
    return owner->GetIntVar(id);
}

float EclRunContext::GetFloatVar(float id) {
    if (id >= 0.0f) {
        return *(float *)(stack.data + stack.bp + (int)id);
    }
    if (id == -1.0f) {
        float val;
        if (stack.sp - 4 < 0) {
            return id;
        }
        stack.sp -= 4;
        *(int *)&val = *(int *)&stack.data[stack.sp];
        stack.sp -= 4;
        unsigned char tag = stack.data[stack.sp];
        if (tag != 'f' && tag == 'i') {
            val = (float)(*(int *)&val);
        }
        return val;
    }
    return owner->GetFloatVar((int)id);
}

int EclRunContext::GetIntArg(int argIndex) {
    EclRunInstr *instr = curInstr;
    if ((instr->paramMask & (1 << argIndex)) != 0) {
        int id = instr->iargs[argIndex];
        if (id >= 0) {
            return *(int *)EclRunLocalPtr(&stack, id);
        }
        if (id == -1) {
            stack.Pop(&argIndex, 'i');
            return argIndex;
        }
        return owner->GetIntVar(id);
    }
    return instr->iargs[argIndex];
}

float EclRunContext::GetFloatArg(int argIndex) {
    EclRunInstr *instr = curInstr;
    if ((instr->paramMask & (1 << argIndex)) != 0) {
        if (instr->fargs[argIndex] >= 0.0f) {
            return *(float *)(stack.data + stack.bp + (int)instr->fargs[argIndex]);
        }
        if (instr->fargs[argIndex] == -1.0f) {
            float val;
            if (stack.sp - 4 >= 0) {
                stack.sp -= 4;
                *(int *)&val = *(int *)&stack.data[stack.sp];
                stack.sp -= 4;
                unsigned char tag = stack.data[stack.sp];
                if (tag != 'f' && tag == 'i') {
                    val = (float)(*(int *)&val);
                }
            }
            return val;
        }
        return owner->GetFloatVar((int)instr->fargs[argIndex]);
    }
    return instr->fargs[argIndex];
}

int *EclRunContext::GetIntArg0Ptr() {
    EclRunInstr *instr = curInstr;
    if ((instr->paramMask & 1) != 0) {
        int id = instr->iargs[0];
        if (id >= 0) {
            return (int *)((unsigned char *)stack.data + (stack.bp + id));
        }
        return owner->GetIntVarPtr(id);
    }
    return NULL;
}

float *EclRunContext::GetFloatArgPtr(int argIndex) {
    EclRunInstr *instr = curInstr;
    if ((instr->paramMask & (1 << argIndex)) != 0) {
        if (instr->fargs[argIndex] >= 0.0f) {
            return (float *)EclRunLocalPtr(&stack, (int)instr->fargs[argIndex]);
        }
        return owner->GetFloatVarPtr((int)instr->fargs[argIndex]);
    }
    return NULL;
}

float *EclRunContext::GetFloatArg1Ptr(EclRunInstr *instr) {
    if ((instr->paramMask & 2) != 0) {
        if (instr->fargs[1] >= 0.0f) {
            return (float *)EclRunLocalPtr(&stack, (int)instr->fargs[1]);
        }
        return owner->GetFloatVarPtr((int)instr->fargs[1]);
    }
    return NULL;
}

int EclRunContext::CallSub(EclRunContext *target, int argStartIndex) {
    unsigned int byteOff = curInstr->iargs[0] + argStartIndex * 4 + 4;
    int savedBp = 0;
    int savedSp = target->stack.sp;
    int dstOff = savedSp + 12;
    if (savedSp == 0) {
        EclRunPushRaw4(&target->stack, savedBp);
        dstOff = 16;
    }
    int *dstPtr = (int *)(target->stack.data + dstOff);
    for (int i = argStartIndex + 1; i < curInstr->paramCount; ++i) {
        unsigned char srcType = curInstr->bargs[byteOff];
        if (srcType == 'f' || srcType == 'g') {
            float val = curInstr->fargs[(byteOff + 4) >> 2];
            if ((curInstr->paramMask & (1 << i)) != 0) {
                val = GetFloatVar(val);
            }
            if (curInstr->bargs[byteOff + 1] == 'f') {
                *(float *)dstPtr = val;
            } else {
                *dstPtr = (int)val;
            }
        } else {
            int val = curInstr->iargs[(byteOff + 4) >> 2];
            if ((curInstr->paramMask & (1 << i)) != 0) {
                if (val >= 0) {
                    val = *(int *)((unsigned char *)&stack + stack.bp + val);
                } else if (val == -1) {
                    if (stack.sp - 4 >= 0) {
                        stack.sp -= 4;
                        val = *(int *)&stack.data[stack.sp];
                        stack.sp -= 4;
                        if (stack.data[stack.sp] == 'f') {
                            val = (int)(*(float *)&val);
                        }
                    }
                } else {
                    val = owner->GetIntVar(val);
                }
            }
            if (curInstr->bargs[byteOff + 1] == 'f') {
                *(float *)dstPtr = (float)val;
            } else {
                *dstPtr = val;
            }
        }
        ++dstPtr;
        byteOff += 8;
    }

    int newSp = target->stack.sp;
    if (savedSp != 0) {
        EclRunPopRaw4(&target->stack, &savedBp);
        target->stack.sp = savedSp;
        *(int *)(target->stack.data + savedSp - 4) = savedBp;
    } else {
        target->stack.sp = 4;
    }

    EclRunPushRaw4(&target->stack, newSp);
    if (savedSp != 0) {
        EclRunPushRaw4(&target->stack, *(int *)&time);
        EclRunPushRaw4(&target->stack, (int)curInstr);
    } else {
        EclRunPushRaw4(&target->stack, 0);
        EclRunPushRaw4(&target->stack, 0);
    }

    EclRunContext *prevActive = owner->activeContext;
    owner->activeContext = target;
    owner->JumpToSub((const char *)&curInstr->iargs[1]);
    if (owner->activeContext->curInstr == NULL) {
        EclRunSubNotFoundLog((const char *)&curInstr->iargs[1]);
        curInstr = NULL;
        return -1;
    }
    owner->activeContext = prevActive;
    return 0;
}

int EclRunContext::Run(float deltaTime) {
    if (curInstr == NULL) {
        return -1;
    }
    while ((float)curInstr->time <= time) {
        if ((difficultyMask & curInstr->difficultyMask) != 0) {
            switch (curInstr->opcode) {
            case 10: {
                stack.FreeFrame();
                if (stack.sp == 0) {
                    curInstr = NULL;
                    return -1;
                }
                int newSp;
                stack.Pop(&curInstr, 0);
                stack.Pop(&time, 0);
                stack.Pop(&newSp, 0);
                stack.sp = newSp;
                if (curInstr == NULL) {
                    return -1;
                }
                break;
            }
            case 15:
                owner->SpawnAsyncContext(-1, 0);
                break;
            case 21:
                owner->StopAsyncContexts();
                break;
            case 16: {
                int id = curInstr->iargs[(unsigned int)(curInstr->iargs[0] + 4) >> 2];
                if ((curInstr->paramMask & 2) != 0) {
                    owner->SpawnAsyncContext(GetIntVar(id), 1);
                } else {
                    owner->SpawnAsyncContext(id, 1);
                }
                break;
            }
            case 17: {
                EclRunNode *n = owner->FindAsyncContext(GetIntArg(0));
                if (n != NULL) {
                    n->ctx->curInstr = NULL;
                }
                break;
            }
            case 18: {
                EclRunNode *n = owner->FindAsyncContext(GetIntArg(0));
                if (n != NULL) {
                    n->ctx->flags |= 1;
                }
                break;
            }
            case 19: {
                EclRunNode *n = owner->FindAsyncContext(GetIntArg(0));
                if (n != NULL) {
                    n->ctx->flags &= ~1;
                }
                break;
            }
            case 20: {
                EclRunNode *n = owner->FindAsyncContext(GetIntArg(0));
                if (n != NULL) {
                    n->ctx->unk1018 = GetIntArg(1);
                }
                break;
            }
            case 11:
                if (CallSub(this, 0) != 0) {
                    return -1;
                }
                continue;
            case 14: {
                int v;
                stack.Pop(&v, 'i');
                if (v != 0) {
                    goto do_jmp;
                }
                break;
            }
            case 13: {
                int v;
                stack.Pop(&v, 'i');
                if (v != 0) {
                    break;
                }
            }
            case 12:
            do_jmp:
                time = (float)curInstr->iargs[1];
                curInstr = (EclRunInstr *)((char *)curInstr + curInstr->iargs[0]);
                continue;
            case 83:
                time = (float)((double)time - (double)GetIntArg(0));
                break;
            case 40:
                stack.AllocFrame(GetIntArg(0));
                break;
            case 41:
                stack.FreeFrame();
                break;
            case 42: {
                int v = GetIntArg(0);
                stack.Push(&v, 'i');
                break;
            }
            case 43: {
                int *p = GetIntArg0Ptr();
                stack.Pop(p, 'i');
                break;
            }
            case 44: {
                float v = GetFloatArg(0);
                stack.Push(&v, 'f');
                break;
            }
            case 45: {
                float *p = GetFloatArgPtr(0);
                stack.Pop(p, 'f');
                break;
            }
            case 50: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                a += b;
                stack.Push(&a, 'i');
                break;
            }
            case 52: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                a -= b;
                stack.Push(&a, 'i');
                break;
            }
            case 54: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                a *= b;
                stack.Push(&a, 'i');
                break;
            }
            case 56: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                a /= b;
                stack.Push(&a, 'i');
                break;
            }
            case 58: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                a %= b;
                stack.Push(&a, 'i');
                break;
            }
            case 51: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                a += b;
                stack.Push(&a, 'f');
                break;
            }
            case 53: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                a -= b;
                stack.Push(&a, 'f');
                break;
            }
            case 55: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                a *= b;
                stack.Push(&a, 'f');
                break;
            }
            case 57: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                a /= b;
                stack.Push(&a, 'f');
                break;
            }
            case 59: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = (a == b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 61: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = (a != b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 63: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = (a < b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 65: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = (a <= b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 67: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = (a > b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 69: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = (a >= b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 71: {
                int a;
                stack.Pop(&a, 'i');
                int r = (a == 0) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 60: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                int r = (a == b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 62: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                int r = (a != b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 64: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                int r = (a < b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 66: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                int r = (a <= b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 68: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                int r = (a > b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 70: {
                float b, a;
                stack.Pop(&b, 'f');
                stack.Pop(&a, 'f');
                int r = (a >= b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 72: {
                float a;
                stack.Pop(&a, 'f');
                int r = (a == 0.0f) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 73: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = (a || b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 74: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = (a && b) ? 1 : 0;
                stack.Push(&r, 'i');
                break;
            }
            case 75: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = a ^ b;
                stack.Push(&r, 'i');
                break;
            }
            case 76: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = a | b;
                stack.Push(&r, 'i');
                break;
            }
            case 77: {
                int b, a;
                stack.Pop(&b, 'i');
                stack.Pop(&a, 'i');
                int r = a & b;
                stack.Push(&r, 'i');
                break;
            }
            case 84: {
                int a;
                stack.Pop(&a, 'i');
                a = -a;
                stack.Push(&a, 'i');
                break;
            }
            case 85: {
                float a;
                stack.Pop(&a, 'f');
                a = -a;
                stack.Push(&a, 'f');
                break;
            }
            case 78: {
                int v = GetIntArg(0);
                *GetIntArg0Ptr() = v - 1;
                stack.Push(&v, 'i');
                break;
            }
            case 79: {
                float a;
                stack.Pop(&a, 'f');
                float r = (float)sin(a);
                stack.Push(&r, 'f');
                break;
            }
            case 88: {
                float a;
                stack.Pop(&a, 'f');
                float r = (float)sqrt(a);
                stack.Push(&r, 'f');
                break;
            }
            case 80: {
                float a;
                stack.Pop(&a, 'f');
                float r = (float)cos(a);
                stack.Push(&r, 'f');
                break;
            }
            case 81: {
                float ang = EclRunNormalizeAngle(GetFloatArg(2));
                float rad = GetFloatArg(3);
                EclRunVec3 v;
                v.FromAngle(ang, rad);
                *GetFloatArgPtr(0) = v.x;
                *GetFloatArgPtr(1) = v.y;
                break;
            }
            case 86: {
                float x = GetFloatArg(1);
                float y = GetFloatArg(2);
                double dx = x;
                double dy = y;
                *GetFloatArgPtr(0) = (float)(dx * dx + dy * dy);
                break;
            }
            case 82: {
                float a = EclRunNormalizeAngle(GetFloatArg(0));
                *GetFloatArgPtr(0) = a;
                break;
            }
            case 87: {
                float x1 = GetFloatArg(1);
                float y1 = GetFloatArg(2);
                float x2 = GetFloatArg(3);
                float y2 = GetFloatArg(4);
                float ang = (float)atan2(y2 - y1, x2 - x1);
                *GetFloatArgPtr(0) = ang;
                break;
            }
            case 89: {
                float a = GetFloatArg(1);
                float b = GetFloatArg(2);
                *GetFloatArgPtr(0) = EclRunNormalizeAngleDiff(b, a);
                break;
            }
            case 90: {
                EclRunVec3 in;
                in.x = GetFloatArg(2);
                in.y = GetFloatArg(3);
                float ang = EclRunNormalizeAngle(GetFloatArg(4));
                EclRunVec3 out;
                EclRunRotateVec2(&out, &in, ang);
                *GetFloatArgPtr(0) = out.x;
                *GetFloatArgPtr(1) = out.y;
                break;
            }
            case 91: {
                int slot = GetIntArg(0);
                interpInstrs[slot] = curInstr;
                EclRunInterpScalar(&interps[slot]);
                interps[slot].duration = GetIntArg(2);
                interps[slot].mode = GetIntArg(3);
                float s = GetFloatArg(4);
                float g = GetFloatArg(5);
                interps[slot].start = s;
                interps[slot].goal = g;
                interps[slot].vel1 = 0.0f;
                interps[slot].vel2 = 0.0f;
                EclRunResetInterpTimer(&interps[slot]);
                break;
            }
            case 92: {
                int slot = GetIntArg(0);
                interpInstrs[slot] = curInstr;
                EclRunInterpScalar(&interps[slot]);
                interps[slot].duration = GetIntArg(2);
                interps[slot].mode = GetIntArg(3);
                float s = GetFloatArg(4);
                float g = GetFloatArg(5);
                interps[slot].start = s;
                interps[slot].goal = g;
                float v1 = GetFloatArg(6);
                float v2 = GetFloatArg(7);
                interps[slot].vel1 = v1;
                interps[slot].vel2 = v2;
                EclRunResetInterpTimer(&interps[slot]);
                break;
            }
            case 0:
            case 22:
            case 30:
                break;
            case 1:
                curInstr = NULL;
                return -1;
            default:
                if (owner->OnSubHook() == -1) {
                    goto update_interps;
                }
                break;
            }
        }
        curInstr = (EclRunInstr *)((char *)curInstr + curInstr->size);
    }
    time += deltaTime;

update_interps:
    for (int i = 0; i < 8; ++i) {
        if (interps[i].duration != 0) {
            float *dst = GetFloatArg1Ptr(interpInstrs[i]);
            *dst = EclRunInterpScalar(&interps[i]);
        }
    }
    return 0;
}

int EclRunVm::RunAllContexts(float deltaTime) {
    EclRunNode *node = &listHead;
    int isMain = 1;
    while (node != NULL) {
        EclRunNode *next = node->next;
        activeContext = node->ctx;
        int res = activeContext->Run(deltaTime);
        if (isMain != 0) {
            if (res != 0) {
                return -1;
            }
            isMain = 0;
        } else if (res != 0) {
            delete activeContext;
            if (node->next != NULL) {
                node->next->prev = node->prev;
            }
            if (node->prev != NULL) {
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

int EclRunVm::SpawnAsyncContext(int asyncId, int argStartIndex) {
    EclRunContext *ctx = new EclRunContext();
    EclRunNode *node = new EclRunNode();
    ctx->time = 0.0f;
    ctx->asyncId = asyncId;
    ctx->owner = this;
    ctx->curInstr = NULL;
    ctx->difficultyMask = activeContext->difficultyMask;

    node->ctx = ctx;
    node->next = NULL;
    node->prev = NULL;
    if (listHead.next != NULL) {
        node->next = listHead.next;
        listHead.next->prev = node;
    }
    listHead.next = node;
    node->prev = &listHead;

    return activeContext->CallSub(ctx, argStartIndex);
}

EclRunNode *EclRunVm::FindAsyncContext(int asyncId) {
    for (EclRunNode *node = &listHead; node != NULL; node = node->next) {
        if (node->ctx->asyncId == asyncId) {
            return node;
        }
    }
    return NULL;
}

void EclRunVm::StopAsyncContexts() {
    EclRunNode *node = listHead.next;
    while (node != NULL) {
        EclRunNode *next = node->next;
        node->ctx->curInstr = NULL;
        node = next;
    }
}

EclRunInstr *EclRunResource::FindSub(const char *name) {
    int low = 0;
    int high = subCount - 1;
    while (low <= high) {
        int mid = (high - low) / 2 + low;
        int cmp = strcmp(name, subs[mid].name);
        if (cmp == 0) {
            return (EclRunInstr *)((char *)subs[mid].code + 0x10);
        }
        if (cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return NULL;
}

EclRunVm::EclRunVm() {
    for (int i = 0; i < 8; ++i) {
        mainContext.interps[i].duration = 0;
    }
}

EclRunVm *EclRunResource::CreateVm(const char *subName) {
    EclRunVm *vm = new EclRunVm();
    vm->resource = this;
    vm->activeContext->curInstr = FindSub(subName);
    vm->activeContext->time = 0.0f;
    return vm;
}

int EclRunVm::JumpToSub(const char *subName) {
    activeContext->curInstr = resource->FindSub(subName);
    activeContext->time = 0.0f;
    return 0;
}

} // namespace th128
