#include "ThreadInf.hpp"

#include <process.h>

extern int g_ThreadFlag4B4D8C;

ThreadInf::ThreadInf()
{
    handle = NULL;
    threadId = 0;
    stopRequested = 0;
    running = 0;
}

ThreadInf::~ThreadInf()
{
    Stop();
}

void ThreadInf::Stop()
{
    if (handle != NULL)
    {
        stopRequested = 1;
        running = 0;
        while (WaitForSingleObject(handle, 200) == WAIT_TIMEOUT)
        {
            stopRequested = 1;
            running = 0;
            Sleep(1);
        }
        CloseHandle(handle);
        handle = NULL;
        startEntry = NULL;
    }
}

void ThreadInf::Start(ThreadStartRoutine startRoutine, void *arg)
{
    Stop();
    startEntry = startRoutine;
    running = 1;
    stopRequested = 0;
    handle = (HANDLE)_beginthreadex(NULL, 0, startRoutine, arg, 0, &threadId);
}

void ThreadInf::StartSuspended(ThreadStartRoutine startRoutine, void *arg)
{
    Stop();
    startEntry = startRoutine;
    running = 1;
    stopRequested = 0;
    handle = (HANDLE)_beginthreadex(NULL, 0, startRoutine, arg, CREATE_SUSPENDED, &threadId);
}

void ThreadInf::Resume()
{
    if (handle != NULL)
    {
        ResumeThread(handle);
    }
}

void __cdecl SetThreadGlobalFlag4B4D8C()
{
    g_ThreadFlag4B4D8C = 1;
}

void __stdcall SetThreadFlagBit2(unsigned int *flags)
{
    *flags |= 2u;
}
