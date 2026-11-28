#include "LowFpuWaveB.hpp"

#include <string.h>

namespace th128 {

// 1. 0x00410500 (138B)
void LfbEffectNode::Release() {
    this->active0c = 0;
    this->timer20.ResetToMinusOne();
    LfbDeleteVmByIdPtr(&this->vmId50);
    LfbDeleteVmByIdPtr(&this->vmId54);
    if (this->next04 != NULL) {
        this->next04->prev08 = this->prev08;
    }
    if (this->prev08 != NULL) {
        this->prev08->next04 = this->next04;
    }
    this->next04 = NULL;
    this->prev08 = NULL;
    if (this->next60 != NULL) {
        this->next60->prev08 = this->prev64;
    }
    if (this->prev64 != NULL) {
        this->prev64->next04 = this->next60;
    }
    this->prev64 = NULL;
    this->next60 = NULL;
}

// 2. 0x004382D0 (129B)
void LfbNameEntry::EnterMode15() {
    if (g_LfbGameState->isReplay74 == 0) {
        g_LfbPauseStartTime = LfbGetElapsedSeconds();
    }
    this->mode04 = 0x15;
    this->timer10.ResetToMinusOne();
    LfbStopVmById(this->vmId1ec, 1);
    LfbStopVmById(this->vmId1e8, 1);
    g_LfbTimeScale = this->savedTimeScale2d8;
}

// 3. 0x004391E0 (129B)
void LfbNameEntry::EnterMode1C() {
    if (g_LfbGameState->isReplay74 == 0) {
        g_LfbPauseStartTime = LfbGetElapsedSeconds();
    }
    this->mode04 = 0x1c;
    this->timer10.ResetToMinusOne();
    LfbStopVmById(this->vmId1ec, 1);
    LfbStopVmById(this->vmId1e8, 1);
    g_LfbTimeScale = this->savedTimeScale2d8;
}

// 4. 0x00425080 (126B)
void LfbHideBossBar() {
    LfbGui *gui = g_LfbGui;
    LfbStopVmById(gui->vmId58cc, 1);
    LfbStopVmById(gui->vmId58d0, 1);
    gui->flags5a04 &= ~0x100u;
    gui->timer5a08.ResetToMinusOne();
}

// 5. 0x00429B60 (121B)
LfbLaserBase::LfbLaserBase() {
    memset(this, 0, sizeof(LfbLaserBase));
    this->timer14.ResetToMinusOne();
}

// 6. 0x0042A740 (121B)
int LfbCancelAllLasers() {
    LfbLaserCancelObj *node = g_LfbLaserMgr->head18;
    while (node != NULL) {
        LfbLaserCancelObj *next = node->next08;
        node->timer5a0.ResetToMinusOne();
        node->f5b4 = 0;
        node->Cancel(1, 0);
        node = next;
    }
    return 0;
}

// 7. 0x004061E0 (118B)
void LfbBgJumpToLabel(int label) {
    LfbBgMgr *bg = g_LfbBgMgr;
    LfbBgCmd *cmd = bg->scriptStart;
    while (cmd->time >= 0) {
        if (cmd->opcode == 0x10 && cmd->arg0 == label) {
            cmd = (LfbBgCmd *)((char *)cmd + cmd->size);
            bg->curCmd = cmd;
            int t = cmd->time;
            bg->timer38.SetInt(t);
            return;
        }
        cmd = (LfbBgCmd *)((char *)cmd + cmd->size);
    }
}

// 8. 0x00456CB0 (135B)
int LfbScreenFade::OnCalc() {
    if (g_LfbSkipFade != 0) {
        return 7;
    }
    int dur = this->duration1c;
    if (dur != 0) {
        if (this->timer30.cur < dur) {
            this->alpha18 = (int)((double)this->timer30.curF * 255.0 / (double)dur);
        } else {
            this->alpha18 = 0xff;
        }
        if (this->alpha18 < 0) {
            this->alpha18 = 0;
        }
    }
    if (this->timer30.cur >= dur + 2) {
        return 7;
    }
    if (g_LfbGameState != NULL) {
        unsigned int f = g_LfbGameState->flags60;
        if (((f >> 2) | f) & 1) {
            return 1;
        }
    }
    this->timer30.Increment();
    return 1;
}

// 9. 0x00427420 (119B)
void LfbPlayTimeObj::AccumulatePlayTime() {
    if (g_LfbGameState->isReplay74 != 0) {
        return;
    }
    if (g_LfbSceneState == -1 || g_LfbSceneState == 3) {
        return;
    }
    double dt = LfbGetElapsedSeconds() - this->lastTime88;
    if (dt > 0.0) {
        g_LfbScoreMgr->playTimeMs += (__int64)(dt * 1000.0);
    }
    this->lastTime88 = LfbGetElapsedSeconds();
}

} // namespace th128
