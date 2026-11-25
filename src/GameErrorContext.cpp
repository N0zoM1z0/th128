#include "GameErrorContext.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "Supervisor.hpp"

const char *__cdecl GameErrorContext::Log(const char *fmt, ...)
{
    char local[0x2000];
    va_list args;

    g_Supervisor.EnterCriticalSection(3);
    va_start(args, fmt);
    vsprintf(local, fmt, args);
    va_end(args);

    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1FFF)
    {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    g_Supervisor.LeaveCriticalSection(3);
    return fmt;
}

const char *__cdecl GameErrorContext::Fatal(const char *fmt, ...)
{
    char local[0x200];
    va_list args;

    g_Supervisor.EnterCriticalSection(3);
    va_start(args, fmt);
    vsprintf(local, fmt, args);
    va_end(args);

    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1FFF)
    {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    fatal = 1;
    g_Supervisor.LeaveCriticalSection(3);
    return fmt;
}
