#include "LowCallWaveA.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

int LcwChain::RunCalcChain() {
    if ((g_LcwSupFlags & 0x8000u) != 0) {
        EnterCriticalSection(&g_LcwChainCs);
        ++g_LcwChainLockDepth;
    }

    int ret;
restart:
    LcwChainNode *cur = calcHead18;
    ret = 0;
    while (cur != NULL) {
        next48 = cur->next4;
        LcwChainElem *elem = cur->elem0;
        if (elem->callback8 != NULL) {
            if ((elem->flags4 & 2u) != 0) {
            run_cb:
                if (paused4c != 0) {
                    goto paused_cb;
                }
                if ((g_LcwSupFlags & 0x8000u) != 0) {
                    LeaveCriticalSection(&g_LcwChainCs);
                    --g_LcwChainLockDepth;
                }
                int r = elem->callback8(elem->arg20);
                if ((g_LcwSupFlags & 0x8000u) != 0) {
                    EnterCriticalSection(&g_LcwChainCs);
                    ++g_LcwChainLockDepth;
                }
                switch (r) {
                case 0:
                    LcwChainCutImpl(this, elem);
                    break;
                case 1:
                    break;
                case 2:
                    if ((elem->flags4 & 2u) != 0) {
                        goto run_cb;
                    }
                    break;
                case 4:
                case 8:
                    ret = 0;
                    goto done;
                case 3:
                    ret = 1;
                    goto done;
                case 5:
                    ret = -1;
                    goto done;
                case 6:
                    goto restart;
                case 7:
                paused_cb:
                    if (elem->pausedCb10 != NULL) {
                        elem->pausedCb10(elem->arg20);
                    }
                    break;
                default:
                    break;
                }
            }
            ++ret;
        }
        cur = next48;
    }

done:
    if ((g_LcwSupFlags & 0x8000u) != 0) {
        LeaveCriticalSection(&g_LcwChainCs);
        --g_LcwChainLockDepth;
    }
    return ret;
}

void LcwGui::UpdateScoreDisplay() {
    LcwGui *gui = g_LcwGui;
    if (g_LcwScore != gui->dispScore58fc) {
        int step = (g_LcwScore - gui->dispScore58fc) / 32;
        if (step >= 578910) {
            step = 578910;
        } else if (step == 0) {
            step = 1;
        }
        if (gui->scoreStep5900 < step) {
            gui->scoreStep5900 = step;
        }
        if (gui->scoreStep5900 > g_LcwScore - gui->dispScore58fc) {
            gui->scoreStep5900 = g_LcwScore - gui->dispScore58fc;
        }
        gui->dispScore58fc += gui->scoreStep5900;
        if (gui->dispScore58fc >= g_LcwScore) {
            gui->scoreStep5900 = 0;
        }
    }
    if (g_LcwHiScore < gui->dispScore58fc) {
        g_LcwHiScore = gui->dispScore58fc;
        g_LcwHiScoreContinue = g_LcwContinueCount;
        g_LcwStageFlagsBits.hiScore = 1;
        if (!g_LcwStageFlagsBits.hiScore) {
            gui->ShowPopup(3, 3, 0);
        }
    }
    if ((gui->flags5a04 & 0x10u) == 0) {
        int threshold = (g_LcwDifficulty == 4) ? g_LcwExtendExtra[g_LcwExtendIdx] : g_LcwExtendNormal[g_LcwExtendIdx];
        if (gui->dispScore58fc >= threshold) {
            LcwGrantExtend(&g_LcwHiScore);
            ++g_LcwExtendIdx;
        }
    }
}

int LcwSoundTest::Update() {
    switch (state30) {
    case 0:
        cursor34.SetIndex(0);
        cursor34.count = 2;
        cursor34.wraps = 1;
        cursor10c.SetIndex(0);
        cursor10c.count = 0x4c;
        cursor10c.wraps = 1;
        state30 = 1;
    case 1:
        cursor34.prev = cursor34.cur;
        if ((g_LcwInputPressed & 0x20u) != 0 || (g_LcwInputRepeat & 0x20u) != 0) {
            cursor34.Move(1);
        }
        if ((g_LcwInputPressed & 0x10u) != 0 || (g_LcwInputRepeat & 0x10u) != 0) {
            cursor34.Move(-1);
        }
        switch (cursor34.cur) {
        case 0:
            cursor10c.prev = cursor10c.cur;
            if ((g_LcwInputPressed & 0x80u) != 0 || (g_LcwInputRepeat & 0x80u) != 0) {
                cursor10c.Move(1);
            }
            if ((g_LcwInputPressed & 0x40u) != 0 || (g_LcwInputRepeat & 0x40u) != 0) {
                cursor10c.Move(-1);
            }
            if ((g_LcwInputPressed & 0x80201u) != 0) {
                LcwPlaySfx(cursor10c.cur);
            }
            if ((g_LcwInputPressed & 0x102u) != 0) {
                LcwStopSfx(cursor10c.cur);
            }
            break;
        case 1:
            if ((g_LcwInputPressed & 0x80201u) != 0) {
                g_LcwSceneNext = ((g_LcwSupFlags & 0x2000u) == 0) ? 3 : 2;
            }
            break;
        }
        break;
    }
    return 1;
}

