#include "CleanBatch1.hpp"

namespace th128 {

inline void Cb1Timer::Reset() {
    if ((flags & 1) == 0) {
        curF = 0.0f;
        cur = 0;
        prev = -999999;
        ctrl = &g_Cb1TimerCtrl;
        flags |= 1;
    }
    cur = 0;
    curF = 0.0f;
    prev = -1;
}

int Cb1Enemy::UpdatePhases() {
    int curHp = hp;
    hpDelta = curHp;
    hpFloor = 0;

    for (unsigned int i = 0; i < 8; ++i) {
        if (phases[i].hpThreshold >= 0) {
            hpDelta = curHp - phases[i].hpThreshold;
            hpFloor = phases[i].hpThreshold;
            if (curHp <= phases[i].hpThreshold) {
                hp = phases[i].hpThreshold;
                phases[i].hpThreshold = -1;
                timer.Reset();
                flags50ac &= ~0x02000000u;
                return phases[i].hpSub;
            }
            break;
        }
    }

    for (unsigned int i = 0; i < 8; ++i) {
        if (phases[i].hpThreshold >= 0 && phases[i].timeLimit > 0) {
            int rem = phases[i].timeLimit - timer.cur;
            int cs = ((rem % 60) * 100) / 60;
            int sec = rem / 60;
            g_Cb1Gui->timerSec = sec > 99 ? 99 : sec;
            g_Cb1Gui->timerCs = sec > 99 ? 99 : cs;
            if (timer.cur >= phases[i].timeLimit) {
                hp = phases[i].hpThreshold;
                phases[i].hpThreshold = -1;
                timer.Reset();
                flags50ac |= 0x02000000u;
                if ((g_Cb1Spell->flags7c & 0x10u) == 0) {
                    if ((g_Cb1Spell->flags7c & 1u) != 0) {
                        if (g_Cb1Spell->timer28 >= 60) {
                            g_Cb1Spell->bonus80 = 0;
                            g_Cb1Spell->flags7c &= ~0x46u;
                        } else if (g_Cb1Player->f3c != 0) {
                            g_Cb1Spell->flags7c |= 0x40u;
                        }
                    }
                    g_Cb1Boss->f18 = 0;
                }
                return phases[i].timeSub;
            }
            return 0;
        }
    }

    return 0;
}

int __stdcall Cb1SegmentsIntersect(Cb1Vec2 p1, Cb1Vec2 p2, Cb1Vec2 p3, Cb1Vec2 p4) {
    float t1 = (p1.x - p2.x) * (p3.y - p1.y) + (p1.y - p2.y) * (p1.x - p3.x);
    float t2 = (p1.x - p2.x) * (p4.y - p1.y) + (p1.y - p2.y) * (p1.x - p4.x);
    if (t1 * t2 > 0.0f) {
        return 0;
    }
    if (t1 == 0.0f && t2 == 0.0f) {
        float tmp;
        if (p1.x > p2.x) {
            tmp = p2.x; p2.x = p1.x; p1.x = tmp;
            tmp = p2.y; p2.y = p1.y; p1.y = tmp;
        }
        if (p3.x > p4.x) {
            tmp = p4.x; p4.x = p3.x; p3.x = tmp;
            tmp = p4.y; p4.y = p3.y; p3.y = tmp;
        }
        if (p4.x >= p1.x && p4.y >= p1.y && p2.x >= p3.x && p2.y >= p3.y) {
            return 1;
        }
        return 0;
    }
    float s1 = (p3.x - p4.x) * (p1.y - p3.y) + (p3.y - p4.y) * (p3.x - p1.x);
    float s2 = (p3.x - p4.x) * (p2.y - p3.y) + (p3.y - p4.y) * (p3.x - p2.x);
    if (s1 * s2 > 0.0f) {
        return 0;
    }
    return 1;
}

int Cb1AnmVm::UpdateRingMesh() {
    float angle = -3.1415927f;
    Cb1RingMesh* mesh = ringMesh;
    mesh->verts[0].pos = pos1 + pos2;
    mesh->verts[0].u += mesh->uvStep;
    if (mesh->verts[0].u < 0.0f) {
        for (int i = 0; i < 33; ++i) {
            mesh->verts[i].u += 1.0f;
        }
    }
    mesh->verts[0].v += mesh->uvStep;
    if (mesh->verts[0].v < 0.0f) {
        for (int i = 0; i < 33; ++i) {
            mesh->verts[i].v += 1.0f;
        }
    }
    mesh->verts[0].color = color;

    for (int i = 0; i < 31; ++i) {
        mesh->verts[i + 1].u += mesh->uvStep;
        if (mesh->verts[i + 1].u < 0.0f) {
            for (int k = 0; k < 33; ++k) {
                mesh->verts[k].u += 1.0f;
            }
        }
        mesh->verts[i + 1].v += mesh->uvStep;
        if (mesh->verts[i + 1].v < 0.0f) {
            for (int k = 0; k < 33; ++k) {
                mesh->verts[k].v += 1.0f;
            }
        }
        mesh->verts[i + 1].color = color;
        ((unsigned char*)&mesh->verts[i + 1].color)[3] = 0;
        mesh->radii[i] += mesh->radiusVel[i];
        mesh->verts[i + 1].pos.SetPolarXY(angle, mesh->radii[i]);
        mesh->verts[i + 1].pos += pos1 + pos2;
        angle = (float)(angle + 0.20268340408802032);
    }

    mesh->verts[32] = mesh->verts[1];
    return 0;
}

void Cb1InputState::Update() {
    unsigned int bits = curr;
    unsigned int mask = 1;
    repeatMask = 0;
    heldMask = 0;
    unsigned int* rc = repeatCount;
    for (int i = 0; i < 32; ++i) {
        if (bits & 1) {
            rc[0]++;
            rc[64]++;
            if (rc[0] >= 8) {
                heldMask |= mask;
            }
            if (rc[0] >= 26) {
                repeatMask |= mask;
                rc[0] -= 8;
            }
        } else {
            rc[0] = 0;
            rc[64] = 0;
        }
        ++rc;
        bits >>= 1;
        mask <<= 1;
    }
    pressed = (prev ^ curr) & curr;
    released = (prev ^ curr) & ~curr;
}

} // namespace th128
