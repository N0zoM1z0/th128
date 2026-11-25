// Non-runnable compiler context: independent calls let VC10 choose the
// private convention (cursor in EAX in the original).
#include "../src/MenuCursor.hpp"

extern "C" int __stdcall MenuCursorProbe(MenuCursor *a, MenuCursor *b, int delta)
{
    a->Push();
    b->Push();
    int result = a->Move(delta) + b->Move(-1) + a->Move(1);
    a->Pop();
    b->Pop();
    return result;
}
