#include "PbgWave.hpp"

#include <process.h>
#include <stdlib.h>
#include <string.h>

namespace th128 {

void PbwSetBgmVol() {
    PbwEnqueueBgmCommand(8, 0, "SetVol");
}

__time64_t PbwGetTime64(__time64_t *t) {
    return _time64(t);
}

void PbwAnmVm::SetScriptAndUpdate(short script) {
    if (cb4c8 != NULL) {
        cb4c8(this, (int)script);
    }
    script3f8 = script;
    Update();
}

struct PbwResFileStub : public PbwResFile {
    virtual void Close() {}
};

bool PbwArchive::OpenMem(const char *path) {
    Close();
    file = new PbwResFileStub();
    if (file == NULL) {
        return false;
    }
    if (!ReadHeader(path)) {
        Close();
        return false;
    }
    name = (char *)malloc(strlen(path) + 1);
    if (name != NULL) {
        strcpy(name, path);
    }
    if (name == NULL) {
        Close();
        return false;
    }
    return true;
}

void PbwDeleteArchiveEntries(PbwArchiveEntry *entries) {
    delete[] entries;
}

unsigned int PbwArchive::GetEntrySize(const char *entryName) {
    PbwArchiveEntry *e = entries;
    if (e != NULL) {
        int n = count;
        if (n > 0) {
            do {
                if (_stricmp(entryName, e->name) == 0) {
                    return e->size;
                }
                --n;
                ++e;
            } while (n > 0);
        }
    }
    return 0;
}

PbwArchiveEntry *PbwArchive::FindEntry(const char *entryName) {
    PbwArchiveEntry *e = entries;
    if (e == NULL) {
        return NULL;
    }
    int n = count;
    if (n > 0) {
        do {
            if (_stricmp(entryName, e->name) == 0) {
                return e;
            }
            --n;
            ++e;
        } while (n > 0);
    }
    return NULL;
}

bool PbwOpenArchiveSlot(const char *path) {
    if (g_PbwArchives[g_PbwArchiveCount].Open(path)) {
        g_PbwArchiveCount += 1;
        return true;
    }
    return false;
}

void PbwLzssDeleteNode(int node) {
    if (g_PbwLzssTree[node].parent == 0) {
        return;
    }
    if (g_PbwLzssTree[node].larger == 0) {
        PbwLzssContractNode(node, g_PbwLzssTree[node].smaller);
    } else if (g_PbwLzssTree[node].smaller == 0) {
        PbwLzssContractNode(node, g_PbwLzssTree[node].larger);
    } else {
        int next = PbwLzssFindNextNode(node);
        PbwLzssDeleteNode(next);
        PbwLzssReplaceNode(node, next);
    }
}

void PbwLzssReplaceNode(int oldNode, int newNode) {
    int parent = g_PbwLzssTree[oldNode].parent;
    if (g_PbwLzssTree[parent].smaller == oldNode) {
        g_PbwLzssTree[parent].smaller = newNode;
    } else {
        g_PbwLzssTree[parent].larger = newNode;
    }
    g_PbwLzssTree[newNode] = g_PbwLzssTree[oldNode];
    g_PbwLzssTree[g_PbwLzssTree[newNode].smaller].parent = newNode;
    g_PbwLzssTree[g_PbwLzssTree[newNode].larger].parent = newNode;
    g_PbwLzssTree[oldNode].parent = 0;
}

bool PbwMemFile::Open(const char *name, const char *mode) {
    (void)mode;
    dataC = PbwReadFile(name);
    size4 = PbwGetFileSize(name);
    cur8 = dataC;
    return dataC != NULL;
}

void PbwMemFile::Close() {
}

bool PbwResFileImpl::Open(const char *name, const char *mode) {
    (void)mode;
    Close();
    HRSRC res = FindResourceA(NULL, name, (LPCSTR)10);
    if (res == NULL) {
        Close();
        return true;
    }
    HGLOBAL hMem = LoadResource(NULL, res);
    if (hMem == NULL) {
        Close();
        return true;
    }
    void *ptr = LockResource(hMem);
    if (ptr == NULL) {
        UnlockResource(NULL);
        Close();
        return true;
    }
    DWORD sz = SizeofResource(NULL, res);
    size4 = sz;
    dataC = (unsigned char *)malloc(sz);
    if (dataC == NULL) {
        Close();
        return true;
    }
    memcpy(dataC, ptr, sz);
    cur8 = dataC;
    UnlockResource(hMem);
    return true;
}

void PbwResFileImpl::Close() {
}

PbwThreadInf::~PbwThreadInf() {
    Stop();
}

void PbwThreadInf::Start() {
    flags20 &= ~7u;
    field4c = 0;
    Stop();
    proc18 = PbwThreadProc;
    running10 = 1;
    fieldC = 0;
    handle4 = (void *)_beginthreadex(NULL, 0, PbwThreadProc, NULL, 0, &threadId8);
}

} // namespace th128
