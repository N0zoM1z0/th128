// Non-runnable compiler context: the directory's new[]/delete[] take the
// entry constructor and destructor addresses, as in the original.
#include "../src/PbgArchive.hpp"

extern "C" void __stdcall PbgArchiveProbe(Pbg::ArchiveEntry **a, Pbg::ArchiveEntry **b, int count)
{
    Pbg::Archive *archives = new Pbg::Archive[count];
    archives[0].Close();
    archives[count - 1].Close();
    delete[] archives;
    *a = new Pbg::ArchiveEntry[count];
    *b = new Pbg::ArchiveEntry[count + 1];
    delete[] *a;
    delete[] *b;
}