void LcwNameEntry::Init() {
    if (g_LcwCurScene == 0x10 && flag1f4 != 0) {
        g_LcwActiveTable = g_LcwTable1;
        g_LcwCurScene = 0x17;
        g_LcwNextScene = 0x17;
    }
    int rank = LcwInsertScoreEntry(&g_LcwScoreFile->routes8[g_LcwRouteIdx]);
    if (g_LcwCurScene == 0x10 && flag1f4 != 0) {
        g_LcwActiveTable = g_LcwTable0;
        g_LcwCurScene = 0x10;
        g_LcwNextScene = 0x10;
    }
    if (rank >= 0) {
        cursor38.count = 0x19;
        cursor38.wraps = 1;
        cursor38.SetIndex(rank);
        cursor110.SetIndex(0);
        cursor110.count = 0x5b;
        cursor110.wraps = 1;
        strcpy(name2cc, g_LcwScoreFile->lastName3374);
        namePos1f0 = 0;
        if (strcmp(name2cc, "        ") != 0) {
            cursor110.Move(-1);
        }
        int len = 8;
        while (len > 0 && name2cc[len - 1] == ' ') {
            --len;
        }
        namePos1f0 = len;
        failed1f8 = 0;
    } else {
        failed1f8 = 1;
    }
}

void LcwGui::PreCleanup() {
    if (msg5a1c != NULL) {
        msg5a1c->Cleanup();
        msg5a1c = NULL;
    }
    if ((g_LcwStageFlags & 9u) == 0) {
        if (buf5a20 != NULL) {
            free(buf5a20);
            buf5a20 = NULL;
        }
        buf5a20 = NULL;
        g_LcwSavedMsgBuf = NULL;
    } else {
        g_LcwSavedMsgBuf = buf5a20;
    }
    if (chain8 != NULL) {
        chain8->flags4 &= ~2u;
    }
    LcwDeleteVmByIdPtr(&id5888);
    LcwDeleteVmByIdPtr(&id588c);
    LcwDeleteVmByIdPtr(&id5898);
    for (int i = 0; i < 10; ++i) {
        arr589c[i] = 0;
    }
    for (int i = 0; i < 10; ++i) {
        LcwAnmVm *vm = g_LcwAnmMgr->FindVmInline(anmIds5860[i]);
        if (vm != NULL) {
            vm->flags4b0 |= 0x20000000u;
            if (vm->parent18 == NULL) {
                for (LcwAnmNode *n = vm->children14; n != NULL; n = n->next4) {
                    n->vm0->flags4b0 |= 0x20000000u;
                }
            }
        }
        anmIds5860[i] = 0;
    }
    f5914 = 0;
    f5968 = 0;
    f59bc = 0;
    flags5a04 |= 0x20u;
    flags5a04 |= 0x40u;
    flags5a04 |= 0x80u;
    f5954 = 0;
    f59a8 = 0;
    f59fc = 0;
}

int LcwReplayMgr::StepInput() {
    if (g_LcwReplayActive == 0) {
        return 1;
    }
    if (stages138[curStage3ac].frameIdx14 < 0) {
        g_LcwInputState.cur218 = 0;
        g_LcwInputState.ext1_224 = 0;
        g_LcwInputState.ext2_228 = 0;
        return 1;
    }
    if (curStage3ac >= 0) {
        g_LcwInputState.prev21c = g_LcwInputState.cur218;
        if (stages138[curStage3ac].frameIdx14 < stages138[curStage3ac].hdr10->frameCount4) {
            unsigned short *p = stages138[curStage3ac].curFrame4;
            if (p[0] == 0xffff && p[1] == 0xffff && p[2] == 0xffff) {
                g_LcwInputState.cur218 = 0;
                g_LcwInputState.ext1_224 = 0;
                g_LcwInputState.ext2_228 = 0;
                LcwOnReplayEnd();
                return 1;
            }
            g_LcwInputState.cur218 = p[0];
            g_LcwInputState.ext1_224 = stages138[curStage3ac].curFrame4[1];
            g_LcwInputState.ext2_228 = stages138[curStage3ac].curFrame4[2];
            g_LcwInputState.UpdateRepeatMasks();
            slowRate3a0 = *stages138[curStage3ac].slowPtrC;
            stages138[curStage3ac].curFrame4 += 3;
            if (totalFrames3a4 % 30 == 0) {
                ++stages138[curStage3ac].slowPtrC;
            }
        } else {
            g_LcwInputState.cur218 = 0;
            g_LcwInputState.ext1_224 = 0;
            g_LcwInputState.ext2_228 = 0;
        }
        ++stages138[curStage3ac].frameIdx14;
        ++totalFrames3a4;
        return 1;
    }
    g_LcwInputState.cur218 = 0;
    g_LcwInputState.ext1_224 = 0;
    g_LcwInputState.ext2_228 = 0;
    ++totalFrames3a4;
    return 1;
}

} // namespace th128
