#include "ZeroCallMedium4.hpp"

#include <math.h>
#include <string.h>

namespace th128 {

void Zc4InitRouteScores(Zc4RouteScore *route) {
    memset(route, 0, sizeof(Zc4RouteScore));
    route->magic = 0x35243;
    route->size = sizeof(Zc4RouteScore);
    for (int d = 0; d < 5; ++d) {
        for (int i = 0; i < 10; ++i) {
            route->entries[d][i].score = (10 - i) * 10000;
            route->entries[d][i].stage = 1;
            strcpy(route->entries[d][i].name, "--------");
            route->entries[d][i].timestampLo = 0;
            route->entries[d][i].timestampHi = 0;
            route->entries[d][i].cont = 0;
        }
    }
}

int __stdcall Zc4LzssInsertNode(int node, int *outMatchPos) {
    if (node == 0) {
        return 0;
    }
    int cur = g_Zc4LzssRoot;
    int bestLen = 0;
    for (;;) {
        int diff = 0;
        int len = 0;
        for (; len < 18; ++len) {
            diff = (int)g_Zc4LzssBuf[(node + len) & 0x1fff] - (int)g_Zc4LzssBuf[(cur + len) & 0x1fff];
            if (diff != 0) {
                break;
            }
        }
        if (len >= bestLen) {
            bestLen = len;
            *outMatchPos = cur;
            if (len >= 18) {
                int parent = g_Zc4LzssTree[cur].parent;
                if (g_Zc4LzssTree[parent].left == cur) {
                    g_Zc4LzssTree[parent].left = node;
                } else {
                    g_Zc4LzssTree[parent].right = node;
                }
                g_Zc4LzssTree[node].parent = g_Zc4LzssTree[cur].parent;
                g_Zc4LzssTree[node].left = g_Zc4LzssTree[cur].left;
                g_Zc4LzssTree[node].right = g_Zc4LzssTree[cur].right;
                g_Zc4LzssTree[g_Zc4LzssTree[node].left].parent = node;
                g_Zc4LzssTree[g_Zc4LzssTree[node].right].parent = node;
                g_Zc4LzssTree[cur].parent = 0;
                return len;
            }
        }
        int *branch = (diff >= 0) ? &g_Zc4LzssTree[cur].right : &g_Zc4LzssTree[cur].left;
        if (*branch == 0) {
            *branch = node;
            g_Zc4LzssTree[node].parent = cur;
            g_Zc4LzssTree[node].right = 0;
            g_Zc4LzssTree[node].left = 0;
            return bestLen;
        }
        cur = *branch;
    }
}

int __stdcall Zc4PlayerCheckLaserHit(const ZunVec3 *laserPos, float angle, float width, float length) {
    Zc4Player *p = g_Zc4Player;
    ZunVec3 rel;
    rel.x = p->posX - laserPos->x;
    rel.y = p->posY - laserPos->y;
    float negAngle = -angle;
    float s = (float)sin(negAngle);
    float c = (float)cos(negAngle);
    float ry = rel.y * c + rel.x * s;
    float rx = rel.x * c - rel.y * s;
    rel.x = rx;
    rel.y = ry;

    ZunVec3 graze;
    graze.x = (float)(p->hitHalfW * 16.0);
    graze.y = (float)(p->hitHalfH * 16.0);
    ZunVec3 gMin;
    gMin.x = rel.x - graze.x;
    gMin.y = rel.y - graze.y;
    ZunVec3 gMax;
    gMax.x = rel.x + graze.x;
    gMax.y = rel.y + graze.y;

    if (gMin.x > length || gMin.y > width * 0.5 || gMax.x < 0.0f || gMax.y < -width * 0.5) {
        return 0;
    }

    ZunVec3 hMin;
    hMin.x = rel.x - p->hitHalfW;
    hMin.y = rel.y - p->hitHalfH;
    ZunVec3 hMax;
    hMax.x = rel.x + p->hitHalfW;
    hMax.y = rel.y + p->hitHalfH;

    if (hMin.x > length || hMin.y > width * 0.5 || hMax.x < 0.0f || hMax.y < -width * 0.5) {
        return 2;
    }
    return 1;
}

inline void Zc4AnmVmRaw::ResetForReuse() {
    ZunVec3 savedPos = pos;
    unsigned int savedId = vmId;
    memset(this, 0, sizeof(Zc4AnmVmRaw));
    scaleX = 1.0f;
    scaleY = 1.0f;
    scale2X = 1.0f;
    scale2Y = 1.0f;
    uvScaleX = 1.0f;
    uvScaleY = 1.0f;
    pos = savedPos;
    vmId = savedId;
    color = 0xffffffffu;
    mat[1] = mat[2] = mat[3] =
    mat[4] = mat[6] = mat[7] =
    mat[8] = mat[9] = mat[11] =
    mat[12] = mat[13] = mat[14] = 0.0f;
    mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
    flagsLo = 7;
    timerCur = 0;
    timerCurF = 0.0f;
    timerPrev = -999999;
    interp0 = 0;
    interp1 = 0;
    interp2 = 0;
    interp3 = 0;
    interp4 = 0;
    interp5 = 0;
    interp6 = 0;
    interp7 = 0;
    interp8 = 0;
    interp9 = 0;
    stateFlags = (stateFlags & ~0x80u) | 0x40u;
    node1Self = this;
    node1Next = 0;
    node1Prev = 0;
    node2Self = this;
    node2Next = 0;
    node2Prev = 0;
    flags = (flags & ~0xc0u) | 0x20u;
}

int Zc4LaserLine::Init(const Zc4LaserDesc *d) {
    desc = *d;
    width = *(float *)&desc.words[8 - 3];
    unk68 = *(float *)&desc.words[6 - 3];
    unk0c = 3;
    unk10 = 3;
    pos = desc.pos;
    unk5bc = (int)desc.words[11 - 3];
    unk80 = (int)desc.words[10 - 3];
    for (int i = 0; i < 512; ++i) {
        widths[i] = width;
    }
    unk70 = 1.0f;
    unkDfc = 0.0f;
    vm.ResetForReuse();
    return 0;
}

int Zc4Enemy::CheckSpellInterrupts() {
    hpRemaining = hp;
    hpFloor = 0;
    for (unsigned int i = 0; i < 8; ++i) {
        if (interrupts[i].hpThreshold >= 0) {
            hpRemaining = hp - interrupts[i].hpThreshold;
            hpFloor = interrupts[i].hpThreshold;
            if (hp <= interrupts[i].hpThreshold) {
                hp = interrupts[i].hpThreshold;
                interrupts[i].hpThreshold = -1;
                timer.Initialize();
                timer.current = 0;
                timer.currentF = 0.0f;
                timer.previous = -1;
                flags &= ~0x02000000u;
                return interrupts[i].subIndex;
            }
            break;
        }
    }
    for (unsigned int i = 0; i < 8; ++i) {
        if (interrupts[i].hpThreshold >= 0 && interrupts[i].timeLimit > 0) {
            int rem = interrupts[i].timeLimit - timer.current;
            int sec = rem / 60;
            int cs = (rem % 60) * 100 / 60;
            g_Zc4Gui->timerSec = (sec > 99) ? 99 : sec;
            g_Zc4Gui->timerCs = (sec > 99) ? 99 : cs;
            if (timer.current >= interrupts[i].timeLimit) {
                hp = interrupts[i].hpThreshold;
                interrupts[i].hpThreshold = -1;
                timer.Initialize();
                timer.current = 0;
                timer.currentF = 0.0f;
                timer.previous = -1;
                flags |= 0x02000000u;
                Zc4CardInf *card = g_Zc4CardInf;
                if ((card->flags & 0x10) == 0) {
                    if ((card->flags & 1) != 0) {
                        if (card->timerCur >= 60) {
                            card->bonus = 0;
                            card->flags &= ~0x46u;
                        } else if (g_Zc4Gate->active != 0) {
                            card->flags |= 0x40u;
                        }
                    }
                    g_Zc4EnemyMgr->field18 = 0;
                }
                return interrupts[i].subIndex;
            }
            break;
        }
    }
    return 0;
}

} // namespace th128
