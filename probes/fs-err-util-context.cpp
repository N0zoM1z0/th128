// Non-runnable VC10 /GL context for FileSystem, GameErrorContext, and EngineUtil.
#include "../src/DebugLog.hpp"
#include "../src/EngineUtil.hpp"
#include "../src/FileSystem.hpp"
#include "../src/GameErrorContext.hpp"
#include "../src/PbgArchive.hpp"
#include "../src/Supervisor.hpp"

Supervisor g_Supervisor;
GameErrorContext g_GameErrorContext;
Pbg::Archive g_MainArchive;
HANDLE g_StreamFileHandle = INVALID_HANDLE_VALUE;

unsigned int *g_ProbeSupFlags = &g_Supervisor.flags;
CRITICAL_SECTION *g_ProbeSupCs = g_Supervisor.criticalSections;
unsigned char *g_ProbeSupDepth = g_Supervisor.lockDepths;

typedef unsigned char *(__stdcall *OpenPathPtr)(const char *, int *, int);
OpenPathPtr g_ProbeOpenPath = FileSystem::OpenPath;

extern "C" void __stdcall FsErrUtilProbe(
    const char *p1, const char *p2, int *outSz, int ext,
    const void *buf, unsigned int sz,
    GameErrorContext *e1, GameErrorContext *e2,
    EngineVec2 *vout, const EngineVec2 *vin, float ang,
    HWND hwnd, char *dst, int maxLen)
{
    DebugPrint("probe");
    FileSystem::OpenPath(p1, outSz, ext);
    FileSystem::OpenPath(p2, 0, 1);
    FileSystem::FileExists(p1);
    FileSystem::FileExists(p2);
    FileSystem::WriteDataToFile(p1, buf, sz);
    FileSystem::WriteDataToFile(p2, buf, sz + 1);
    FileSystem::OpenWriteStream(p1);
    FileSystem::OpenWriteStream(p2);
    FileSystem::OpenReadStream(p1);
    FileSystem::OpenReadStream(p2);
    FileSystem::WriteStream(buf, sz);
    FileSystem::WriteStream(buf, sz + 4);
    FileSystem::ReadStream(sz);
    FileSystem::ReadStream(sz + 4);
    FileSystem::CloseStream();

    e1->Log("%s %d", p1, ext);
    e2->Log("%s", p2);
    e1->Fatal("%s %d", p1, ext);
    e2->Fatal("%s", p2);

    RotateVec2(vout, vin, ang);
    RotateVec2(vout + 1, vin + 1, ang + 1.0f);
    IsWindowMinimized(hwnd);
    GetWindowsVersionCode();
    ReadShiftJisLine(dst, p1, maxLen);
    ReadShiftJisLine(dst + 16, p2, maxLen - 1);
    ChecksumBytes((const unsigned char *)buf, sz);
    ChecksumBytes((const unsigned char *)p1, sz + 1);
    ChecksumDwords((const unsigned int *)buf, (int)sz);
    ChecksumDwords((const unsigned int *)p1, (int)sz + 4);
}
