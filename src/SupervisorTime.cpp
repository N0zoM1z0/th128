#include "Supervisor.hpp"

#include <mmsystem.h>

extern LARGE_INTEGER g_QpcFrequency; // 0x004D3988
extern LARGE_INTEGER g_QpcStart;     // 0x004D3990
extern double g_TimeOrigin;          // 0x004D59C8

double __cdecl GetElapsedSeconds()
{
    double elapsed;
    LARGE_INTEGER now;
    g_Supervisor.EnterCriticalSection(5);
    if (g_QpcFrequency.QuadPart != 0)
    {
        QueryPerformanceCounter(&now);
        elapsed = (double)(now.QuadPart - g_QpcStart.QuadPart) / (double)g_QpcFrequency.QuadPart;
        if (g_TimeOrigin > elapsed)
        {
            g_TimeOrigin = elapsed;
        }
        g_Supervisor.LeaveCriticalSection(5);
        return elapsed - g_TimeOrigin;
    }
    double nowMs = (double)timeGetTime();
    if (g_TimeOrigin > nowMs)
    {
        g_TimeOrigin = nowMs;
    }
    double result = (nowMs - g_TimeOrigin * 1000.0) / 1000.0;
    g_Supervisor.LeaveCriticalSection(5);
    return result;
}
