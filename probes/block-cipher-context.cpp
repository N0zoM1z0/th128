// Non-runnable compiler context shaped like the original call sites: the
// THA1 header (constant key and sizes), the directory (variable size) and an
// entry (key and parameters from a table), so VC10 chooses the private
// convention the original uses (key in AL, everything else on the stack).
#include "../src/BlockCipher.hpp"

struct CipherParameters
{
    unsigned char key;
    unsigned char step;
    int block;
    int limit;
};

extern "C" unsigned char *__stdcall BlockCipherProbe(unsigned char *header, unsigned char *listing,
                                                     int listingSize, unsigned char *entry,
                                                     int entrySize, const CipherParameters *table)
{
    DecryptBlocks(header, 0x10, 0x1B, 0x37, 0x10, 0x10);
    DecryptBlocks(listing, listingSize, 0x3E, 0x9B, 0x80, listingSize);
    const CipherParameters *p = &table[entrySize & 7];
    return DecryptBlocks(entry, entrySize, p->key, p->step, p->block, p->limit);
}
