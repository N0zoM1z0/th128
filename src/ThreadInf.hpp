#ifndef TH128_THREAD_INF_HPP
#define TH128_THREAD_INF_HPP

#include <windows.h>

typedef unsigned int(__stdcall *ThreadStartRoutine)(void *);

class ThreadInf
{
public:
    ThreadInf();
    virtual ~ThreadInf();

    void Stop();
    void Start(ThreadStartRoutine startRoutine, void *arg);
    void StartSuspended(ThreadStartRoutine startRoutine, void *arg);
    void Resume();

    HANDLE handle;                 // +0x04
    unsigned int threadId;         // +0x08
    int stopRequested;             // +0x0C
    int running;                   // +0x10
    int field14;                   // +0x14
    ThreadStartRoutine startEntry; // +0x18
};

void __cdecl SetThreadGlobalFlag4B4D8C();
void __stdcall SetThreadFlagBit2(unsigned int *flags);

#endif
