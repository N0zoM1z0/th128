#include "PbgFile.hpp"

namespace Pbg
{
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
}
