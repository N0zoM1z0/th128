// Non-runnable compiler context: the registry is used through its vtable.
#include "../src/EclResource.hpp"

extern "C" int __stdcall EclResourceProbe(SptResourceInf *registry, void *file)
{
    return registry->Load(file);
}
