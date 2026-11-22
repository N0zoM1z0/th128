// Non-runnable compiler context: the directory's new[]/delete[] take the
// entry constructor and destructor addresses, as in the original.
#include "../src/PbgArchive.hpp"
#include "../src/DebugLog.hpp"

// Placeholder for the not yet reconstructed header reader (0x0044F9A0): it
// only gives Open a defined LTCG callee that takes its filename on the stack.
bool Pbg::Archive::ReadHeader(const char *filename)
{
    unsigned int header[4];
    if (!file->Open(filename, "r"))
    {
        return false;
    }
    if (file->Read(header, sizeof(header)) == 0)
    {
        return false;
    }
    count = header[3];
    file->Seek(header[1], FILE_BEGIN);
    return file->Read(header, header[2]) != 0;
}

extern "C" void __stdcall PbgArchiveProbe(Pbg::ArchiveEntry **a, Pbg::ArchiveEntry **b, int count)
{
    Pbg::Archive *archives = new Pbg::Archive[count];
    archives[0].Open((const char *)a);
    archives[1].Open((const char *)b);
    archives[0].Close();
    archives[count - 1].Close();
    delete[] archives;
    *a = Pbg::ParseArchiveDirectory((const char *)b, count, 16);
    *b = Pbg::ParseArchiveDirectory((const char *)a, count + 2, 32);
    delete[] *a;
    delete[] *b;
    *a = new Pbg::ArchiveEntry[count];
    *b = new Pbg::ArchiveEntry[count + 1];
    delete[] *a;
    delete[] *b;
}
