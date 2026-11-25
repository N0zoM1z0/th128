#include "MenuCursor.hpp"

void MenuCursor::Push()
{
    savedValues[stackDepth] = value;
    savedCounts[stackDepth] = count;
    ++stackDepth;
    disabledCount = 0;
    if (stackDepth >= 16)
    {
        stackDepth = 15;
    }
}

void MenuCursor::Pop()
{
    --stackDepth;
    if (stackDepth < 0)
    {
        stackDepth = 0;
    }
    value = savedValues[stackDepth];
    count = savedCounts[stackDepth];
    disabledCount = 0;
}

// Moves by `delta`, wrapping or clamping at the ends and skipping disabled
// entries; returns the new value.
int MenuCursor::Move(int delta)
{
    if (count > 0)
    {
        int disabledEntries = disabledCount;
    retry:
        value += delta;
        while (value >= count)
        {
            if (wraps)
            {
                value -= count;
            }
            else
            {
                value = count - 1;
            }
        }
        while (value < 0)
        {
            if (wraps)
            {
                value += count;
            }
            else
            {
                value = 0;
            }
        }
        for (int i = 0; i < disabledEntries; ++i)
        {
            if (disabled[i] == value)
            {
                goto retry;
            }
        }
    }
    return value;
}
