#include "CleanSmallWaveC.hpp"

#include <stdlib.h>

namespace th128 {

// 1. 0x0041FBB0 (83B)
CscGuiSubObj::~CscGuiSubObj() {
    CscDeleteVmByIdPtr(&this->vmIds[0]);
    CscDeleteVmByIdPtr(&this->vmIds[1]);
    CscDeleteVmByIdPtr(&this->vmIds[2]);
    CscDeleteVmByIdPtr(&this->vmIds[3]);
    CscDeleteVmByIdPtr(&this->vmIds[4]);
    CscDeleteVmByIdPtr(&this->vmIds[5]);
    CscDeleteVmByIdPtr(&this->vmIds[6]);
    CscDeleteVmByIdPtr(&this->vmIds[7]);
    CscDeleteVmByIdPtr(&this->vmIds[8]);
    CscDeleteVmByIdPtr(&this->vmIds[9]);
}

// 2. 0x00444150 (83B)
void CscLineArray::FreeLines() {
    if (this->lines != NULL) {
        for (int i = 0; i < this->count; ++i) {
            void *p = this->lines[i];
            if (p != NULL) {
                free(p);
                this->lines[i] = NULL;
            }
        }
        if (this->lines != NULL) {
            free(this->lines);
            this->lines = NULL;
        }
        this->lines = NULL;
    }
}

// 3. 0x00442820 (82B)
unsigned int CscScoreBuf::CalcChecksum(int totalLen) {
    unsigned int sum = 0;
    int len = totalLen - 8;
    for (int i = 0; i < len; ++i) {
        sum += this->data[i + 8];
    }
    return sum;
}

// 4. 0x0041F800 (86B)
int CscFogMgr::RegisterDrawChain() {
    CscChainElem *elem = new CscChainElem();
    elem->flags |= 3u;
    elem->cb = (void *)CscFogDrawCb;
    elem->f0c = NULL;
    elem->f10 = NULL;
    elem->arg20 = this;
    CscRegisterDraw(elem, 0x43);
    this->drawElem = elem;
    return 0;
}

// 5. 0x0046D420 (77B)
HRESULT CscStreamingSound::Resume() {
    if (this->buffers == NULL) {
        return (HRESULT)0x800401f0;
    }
    if (this->paused == 0) {
        return (HRESULT)0x800401f0;
    }
    this->paused = 0;
    this->waveFile->ReopenTrack(this->waveFile->desc, this->waveFile->handle);
    this->playing = 1;
    return this->buffers[0]->Play(0, this->playPriority, this->playFlags);
}

// 6. 0x004140C0 (82B)
int CscLoadEndingAnmWorkerA() {
    CscEndingInner *in = g_CscEndingMgr->inner;
    void *anm = CscPreloadAnm(in->slotIdx + 0x20, in->anmPath);
    in->anms[in->slotIdx] = anm;
    in->flags74 &= ~4u;
    CscAsciiMgr *asc = g_CscAsciiMgr;
    CscStopVmById(asc->loadingVmId, 1);
    asc->loadingVmId = 0;
    return 0;
}

// 7. 0x00413110 (82B)
int CscLoadEndingAnmWorkerB() {
    CscEndingInner *in = g_CscEndingMgr->inner;
    void *anm = CscPreloadAnm(in->slotIdx + 0x20, in->anmPath);
    in->anms[in->slotIdx] = anm;
    in->flags74 &= ~4u;
    CscAsciiMgr *asc = g_CscAsciiMgr;
    CscStopVmById(asc->loadingVmId, 1);
    asc->loadingVmId = 0;
    return 0;
}

// 8. 0x00434900 (77B)
int __stdcall CscPlayPreloadedBgm(int slot, int bgmIdx) {
    if (g_CscSoundFlag & 0x10) {
        CscEnqueueBgmCommand(&g_CscSoundMgr, 4, 0, "dummy");
    }
    CscEnqueueBgmCommand(&g_CscSoundMgr, 2, slot, "dummy");
    g_CscScoreData->bgmUnlocked[bgmIdx] = 1;
    return 0;
}

} // namespace th128
