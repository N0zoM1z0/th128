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
}
