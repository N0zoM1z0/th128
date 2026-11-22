#include "BlockCipher.hpp"

#include <stdlib.h>
#include <string.h>

unsigned char *__stdcall DecryptBlocks(unsigned char *data, int size, unsigned char key,
                                       unsigned char step, int block, int limit)
{
    int tail = size % block < block / 4 ? size % block : 0;
    int length = size;
    if (limit <= size)
    {
        length = limit;
    }
    unsigned char *out = data;
    unsigned char *temp = (unsigned char *)malloc(length);
    unsigned char *in = temp;
    if (temp != NULL)
    {
        size -= (size & 1) + tail;
        memcpy(temp, data, length);
        while (size > 0 && limit > 0)
        {
            if (size < block)
            {
                block = size;
            }
            unsigned char *end = out + block;
            unsigned char *p = end - 1;
            for (int i = (block + 1) / 2; i > 0; --i)
            {
                *p = *in ^ key;
                key += step;
                p -= 2;
                ++in;
            }
            p = end - 2;
            for (int i = block / 2; i > 0; --i)
            {
                *p = *in ^ key;
                key += step;
                p -= 2;
                ++in;
            }
            limit -= block;
            size -= block;
            out = end;
        }
        free(temp);
    }
    return data;
}
