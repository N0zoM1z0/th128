#include "Laser.hpp"

// Runs the timed commands that are due.
void LaserInfiniteInf::Slot01()
{
    while (commandIndex < 18)
    {
        LaserCommand *command = &commands[commandIndex];
        if (command->type == 0)
        {
            return;
        }
        if (command->immediate == 0 && busy != 0)
        {
            return;
        }
        switch (command->type)
        {
        case 0x80:
            commandValue80 = command->value;
            break;
        case 0x400:
            state = 3;
            break;
        case 0x100000:
            if (command->value != 0)
            {
                mode = 1;
            }
            else
            {
                mode = 0;
            }
            break;
        }
        ++commandIndex;
    }
}
