#include "AnmVm.hpp"

#include <string.h>

AnmVm::AnmVm()
{
    memset(this, 0, sizeof(AnmVm));
    anmFileIndex = -1;
}
