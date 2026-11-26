#include "PopupNotifications.hpp"
#include "CardActivityViews.hpp"
#include "AnmHelpers.hpp"

void PopupNotifications::Show(int kind, int value)
{
    switch (kind)
    {
    case 0:
    {
        AnmHelpers::DeleteVmByIdPtr(&primaryLabel);
        primaryLabel = resource->CreateVm(0, 0x2D, 23, 0);
        int remaining = value;
        int started = 0;
        int divisor = 10000000;
        for (int i = 0; i < 8; ++i)
        {
            AnmHelpers::DeleteVmByIdPtr(&digits[i]);
            digits[i] = CardAsciiResource()->CreateVm(0, i + 9, 23, 0);
            const int digit = remaining / divisor;
            const int rest = remaining % divisor;
            if (digit != 0)
                started = 1;
            AnmVm *vm = g_AnmManager->GetVmById(digits[i]);
            if (vm != 0)
                vm->BindAnmFile(vm->resource, digit + 0xF3);
            if (started == 0)
                AnmHelpers::ClearVisibleBitByIdPtr(&digits[i]);
            else
                AnmHelpers::SetVisibleBitByIdPtr(&digits[i]);
            remaining = rest;
            divisor /= 10;
        }
        goto separators;
    }
    case 1:
        AnmHelpers::DeleteVmByIdPtr(&primaryLabel);
        primaryLabel = resource->CreateVm(0, 0x2E, 23, 0);
        goto marker;
    case 2:
        AnmHelpers::DeleteVmByIdPtr(&secondaryLabel);
        secondaryLabel = resource->CreateVm(0, 0x2F, 23, 0);
        return;
    case 3:
        AnmHelpers::DeleteVmByIdPtr(&secondaryLabel);
        secondaryLabel = resource->CreateVm(0, 0x30, 23, 0);
        return;
    case 4:
        AnmHelpers::DeleteVmByIdPtr(&secondaryLabel);
        secondaryLabel = resource->CreateVm(0, 0x31, 23, 0);
        return;
    case 5:
        return;
    case 6:
        AnmHelpers::DeleteVmByIdPtr(&primaryLabel);
        primaryLabel = resource->CreateVm(0, 0x32, 23, 0);
        return;
    case 7:
    case 8:
    {
        AnmHelpers::DeleteVmByIdPtr(&primaryLabel);
        primaryLabel = resource->CreateVm(0, (kind != 7) + 0xAA, 23, 0);
        int remaining = value;
        int started = 0;
        int divisor = 10000000;
        for (int i = 0; i < 8; ++i)
        {
            AnmHelpers::DeleteVmByIdPtr(&digits[i]);
            digits[i] = CardAsciiResource()->CreateVm(0, i + 9, 23, 0);
            const int digit = remaining / divisor;
            remaining %= divisor;
            if (digit != 0)
                started = 1;
            AnmVm *vm = g_AnmManager->GetVmById(digits[i]);
            if (vm != 0)
                vm->BindAnmFile(vm->resource, digit + 0xF3);
            if (started == 0)
                AnmHelpers::ClearVisibleBitByIdPtr(&digits[i]);
            else
                AnmHelpers::SetVisibleBitByIdPtr(&digits[i]);
            divisor /= 10;
        }
        goto separators;
    }
    case 9:
        AnmHelpers::DeleteVmByIdPtr(&primaryLabel);
        primaryLabel = resource->CreateVm(0, 0xAC, 23, 0);
        goto marker;
    }
    return;

separators:
    AnmHelpers::DeleteVmByIdPtr(&millionSeparator);
    if (value >= 1000000)
    {
        millionSeparator = CardAsciiResource()->CreateVm(0, 17, 23, 0);
        AnmVm *vm = g_AnmManager->GetVmById(millionSeparator);
        if (vm != 0)
            vm->BindAnmFile(vm->resource, 0xFD);
    }
    AnmHelpers::DeleteVmByIdPtr(&thousandSeparator);
    if (value >= 1000)
    {
        thousandSeparator = CardAsciiResource()->CreateVm(0, 18, 23, 0);
        AnmVm *vm = g_AnmManager->GetVmById(thousandSeparator);
        if (vm != 0)
            vm->BindAnmFile(vm->resource, 0xFD);
    }
marker:
    markerActive = 1;
    this->marker = resource->CreateVm(0, 0x50, 23, 0);
}
