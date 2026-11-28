#include "../src/PbgWave.hpp"

namespace th128 {

int g_PbwArchiveCount = 0;
PbwArchive g_PbwArchives[16];
PbwLzssTreeNode g_PbwLzssTree[4098];

static volatile int g_Sink = 0;

__declspec(noinline) void PbwAnmVm::Update() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}

__declspec(noinline) PbwArchiveEntry::~PbwArchiveEntry() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}

__declspec(noinline) void PbwArchive::Close() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}

__declspec(noinline) bool PbwArchive::Open(const char *path) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)path + (int)t;
    return g_Sink != 0;
}

__declspec(noinline) bool PbwArchive::ReadHeader(const char *path) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)path + (int)t;
    return g_Sink != 0;
}

__declspec(noinline) void PbwThreadInf::Stop() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}

__declspec(noinline) void __stdcall PbwEnqueueBgmCommand(int cmd, int arg, const char *label) {
    DWORD t = GetTickCount();
    g_Sink += cmd + arg + (int)label + (int)t;
}

__declspec(noinline) void PbwLzssContractNode(int oldNode, int newNode) {
    DWORD t = GetTickCount();
    g_Sink += oldNode + newNode + (int)t;
}

__declspec(noinline) int PbwLzssFindNextNode(int node) {
    DWORD t = GetTickCount();
    g_Sink += node + (int)t;
    return g_Sink;
}

__declspec(noinline) unsigned int __stdcall PbwThreadProc(void *arg) {
    return (unsigned int)arg + g_Sink;
}

} // namespace th128

extern "C" int __stdcall PbgWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        PbwSetBgmVol();
        return g_Sink;
    }
    if (selector == 2) {
        return (int)PbwGetTime64((__time64_t *)arg);
    }
    if (selector == 3) {
        ((PbwAnmVm *)arg)->SetScriptAndUpdate((short)arg);
        return g_Sink;
    }
    if (selector == 4) {
        return ((PbwArchive *)arg)->OpenMem((const char *)(arg + 4)) ? 1 : 0;
    }
    if (selector == 5) {
        PbwDeleteArchiveEntries((PbwArchiveEntry *)arg);
        return g_Sink;
    }
    if (selector == 6) {
        return (int)((PbwArchive *)arg)->GetEntrySize((const char *)(arg + 4));
    }
    if (selector == 7) {
        return (int)((PbwArchive *)arg)->FindEntry((const char *)(arg + 4));
    }
    if (selector == 8) {
        return PbwOpenArchiveSlot((const char *)arg) ? 1 : 0;
    }
    if (selector == 9) {
        PbwLzssDeleteNode(arg);
        return g_Sink;
    }
    if (selector == 10) {
        PbwLzssReplaceNode(arg, arg + 1);
        return g_Sink;
    }
    if (selector == 11) {
        PbwMemFile mf;
        PbwResFileImpl rf;
        return (mf.Open((const char *)arg, "rb") ? 1 : 0) + (rf.Open((const char *)arg, "rb") ? 2 : 0);
    }
    if (selector == 12) {
        ((PbwThreadInf *)arg)->~PbwThreadInf();
        ((PbwThreadInf *)arg)->Start();
        return g_Sink;
    }
    return 0;
}
