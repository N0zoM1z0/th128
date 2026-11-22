#include "PbgFile.hpp"

#include <stdlib.h>
#include <string.h>

namespace Pbg
{
MemFile::~MemFile()
{
    Close();
}

void MemFile::Close()
{
    if (data != NULL)
    {
        free(data);
        data = NULL;
    }
    data = NULL;
    size = 0;
    cursor = NULL;
}

unsigned int MemFile::Read(void *destination, unsigned int length)
{
    unsigned int remaining = data + size - cursor;
    if (remaining >= length)
    {
        memcpy(destination, cursor, length);
        cursor += length;
        return length;
    }
    if (remaining != 0)
    {
        memcpy(destination, cursor, remaining);
        cursor += remaining;
        return remaining;
    }
    return 0;
}

unsigned char MemFile::Write(const void *, unsigned int)
{
    return 0;
}

unsigned int MemFile::Tell()
{
    return cursor - data;
}

unsigned int MemFile::GetSize()
{
    return size;
}

unsigned char MemFile::Seek(long offset, unsigned int origin)
{
    unsigned char *base;
    switch (origin)
    {
    case FILE_BEGIN:
        if (offset < 0 || (unsigned int)offset >= size)
        {
            return 0;
        }
        cursor = data + offset;
        return 1;
    case FILE_CURRENT:
        if (data + size - cursor <= offset)
        {
            return 0;
        }
        base = cursor;
        break;
    case FILE_END:
    {
        if (offset > 0)
        {
            return 0;
        }
        unsigned int distance = offset;
        if (offset < 0)
        {
            distance = -offset;
        }
        if (distance >= size)
        {
            return 0;
        }
        base = data + size;
        break;
    }
    default:
        return 0;
    }
    cursor = base + offset;
    return 1;
}
}
