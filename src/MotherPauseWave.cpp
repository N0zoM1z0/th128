#include "MotherPauseWave.hpp"

#include <process.h>
#include <stdlib.h>
#include <string.h>

namespace th128 {

int __stdcall MpwMother::Init() {
    MpwChainElem *c = new MpwChainElem();
    c->flags4 = (c->flags4 & ~2u) | 1u;
    c->cb8 = (void *)&MpwMotherCalc;
    c->onRegC = NULL;
    c->onCut10 = NULL;
    c->arg20 = this;
    g_MpwChain->RegisterCalc(c, 3);
    calc8 = c;

    MpwChainElem *d = new MpwChainElem();
    d->flags4 = (d->flags4 & ~2u) | 1u;
    d->cb8 = (void *)&MpwMotherDraw;
    d->onRegC = NULL;
    d->onCut10 = NULL;
    d->arg20 = this;
    g_MpwChain->RegisterDraw(d, 0x3c);
    drawC = d;

    thread10.Stop();
    thread10.proc18 = &MpwMotherThread;
    thread10.running10 = 1;
    thread10.fieldC = 0;
    thread10.handle4 = (HANDLE)_beginthreadex(NULL, 0, &MpwMotherThread, this, 0, &thread10.threadId8);
    return 0;
}

int MpwSupervisor::Shutdown() {
    while (MpwWaitSoundReady() != 0) {
    }
    g_MpwAudioMode = 2;
    g_MpwSupThread.Stop();
    if (g_MpwCfgAlloc != NULL) {
        free(g_MpwCfgAlloc);
        g_MpwCfgAlloc = NULL;
    }
    MpwCleanupSub();
    if (g_MpwGlobalObj != NULL) {
        delete g_MpwGlobalObj;
    }
    if (g_MpwAnmMgr->res4e9698 != NULL) {
        g_MpwAnmMgr->res4e9698->Release();
        g_MpwAnmMgr->res4e9698 = NULL;
    }
    MpwEnqueueBgmCommand(g_MpwSoundMgrPtr, 4, 0, "dummy");
    MpwDestroyTextImage();
    DeleteObject(g_MpwGdiObjs[0]);
    DeleteObject(g_MpwGdiObjs[1]);
    DeleteObject(g_MpwGdiObjs[2]);
    DeleteObject(g_MpwGdiObjs[3]);
    DeleteObject(g_MpwGdiObjs[4]);
    DeleteObject(g_MpwGdiObjs[5]);
    DeleteObject(g_MpwGdiObjs[6]);
    DeleteObject(g_MpwGdiObjs[7]);
    if (kbDev20 != NULL) {
        kbDev20->Unacquire();
        if (kbDev20 != NULL) {
            kbDev20->Release();
            kbDev20 = NULL;
        }
    }
    if (joyDev24 != NULL) {
        joyDev24->Unacquire();
        if (joyDev24 != NULL) {
            joyDev24->Release();
            joyDev24 = NULL;
        }
    }
    if (dinputC != NULL) {
        dinputC->Release();
        dinputC = NULL;
    }
    g_MpwArchive.Close();
    if (g_MpwBufObjs[0] != NULL) {
        delete g_MpwBufObjs[0];
        g_MpwBufObjs[0] = NULL;
    }
    if (g_MpwBufObjs[1] != NULL) {
        delete g_MpwBufObjs[1];
        g_MpwBufObjs[1] = NULL;
    }
    if (g_MpwBufObjs[2] != NULL) {
        delete g_MpwBufObjs[2];
        g_MpwBufObjs[2] = NULL;
    }
    return 0;
}

int MpwRegisterSupervisorChains() {
    g_MpwSceneCur = -2;
    g_MpwSceneNext = 0;
    g_MpwSceneFlag = 0;

    MpwChainElem *c = new MpwChainElem();
    c->flags4 |= 3u;
    c->cb8 = (void *)&MpwSupCalc;
    c->onCut10 = NULL;
    c->arg20 = &g_MpwSupervisor;
    c->onRegC = (int (__fastcall *)(void *))&MpwSupOnReg;
    int err = g_MpwChain->RegisterCalc(c, 1);
    if (err != 0) {
        return err;
    }

    MpwChainElem *d0 = new MpwChainElem();
    d0->flags4 |= 3u; d0->cb8 = (void *)&MpwSupDraw0; d0->onRegC = NULL; d0->onCut10 = NULL; d0->arg20 = &g_MpwSupervisor;
    g_MpwChain->RegisterDraw(d0, 1);

    MpwChainElem *d1 = new MpwChainElem();
    d1->flags4 |= 3u; d1->cb8 = (void *)&MpwSupDraw1; d1->onRegC = NULL; d1->onCut10 = NULL; d1->arg20 = &g_MpwSupervisor;
    g_MpwChain->RegisterDraw(d1, 0xc);

    MpwChainElem *d2 = new MpwChainElem();
    d2->flags4 |= 3u; d2->cb8 = (void *)&MpwSupDraw2; d2->onRegC = NULL; d2->onCut10 = NULL; d2->arg20 = &g_MpwSupervisor;
    g_MpwChain->RegisterDraw(d2, 0xe);

    MpwChainElem *d3 = new MpwChainElem();
    d3->flags4 |= 3u; d3->cb8 = (void *)&MpwSupDraw3; d3->onRegC = NULL; d3->onCut10 = NULL; d3->arg20 = &g_MpwSupervisor;
    g_MpwChain->RegisterDraw(d3, 0x27);

    MpwChainElem *d4 = new MpwChainElem();
    d4->flags4 |= 3u; d4->cb8 = (void *)&MpwSupDraw4; d4->onRegC = NULL; d4->onCut10 = NULL; d4->arg20 = &g_MpwSupervisor;
    g_MpwChain->RegisterDraw(d4, 0x29);

    MpwChainElem *d5 = new MpwChainElem();
    d5->flags4 |= 3u; d5->cb8 = (void *)&MpwSupDraw5; d5->onRegC = NULL; d5->onCut10 = NULL; d5->arg20 = &g_MpwSupervisor;
    g_MpwChain->RegisterDraw(d5, 0x33);

    MpwChainElem *d6 = new MpwChainElem();
    d6->flags4 |= 3u; d6->cb8 = (void *)&MpwSupDraw6; d6->onRegC = NULL; d6->onCut10 = NULL; d6->arg20 = &g_MpwSupervisor;
    g_MpwChain->RegisterDraw(d6, 0x34);

    MpwChainElem *d7 = new MpwChainElem();
    d7->flags4 |= 3u; d7->cb8 = (void *)&MpwSupDraw7; d7->onRegC = NULL; d7->onCut10 = NULL; d7->arg20 = &g_MpwSupervisor;
    g_MpwChain->RegisterDraw(d7, 0x4c);

    return 0;
}

int MpwSupervisor::LoadConfig() {
    g_MpwConfig.ResetDefaults();
    char path[0x1000];
    strcpy(path, g_MpwAppDataDir);
    strcat(path, "th128.cfg");
    DWORD sz;
    MpwConfig *loaded = (MpwConfig *)MpwLoadFile(path, &sz, 1);
    if (loaded == NULL) {
        g_MpwErrorCtx.Log("cfg not found\n");
        g_MpwConfig.ResetDefaults();
    } else {
        g_MpwConfig = *loaded;
        free(loaded);
        if (g_MpwConfig.f1a >= 2 || g_MpwConfig.f1b >= 3 || g_MpwConfig.f1c >= 2 ||
            g_MpwConfig.f1d >= 4 || g_MpwConfig.f1e >= 3 || g_MpwConfig.f1f >= 3 ||
            g_MpwConfig.version0 != 0x128001u || sz != sizeof(MpwConfig)) {
            g_MpwErrorCtx.Log("cfg invalid\n");
            g_MpwConfig.ResetDefaults();
        } else {
            g_MpwInputKeys[0] = g_MpwConfig.keys4[0];
            g_MpwInputKeys[1] = g_MpwConfig.keys4[1];
            g_MpwInputKeys[2] = g_MpwConfig.keys4[2];
            g_MpwInputKeys[3] = g_MpwConfig.keys4[3];
            g_MpwInputKey10 = g_MpwConfig.key14;
        }
    }
    flag57c = 0;
    if (flags200 & 4) g_MpwErrorCtx.Log("f4\n");
    if (flags200 & 1) g_MpwErrorCtx.Log("f1\n");
    if (flag114 != 0) g_MpwErrorCtx.Log("f114\n");
    if (flags200 & 2) g_MpwErrorCtx.Log("f2\n");
    if (flags200 & 8) g_MpwErrorCtx.Log("f8\n");
    if (flags200 & 0x10) g_MpwErrorCtx.Log("f10\n");
    if (flags200 & 0x20) {
        g_MpwErrorCtx.Log("f20\n");
        g_MpwSupFlag3e4 = 1;
    }
    if (flags200 & 0x40) g_MpwErrorCtx.Log("f40\n");

    if (MpwWriteDataToFile(path, &g_MpwConfig, sizeof(MpwConfig)) != 0) {
        g_MpwErrorCtx.Fatal("write fail %s\n", "th128.cfg");
        g_MpwErrorCtx.Fatal("write fail2\n");
        return -1;
    }
    return 0;
}

void MpwSupervisor::SpawnLoadingVmsA(const MpwVec3 *pos) {
    if (loadingActive93c != 0) {
        return;
    }
    g_MpwLoadVmIds[0] = loadAnm58c->CreateVm(NULL, 0, 0x17, 0);
    g_MpwLoadVmIds[1] = loadAnm58c->CreateVm(NULL, 1, 0x17, 0);
    g_MpwLoadVmIds[2] = loadAnm58c->CreateVm(NULL, 2, 0x17, 0);
    loadingActive93c = 1;

    MpwAnmVm *vm = g_MpwAnmMgr->GetVmById(g_MpwLoadVmIds[0]);
    if (vm != NULL) {
        vm->pos464 = *pos;
    }
    vm = g_MpwAnmMgr->GetVmById(g_MpwLoadVmIds[1]);
    if (vm != NULL) {
        vm->pos464 = *pos;
    }
    vm = g_MpwAnmMgr->GetVmById(g_MpwLoadVmIds[2]);
    if (vm != NULL) {
        vm->pos464 = *pos;
    }
}

void MpwSupervisor::SpawnLoadingVmsB(const MpwVec3 *pos) {
    if (loadingActive93c != 0) {
        return;
    }
    g_MpwLoadVmIds[0] = loadAnm58c->CreateVm(NULL, 0, 0x17, 0);
    g_MpwLoadVmIds[1] = loadAnm58c->CreateVm(NULL, 1, 0x17, 0);
    g_MpwLoadVmIds[2] = loadAnm58c->CreateVm(NULL, 2, 0x17, 0);
    loadingActive93c = 1;

    MpwAnmVm *vm = g_MpwAnmMgr->GetVmById(g_MpwLoadVmIds[0]);
    if (vm != NULL) {
        vm->pos464 = *pos;
    }
    vm = g_MpwAnmMgr->GetVmById(g_MpwLoadVmIds[1]);
    if (vm != NULL) {
        vm->pos464 = *pos;
    }
    vm = g_MpwAnmMgr->GetVmById(g_MpwLoadVmIds[2]);
    if (vm != NULL) {
        vm->pos464 = *pos;
    }
}

void MpwUpdateBgmFade() {
    if (g_MpwStreamSound == NULL) {
        return;
    }
    if (g_MpwStreamSound->fadeMode1c == 1) {
        --g_MpwStreamSound->fadeRem14;
        if (g_MpwStreamSound->fadeRem14 <= 0) {
            g_MpwStreamSound->fadeMode1c = 0;
            g_MpwStreamSound->buffers4[0]->Stop();
        } else {
            g_MpwStreamSound->SetVolume(g_MpwStreamSound->fadeRem14 * 5000 / g_MpwStreamSound->fadeTotal18 - 5000);
        }
    }
    if (g_MpwStreamSound->fadeMode1c == 2) {
        --g_MpwStreamSound->fadeRem14;
        if (g_MpwStreamSound->fadeRem14 <= 0) {
            g_MpwStreamSound->fadeMode1c = 0;
        } else {
            g_MpwStreamSound->SetVolume(g_MpwStreamSound->fadeRem14 * -5000 / g_MpwStreamSound->fadeTotal18);
        }
    }
    if (g_MpwStreamSound->fadeMode1c == 4) {
        --g_MpwStreamSound->fadeRem14;
        if (g_MpwStreamSound->fadeRem14 <= 0) {
            g_MpwStreamSound->fadeMode1c = 0;
        } else {
            g_MpwStreamSound->SetVolume(g_MpwStreamSound->fadeRem14 * 1000 / g_MpwStreamSound->fadeTotal18 - 1000);
        }
    }
    if (g_MpwStreamSound->fadeMode1c == 3) {
        --g_MpwStreamSound->fadeRem14;
        if (g_MpwStreamSound->fadeRem14 <= 0) {
            g_MpwStreamSound->fadeMode1c = 0;
        } else {
            g_MpwStreamSound->SetVolume(g_MpwStreamSound->fadeRem14 * -1000 / g_MpwStreamSound->fadeTotal18);
        }
    }
}

void MpwPauseMenu::InitScoreNameEntry() {
    if (g_MpwStageNum == 0x10 && isPractice1f4 != 0) {
        g_MpwStageTablePtr = g_MpwStageTableB;
        g_MpwStageNum = 0x17;
        g_MpwStageNumCopy = 0x17;
    }
    int rank = MpwInsertScoreEntry(g_MpwScoreFile->routes8[g_MpwRouteIdx]);
    if (g_MpwStageNum == 0x10 && isPractice1f4 != 0) {
        g_MpwStageTablePtr = g_MpwStageTableA;
        g_MpwStageNum = 0x10;
        g_MpwStageNumCopy = 0x10;
    }
    if (rank < 0) {
        noHighScore1f8 = 1;
        return;
    }
    scoreCursor38.count = 0x19;
    scoreCursor38.flagD0 = 1;
    if (scoreCursor38.count != 0 && rank >= scoreCursor38.count) {
        rank = scoreCursor38.count - 1;
    }
    scoreCursor38.cur = rank;

    if (charCursor110.count == 0) {
        charCursor110.cur = 0;
    } else if (charCursor110.count > 0) {
        charCursor110.cur = 0;
    } else {
        charCursor110.cur = charCursor110.count - 1;
    }
    charCursor110.count = 0x5b;
    charCursor110.flagD0 = 1;

    strcpy(nameBuf2cc, g_MpwScoreFile->lastName3374);
    nameLen1f0 = 0;
    if (strcmp(nameBuf2cc, "        ") != 0) {
        charCursor110.Move(-1);
    }
    int len = 8;
    while (len > 0 && nameBuf2cc[len - 1] == ' ') {
        --len;
    }
    nameLen1f0 = len;
    noHighScore1f8 = 0;
}

} // namespace th128
