#include "PbgArchive.hpp"

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
}
