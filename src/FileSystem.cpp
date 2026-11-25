#include "FileSystem.hpp"

#include <stdlib.h>
#include <string.h>

#include "DebugLog.hpp"
#include "PbgArchive.hpp"
#include "Supervisor.hpp"

extern Pbg::Archive g_MainArchive;

namespace FileSystem
{
unsigned char *__stdcall OpenPath(const char *path, int *outSize, int isExternal)
{
    g_Supervisor.EnterCriticalSection(2);
    if (isExternal == 0)
    {
        const char *name = strrchr(path, '\\');
        if (name == NULL)
        {
            name = path;
        }
        else
        {
            ++name;
        }
        const char *slash = strrchr(name, '/');
        if (slash == NULL)
        {
            slash = path;
        }
        else
        {
            ++slash;
        }
        int entrySize = 0;
        Pbg::ArchiveEntry *entry = g_MainArchive.entries;
        if (entry != NULL)
        {
            int remaining = g_MainArchive.count;
            while (remaining > 0)
            {
                if (_stricmp(slash, entry->name) == 0)
                {
                    entrySize = entry->size;
                    break;
                }
                --remaining;
                ++entry;
            }
        }
        if (outSize != NULL)
        {
            *outSize = entrySize;
        }
        if (entrySize == 0)
        {
            g_Supervisor.LeaveCriticalSection(2);
            return NULL;
        }
        DebugPrint("%s Decode ... \r\n", slash);
        unsigned char *buffer = (unsigned char *)malloc(entrySize);
        if (buffer == NULL)
        {
            g_Supervisor.LeaveCriticalSection(2);
            return NULL;
        }
        g_MainArchive.ReadEntry(slash, buffer);
        g_Supervisor.LeaveCriticalSection(2);
        return buffer;
    }

    DebugPrint("%s Load ... \r\n", path);
    HANDLE handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        DebugPrint("error : %s is not found.\r\n", path);
        g_Supervisor.LeaveCriticalSection(2);
        return NULL;
    }
    DWORD fileSize = GetFileSize(handle, NULL);
    unsigned char *buffer = (unsigned char *)malloc(fileSize);
    if (buffer == NULL)
    {
        DebugPrint("error : %s allocation error.\r\n", path);
        CloseHandle(handle);
        g_Supervisor.LeaveCriticalSection(2);
        return NULL;
    }
    DWORD bytesRead = fileSize;
    ReadFile(handle, buffer, fileSize, &bytesRead, NULL);
    if (outSize != NULL)
    {
        *outSize = (int)bytesRead;
    }
    CloseHandle(handle);
    g_Supervisor.LeaveCriticalSection(2);
    return buffer;
}

int __stdcall FileExists(const char *path)
{
    g_Supervisor.EnterCriticalSection(2);
    HANDLE handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle);
        g_Supervisor.LeaveCriticalSection(2);
        return 1;
    }
    g_Supervisor.LeaveCriticalSection(2);
    return 0;
}

int __stdcall WriteDataToFile(const char *path, const void *data, unsigned int size)
{
    g_Supervisor.EnterCriticalSection(2);
    HANDLE handle = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        LPSTR message;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPSTR)&message, 0, NULL);
        DebugPrint("error : %s write error %s\r\n", path, message);
        LocalFree(message);
        g_Supervisor.LeaveCriticalSection(2);
        return -1;
    }
    DWORD written;
    WriteFile(handle, data, size, &written, NULL);
    if (size != written)
    {
        CloseHandle(handle);
        DebugPrint("error : %s write error\r\n", path);
        g_Supervisor.LeaveCriticalSection(2);
        return -2;
    }
    CloseHandle(handle);
    DebugPrint("%s write ...\r\n", path);
    g_Supervisor.LeaveCriticalSection(2);
    return 0;
}

int __stdcall OpenWriteStream(const char *path)
{
    g_Supervisor.EnterCriticalSection(2);
    g_StreamFileHandle = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                     CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (g_StreamFileHandle == INVALID_HANDLE_VALUE)
    {
        LPSTR message;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPSTR)&message, 0, NULL);
        DebugPrint("error : %s write error %s\r\n", path, message);
        LocalFree(message);
        g_Supervisor.LeaveCriticalSection(2);
        return -1;
    }
    DebugPrint("%s open ...\r\n", path);
    return 0;
}

int __stdcall OpenReadStream(const char *path)
{
    g_Supervisor.EnterCriticalSection(2);
    g_StreamFileHandle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (g_StreamFileHandle == INVALID_HANDLE_VALUE)
    {
        LPSTR message;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPSTR)&message, 0, NULL);
        DebugPrint("error : %s write error %s\r\n", path, message);
        LocalFree(message);
        g_Supervisor.LeaveCriticalSection(2);
        return -1;
    }
    DebugPrint("%s open ...\r\n", path);
    return 0;
}

int __stdcall WriteStream(const void *data, unsigned int size)
{
    if (g_StreamFileHandle == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    DWORD written;
    WriteFile(g_StreamFileHandle, data, size, &written, NULL);
    if (size != written)
    {
        CloseHandle(g_StreamFileHandle);
        g_Supervisor.LeaveCriticalSection(2);
        return -2;
    }
    return 0;
}

unsigned char *__stdcall ReadStream(unsigned int size)
{
    if (g_StreamFileHandle == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }
    unsigned char *buffer = (unsigned char *)malloc(size);
    if (buffer == NULL)
    {
        CloseHandle(g_StreamFileHandle);
        return NULL;
    }
    DWORD bytesRead;
    ReadFile(g_StreamFileHandle, buffer, size, &bytesRead, NULL);
    return buffer;
}

int __stdcall CloseStream()
{
    if (g_StreamFileHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_StreamFileHandle);
        g_Supervisor.LeaveCriticalSection(2);
    }
    return 0;
}
}
