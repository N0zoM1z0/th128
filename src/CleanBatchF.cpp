#include "CleanBatchF.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CbfAnmMgr *g_CbfAnmMgr;
CbfPlayerData *g_CbfPlayerData;
CbfStageObj *g_CbfStage;
unsigned int g_CbfSupFlags;
CbfLog g_CbfLog;
char g_CbfPathBuf[256];
void *g_CbfMainMgr;

// 1. 0x00410ea0 (222B)
void __stdcall CbfCleanup410ea0(CbfCleanupObj *obj) {
    CbfAnmMgr *mgr = g_CbfAnmMgr;
    for (int i = 0; i < 8; ++i) {
        void *slot = mgr->slots[i];
        CbfListNode *n = mgr->list1;
        while (n != 0) {
            CbfListNode *next = n->next;
            CbfAnmVm *vm = (CbfAnmVm *)n->data;
            if (vm->slot == slot) {
                vm->flags |= 0x20000000;
            }
            n = next;
        }
        n = mgr->list2;
        while (n != 0) {
            CbfListNode *next = n->next;
            CbfAnmVm *vm = (CbfAnmVm *)n->data;
            if (vm->slot == slot) {
                vm->flags |= 0x20000000;
            }
            n = next;
        }
    }
    CbfListNode *item = obj->itemList;
    while (item != 0) {
        CbfListNode *next = item->next;
        CbfVObj *v = (CbfVObj *)item->data;
        if (v != 0) {
            delete v;
        }
        item = next;
    }
    obj->fieldc4 = 0;
    obj->field5c = 0;
    if (obj->calcElem != 0) {
        obj->calcElem->flags &= ~2U;
    }
    if (obj->drawElem != 0) {
        obj->drawElem->flags &= ~2U;
    }
}

// 2. 0x00420020 (165B)
void __stdcall CbfDecodeLivesBombs(int *outLives, int *outBombs) {
    CbfPlayerData *pd = g_CbfPlayerData;
    int v = pd->encodedA8;
    int a = (v / 100 % 1000 + 0x3a6) % 1000;
    int b = (v % 100 + 0x43) % 100;
    if (v / 100000 - 0x16 != a + b) {
        *outLives = 999;
        *outBombs = 99;
        return;
    }
    int v2 = pd->encodedA8;
    *outLives = (v2 / 100 % 1000 + 0x3a6) % 1000;
    *outBombs = (v2 % 100 + 0x43) % 100;
}

// 3. 0x004154b0 (341B)
int CbfStageLoader::LoadResources(CbfChunkHeader *hdr) {
    if (hdr->magic != 0x4d494e41) {
        return 0;
    }
    const char *p = (const char *)(hdr + 1);
    for (unsigned int i = 0; i < hdr->count; ++i) {
        CbfPreloadAnmObj *anm;
        if (g_CbfAnmMgr->slots[i] != 0) {
            CbfDebugLog("::preloadAnim already : %s\n", p);
            anm = (CbfPreloadAnmObj *)g_CbfAnmMgr->slots[i];
        } else {
            anm = CbfPreloadAnm(g_CbfAnmMgr, i + 8, p);
            if (anm != 0) {
                anm->busy124 = 1;
                do {
                    if (g_CbfSupFlags & 0x180) {
                        break;
                    }
                    Sleep(1);
                } while (anm->busy124 != 0);
                CbfDebugLog("::preloadAnimEnd : %s\n", p);
            }
        }
        g_CbfStage->anms[i] = anm;
        if (anm == 0) {
            g_CbfLog.Print("\x83" "f\x81[\x83^\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");
            return -1;
        }
        p += strlen(p) + 1;
    }
    int rem = (p - (const char *)hdr) % 4;
    if (rem != 0) {
        p += 4 - rem;
    }
    CbfChunkHeader *ecli = (CbfChunkHeader *)p;
    if (ecli->magic == 0x494c4345) {
        p = (const char *)(ecli + 1);
        for (unsigned int j = 0; j < ecli->count; ++j) {
            LoadEcl(p);
            p += strlen(p) + 1;
        }
    }
    return 0;
}

// 4. 0x00405130 (299B)
int __stdcall CbfLoadStd(CbfStdObj *obj, const char *name) {
    if (obj->rawData == 0) {
        g_CbfPathBuf[0] = 0;
        strcat(g_CbfPathBuf, name);
        obj->rawData = CbfOpenFile(g_CbfPathBuf, &obj->rawSize, 0);
        if (obj->rawData == 0) {
            return -1;
        }
    }
    obj->buf10 = (unsigned char *)malloc(obj->rawSize);
    memcpy(obj->buf10, obj->rawData, obj->rawSize);
    obj->anm = CbfLoadAnm((obj->flags3810 & 1) + 3, (const char *)(obj->buf10 + 0x10));
    if (obj->anm == 0) {
        g_CbfLog.Print("\x83X\x83" "e\x81[\x83W\x83" "f\x81[\x83^\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x81" "B\x83" "f\x81[\x83^\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");
        return -1;
    }
    CbfStdHeader *hdr = (CbfStdHeader *)obj->buf10;
    obj->ptr14 = (unsigned int *)(obj->buf10 + 0x90);
    obj->ptr18 = obj->buf10 + hdr->off04;
    obj->ptr1c = obj->buf10 + hdr->off08;
    for (int i = 0; i < ((CbfStdHeader *)obj->buf10)->count00; ++i) {
        obj->ptr14[i] += (unsigned int)obj->buf10;
    }
    obj->ptr1c8 = malloc(((CbfStdHeader *)obj->buf10)->count02 * 0x4e8);
    return 0;
}

// 5. 0x004017f0 (270B)
void *__stdcall CbfFormatCommaNumber(void *out, int val, int arg) {
    char buf[0x100];
    void *mgr = g_CbfMainMgr;
    if (val < 1000) {
        sprintf(buf, "%d", val);
    } else if (val < 1000000) {
        sprintf(buf, "%d,%.3d", val / 1000, val % 1000);
    } else if (val < 1000000000) {
        sprintf(buf, "%d,%.3d,%.3d", val / 1000000 % 1000, val / 1000 % 1000, val % 1000);
    } else {
        sprintf(buf, "%d,%.3d,%.3d,%.3d", val / 1000000000 % 1000, val / 1000000 % 1000, val / 1000 % 1000, val % 1000);
    }
    CbfSetText(mgr, buf, arg, out);
    return out;
}
