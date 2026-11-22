#include <windows.h>

// Provisional names; the addresses are TH128 globals reviewed in the unit.
extern unsigned int g_GameFlags;        // bit 7 requests quit, bit 8 cleared with it
extern unsigned int g_WindowFlags;      // bit 0 windowed; maximizing clears bits 2-3, sets bit 1
extern WPARAM g_AppActiveParam;         // last WM_ACTIVATEAPP wParam
extern int g_AppInactive;               // WM_ACTIVATEAPP lost focus
extern int g_CursorAlwaysVisible;

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATEAPP:
        g_AppActiveParam = wParam;
        g_AppInactive = wParam == 0;
        break;
    case WM_SIZE:
        if (g_WindowFlags & 1)
        {
            switch (wParam)
            {
            case SIZE_MAXIMIZED:
                g_WindowFlags = (g_WindowFlags & ~0xC) | 2;
                break;
            }
        }
        break;
    case WM_CLOSE:
        g_GameFlags = (g_GameFlags & ~0x100) | 0x80;
        return 1;
    case WM_ERASEBKGND:
        return 1;
    case WM_SETCURSOR:
        if (!g_CursorAlwaysVisible)
        {
            if (g_AppInactive)
            {
                SetCursor(LoadCursorA(NULL, IDC_ARROW));
                while (ShowCursor(TRUE) < 0)
                {
                }
            }
            else
            {
                while (ShowCursor(FALSE) >= 0)
                {
                }
                SetCursor(NULL);
            }
        }
        else
        {
            SetCursor(LoadCursorA(NULL, IDC_ARROW));
            ShowCursor(TRUE);
        }
        return 1;
    case WM_SYSCOMMAND:
        switch (wParam & 0xFFF0)
        {
        case SC_MOUSEMENU:
        case SC_KEYMENU:
            return 1;
        }
        break;
    case WM_LBUTTONDOWN:
        SetForegroundWindow(window);
        break;
    }
    return DefWindowProcA(window, message, wParam, lParam);
}
