#pragma once

#include <windows.h>
#include <new>
#include <time.h>

namespace th128 {

struct PbwAnmVm {
    unsigned char pad0[0x3f8];
    short script3f8;
    unsigned char pad3fa[0x4c8 - 0x3fa];
    void (__fastcall *cb4c8)(PbwAnmVm *self, int script);

    void Update();
    void SetScriptAndUpdate(short script);
};

struct PbwArchiveEntry {
    char *name;
    unsigned int offset;
    unsigned int size;
    unsigned int reserved;

    ~PbwArchiveEntry();
};

struct PbwResFile {
    void *f4;
    void *f8;
    void *fc;
    PbwResFile() : f4(0), f8(0), fc(0) {}
    virtual void Close() = 0;
};

struct PbwArchive {
    PbwArchiveEntry *entries; // +0x00
    int count;                // +0x04
    char *name;               // +0x08
    PbwResFile *file;         // +0x0C

    void Close();
    bool Open(const char *path);
    bool ReadHeader(const char *path);
    bool OpenMem(const char *path);
    unsigned int GetEntrySize(const char *entryName);
    PbwArchiveEntry *FindEntry(const char *entryName);
};

struct PbwLzssTreeNode {
    int parent;
    int smaller;
    int larger;
};

struct PbwMemFileBase {
    virtual bool Open(const char *name, const char *mode) = 0; // +0x00
    virtual void Close() = 0;                                  // +0x04
};

struct PbwMemFile : public PbwMemFileBase {
    unsigned int size4;
    unsigned char *cur8;
    unsigned char *dataC;

    virtual bool Open(const char *name, const char *mode);
    virtual void Close();
};

struct PbwResFileImpl : public PbwMemFileBase {
    unsigned int size4;
    unsigned char *cur8;
    unsigned char *dataC;

    virtual bool Open(const char *name, const char *mode);
    virtual void Close();
};

struct PbwThreadInf {
    void *handle4;
    unsigned int threadId8;
    int fieldC;
    int running10;
    int field14;
    unsigned int (__stdcall *proc18)(void *);
    int field1c;
    unsigned int flags20;
    unsigned char pad24[0x4c - 0x24];
    int field4c;

    virtual void DummyVFunc() {}
    ~PbwThreadInf();
    void Stop();
    void Start();
};

extern int g_PbwArchiveCount;
extern PbwArchive g_PbwArchives[16];
extern PbwLzssTreeNode g_PbwLzssTree[4098];

void __stdcall PbwEnqueueBgmCommand(int cmd, int arg, const char *label);
void PbwSetBgmVol();
__time64_t PbwGetTime64(__time64_t *t);
void PbwDeleteArchiveEntries(PbwArchiveEntry *entries);
bool PbwOpenArchiveSlot(const char *path);

void PbwLzssContractNode(int oldNode, int newNode);
int PbwLzssFindNextNode(int node);
void PbwLzssDeleteNode(int node);
void PbwLzssReplaceNode(int oldNode, int newNode);

unsigned char *__fastcall PbwReadFile(const char *name);
unsigned int __fastcall PbwGetFileSize(const char *name);
unsigned int __stdcall PbwThreadProc(void *arg);

} // namespace th128
