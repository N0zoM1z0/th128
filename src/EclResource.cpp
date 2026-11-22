#include "EclResource.hpp"

#include <stdlib.h>
#include <string.h>

// Registers a loaded script file and merges its subs into the name-sorted
// sub table; returns the file index, or -1 for an invalid file.
int SptResourceInf::Load(void *file)
{
    files[fileCount] = (ScptHeader *)file;
    ScptHeader **slot = &files[fileCount];
    if ((*slot)->magic != 0x54504353)
    {
        *slot = NULL;
        return -1;
    }
    if ((*slot)->version != 1)
    {
        *slot = NULL;
        return -1;
    }
    ScptHeader *header = *slot;
    const unsigned int *offsets =
        (const unsigned int *)((const char *)header + header->includeLength + sizeof(ScptHeader));
    const char *name = (const char *)(offsets + header->subCount);
    EclSub *old = subs;
    subCount += header->subCount;
    if (old == NULL)
    {
        subs = (EclSub *)malloc(subCount * sizeof(EclSub));
        for (int i = 0; i < subCount; ++i)
        {
            subs[i].code = (char *)files[fileCount] + *offsets;
            subs[i].name = name;
            ++offsets;
            name += strlen(name) + 1;
        }
    }
    else
    {
        subs = (EclSub *)malloc(subCount * sizeof(EclSub));
        int known = subCount - (*slot)->subCount;
        memcpy(subs, old, known * sizeof(EclSub));
        if (old != NULL)
        {
            free(old);
        }
        for (int j = 0; j < files[fileCount]->subCount; ++j)
        {
            int position = 0;
            for (EclSub *sub = subs; position < known; ++sub, ++position)
            {
                if (strcmp(name, sub->name) <= 0)
                {
                    break;
                }
            }
            for (int k = subCount - 1; k > position; --k)
            {
                subs[k] = subs[k - 1];
            }
            subs[position].code = (char *)files[fileCount] + *offsets;
            subs[position].name = name;
            name += strlen(name) + 1;
            ++known;
            ++offsets;
        }
    }
    int index = fileCount++;
    if (files[index]->includeLength != 0)
    {
        LoadIncludes((char *)files[index] + sizeof(ScptHeader));
    }
    return index;
}

int SptResourceInf::LoadIncludes(void *)
{
    return 0;
}
