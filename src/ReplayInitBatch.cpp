#include "ReplayInitBatch.hpp"
#include <string.h>

RibReplayMgr *g_RibReplayMgr;
RibPlayerData *g_RibPlayerData;
RibGameState g_RibGameState;
unsigned short g_RibRngSeed;
int g_RibRngCount;
unsigned int g_RibFlag4d33cc;

inline RibDataHeader::RibDataHeader() {
    memset(this, 0, sizeof(RibDataHeader));
}

inline RibStageHeader::RibStageHeader() {
    memset(this, 0, sizeof(RibStageHeader));
}

int RibReplayMgr::Initialize(const char *filename, int m) {
    mode = m;
    if (m == 0) {
        g_RibReplayMgr = this;
        ResetStageBlocks(g_RibGameState.stageNum);
        curBlockNode = AllocStageBlock(g_RibGameState.stageNum);
        fileHeader = new RibFileHeader();
        dataHeader = new RibDataHeader();
        stageHeaders[g_RibGameState.stageNum] = new RibStageHeader();
        RibStageHeader *sh = stageHeaders[g_RibGameState.stageNum];

        dataHeader->route = g_RibGameState.route;
        dataHeader->field60 = g_RibGameState.field38;
        dataHeader->rank = g_RibGameState.rank;
        dataHeader->flagA = (unsigned char)((g_RibGameState.flags >> 4) & 1);
        if (g_RibPlayerData != 0) {
            dataHeader->sub = g_RibPlayerData->sub;
        }

        sh->stage = (unsigned short)g_RibGameState.stageNum;
        sh->seed = g_RibRngSeed;
        g_RibRngCount = 0;
        sh->flag8c = g_RibFlag4d33cc & 1;
        if (g_RibFlag4d33cc != 0) {
            sh->field18 = 0;
            sh->field1c = 0;
        }
        sh->score = g_RibGameState.score;
        sh->field10 = g_RibGameState.GetVal();
        sh->field14 = g_RibGameState.field1c;
        sh->field80 = g_RibGameState.fieldA4;
        sh->field84 = g_RibGameState.fieldB0;
        sh->field88 = g_RibGameState.floatA0;
        sh->field20 = g_RibGameState.field68;
        sh->field28 = g_RibGameState.field80;
        for (int i = 0; i < 20; ++i) {
            sh->array30[i] = (unsigned int)i * 0xdeaddeadu;
        }
        dataHeader->field6c = g_RibGameState.field68;

        ChainElem *e0 = new ChainElem();
        e0->SetCallback(&RibCalc0);
        e0->flags &= ~2u;
        e0->arg = this;
        e0->flags |= 1u;
        g_Chain->RegisterCalc(e0, 0xc);
        calcElem0 = e0;

        ChainElem *e1 = new ChainElem();
        e1->SetCallback(&RibCalc2);
        e1->flags &= ~2u;
        e1->arg = this;
        e1->flags |= 1u;
        g_Chain->RegisterCalc(e1, 0x1f);
        calcElem1 = e1;

        ChainElem *e2 = new ChainElem();
        e2->SetCallback(&RibDraw);
        e2->flags &= ~2u;
        e2->arg = this;
        e2->flags |= 1u;
        g_Chain->RegisterDraw(e2, 0x3f);
        drawElem = e2;

        curStage = g_RibGameState.stageNum;
        field3a4 = -1;
        return 0;
    }

    if (m == 1) {
        g_RibReplayMgr = this;
        if (LoadReplay(filename) != 0) {
            return -1;
        }
        g_RibPlayerData->sub = dataHeader->sub;
        RibPlaybackSlot *slot = &slots[g_RibGameState.stageNum];
        RibStageHeader *sh = slot->stageHeader;
        slot->curA = slot->bufA;
        slot->curB = slot->bufB;
        slot->state = -1;

        g_RibGameState.route = dataHeader->route;
        g_RibGameState.field38 = dataHeader->field60;
        g_RibGameState.rank = dataHeader->rank;
        g_RibRngSeed = sh->seed;
        g_RibRngCount = 0;
        g_RibGameState.score = sh->score;
        g_RibGameState.field08 = sh->field10;
        if (g_RibGameState.field08 > g_RibGameState.maxVal) {
            g_RibGameState.field08 = g_RibGameState.maxVal;
        } else if (g_RibGameState.field08 < g_RibGameState.minVal) {
            g_RibGameState.field08 = g_RibGameState.minVal;
        }
        g_RibGameState.field1c = sh->field14;
        g_RibGameState.fieldA4 = sh->field80;
        g_RibGameState.fieldA8 = 0;
        if (g_RibGameState.fieldA4 > 100000) {
            g_RibGameState.fieldA4 = 100000;
        }
        g_RibGameState.fieldB0 = sh->field84;
        g_RibGameState.floatA0 = sh->field88;
        g_RibGameState.field68 = sh->field20;
        g_RibGameState.field80 = sh->field28;

        calcElem0 = g_Chain->CreateAndRegisterCalc(&RibCalc1, this, 0xc);

        ChainElem *e1 = new ChainElem();
        e1->SetCallback(&RibCalc2);
        e1->flags &= ~2u;
        e1->arg = this;
        e1->flags |= 1u;
        g_Chain->RegisterCalc(e1, 0x1f);
        calcElem1 = e1;

        drawElem = g_Chain->CreateAndRegisterDraw(&RibDraw, this, 0x3f);
        curStage = -1;
        return 0;
    }

    if (m == 2) {
        if (LoadReplay(filename) != 0) {
            return -1;
        }
    }
    return 0;
}
