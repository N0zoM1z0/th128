#include "PbgArchive.hpp"

#include "DebugLog.hpp"

#include <new>
#include <stdlib.h>
#include <string.h>

namespace Pbg
{
ArchiveEntry::ArchiveEntry() : name(NULL)
{
}

ArchiveEntry::~ArchiveEntry()
{
    if (name != NULL)
    {
        free(name);
        name = NULL;
    }
}

bool Archive::Open(const char *filename)
{
    Close();
    DebugPrint("info : %s open arcfile\r\n", filename);
    file = new File;
    if (file != NULL)
    {
        if (ReadHeader(filename))
        {
            char *copy = (char *)malloc(strlen(filename) + 1);
            if (copy != NULL)
            {
                strcpy(copy, filename);
            }
            name = copy;
            if (name != NULL)
            {
                file->Open(name, "r");
                return true;
            }
        }
        DebugPrint("info : %s not found\r\n", filename);
        Close();
    }
    return false;
}

Archive::~Archive()
{
    Close();
}

void Archive::Close()
{
    if (name != NULL)
    {
        DebugPrint("info : %s close arcfile\r\n", name);
        if (name != NULL)
        {
            free(name);
            name = NULL;
        }
    }
    name = NULL;
    if (entries != NULL)
    {
        delete[] entries;
    }
    entries = NULL;
    if (file != NULL)
    {
        delete file;
    }
    file = NULL;
    count = 0;
}

ArchiveEntry *__stdcall ParseArchiveDirectory(const char *listing, int count, unsigned int archiveSize)
{
    ArchiveEntry *entries = new ArchiveEntry[count + 1];
    if (entries == NULL)
    {
        return NULL;
    }
    const char *cursor = listing;
    for (int i = 0; i < count; ++i)
    {
        char *copy = (char *)malloc(strlen(cursor) + 1);
        if (copy != NULL)
        {
            strcpy(copy, cursor);
        }
        entries[i].name = copy;
        int length = strlen(cursor) + 1;
        if (length % 4 != 0)
        {
            length += 4 - length % 4;
        }
        cursor += length;
        const unsigned int *fields = (const unsigned int *)cursor;
        entries[i].offset = fields[0];
        entries[i].size = fields[1];
        entries[i].reserved = fields[2];
        cursor += 12;
    }
    entries[count].offset = archiveSize;
    entries[count].size = 0;
    return entries;
}
}
