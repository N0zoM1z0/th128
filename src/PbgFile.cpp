#include "PbgFile.hpp"

#include <stdlib.h>
#include <string.h>

namespace Pbg
{
unsigned char File::Open(const char *filename, const char *mode)
{
    int append = 0;
    DWORD creation = 0;
    char path[MAX_PATH];

    Close();
    const char *cursor = mode;
    for (; *cursor != '\0'; ++cursor)
    {
        if (*cursor == 'r')
        {
            access = GENERIC_READ;
            creation = OPEN_EXISTING;
            break;
        }
        if (*cursor == 'w')
        {
            DeleteFileA(filename);
            creation = CREATE_ALWAYS;
            access = GENERIC_WRITE;
            break;
        }
        if (*cursor == 'a')
        {
            append = 1;
            creation = OPEN_ALWAYS;
            access = GENERIC_WRITE;
            break;
        }
    }
    if (*cursor == '\0')
    {
        return 0;
    }

    // Relative names resolve against the executable's directory.
    if (strchr(filename, ':') != NULL)
    {
        strcpy(path, filename);
    }
    else
    {
        GetModuleFileNameA(NULL, path, MAX_PATH);
        char *slash = strrchr(path, '\\');
        if (slash == NULL)
        {
            path[0] = '\0';
        }
        slash[1] = '\0';
        strcat(path, filename);
    }

    handle = CreateFileA(path, access, FILE_SHARE_READ, NULL, creation,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    if (append)
    {
        SetFilePointer(handle, 0, NULL, FILE_END);
    }
    return 1;
}

File::~File()
{
    Close();
}

void File::Close()
{
    if (handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
        access = 0;
    }
}

unsigned int File::Read(void *destination, unsigned int length)
{
    DWORD bytesRead = 0;
    if (access != GENERIC_READ)
    {
        return 0;
    }
    ReadFile(handle, destination, length, &bytesRead, NULL);
    return bytesRead;
}

unsigned char File::Write(const void *source, unsigned int length)
{
    DWORD bytesWritten = 0;
    if (access != GENERIC_WRITE)
    {
        return 0;
    }
    WriteFile(handle, source, length, &bytesWritten, NULL);
    return length == bytesWritten;
}

unsigned int File::Tell()
{
    if (handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return SetFilePointer(handle, 0, NULL, FILE_CURRENT);
}

unsigned int File::GetSize()
{
    if (handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return GetFileSize(handle, NULL);
}

unsigned char File::Seek(long offset, unsigned int origin)
{
    if (handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    SetFilePointer(handle, offset, NULL, origin);
    return 1;
}

void *File::ReadWhole(unsigned int maximumSize)
{
    if (access != GENERIC_READ)
    {
        return NULL;
    }
    unsigned int size = GetSize();
    if (size > maximumSize)
    {
        return NULL;
    }
    void *buffer = malloc(size);
    if (buffer == NULL)
    {
        return NULL;
    }
    unsigned int position = Tell();
    if (!Seek(position, FILE_BEGIN))
    {
        return NULL;
    }
    if (Read(buffer, size) == 0)
    {
        free(buffer);
        return NULL;
    }
    Seek(position, FILE_BEGIN);
    return buffer;
}
}
