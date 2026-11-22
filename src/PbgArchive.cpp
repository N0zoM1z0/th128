#include "PbgArchive.hpp"

#include "DebugLog.hpp"

#include <new>
#include <stdlib.h>

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
