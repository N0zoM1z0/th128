#pragma once

namespace Pbg
{
// One 16-byte archive directory entry. The directory is allocated with
// new[] so the vector iterators call these out-of-line members.
struct ArchiveEntry
{
    char *name;
    unsigned int offset;
    unsigned int size;
    unsigned int reserved;

    ArchiveEntry();
    ~ArchiveEntry();
};
}
