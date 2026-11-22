#include <windows.h>
#include <string.h>

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

// Window state at 0x004D59A8; bit 0 is read by WindowProc. Field names are
// provisional.
struct WindowState
{
    unsigned int windowedFlag : 1;
    unsigned int maximized : 1;
    unsigned int mode : 2;          // 0 fullscreen, 1 640x480, 2 960x720, 3 1280x960
    unsigned int refreshFlag : 1;
    unsigned int otherFlags : 27;
    unsigned char unknown04[0x40 - 0x04];
    int field40;
    int field44;
    int field48;
    int field4C;
    int field50;
    int field54;
    int field58;
    int field5C;
    int field60;
    int field64;
    int field68;
    int field6C;
    int field70;
};

extern WindowState g_Window;
extern unsigned char g_ConfigWindowMode;    // 0x004D304D
extern unsigned char g_ConfigFlag304E;      // 0x004D304E
extern unsigned char g_ConfigFrameSkip;     // 0x004D3053
extern int g_ConfigWindowX;                 // 0x004D3058
extern int g_ConfigWindowY;                 // 0x004D305C
extern WPARAM g_AppActiveParam;
extern int g_AppInactive;
extern int g_CursorAlwaysVisible;
extern HWND g_MainWindow;                   // 0x004D3970
extern RECT g_MainWindowRect;               // 0x004D2E78
extern HWND g_DeviceWindow;                 // 0x004D2EC0

int __stdcall CreateGameWindow(HINSTANCE instance)
{
    WNDCLASSA windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    windowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    windowClass.hInstance = instance;
    windowClass.lpfnWndProc = WindowProc;
    g_AppActiveParam = 1;
    g_AppInactive = 0;
    windowClass.lpszClassName = "BASE";
    RegisterClassA(&windowClass);

    g_Window.mode = g_ConfigWindowMode;
    g_CursorAlwaysVisible = g_Window.mode != 0;
    if (g_ConfigFlag304E == 0 && g_ConfigFrameSkip == 2)
    {
        g_Window.refreshFlag = 1;
    }
    else
    {
        g_Window.refreshFlag = 0;
    }
    g_Window.field44 = 15;
    g_Window.field48 = 15;
    g_Window.field4C = 0;
    g_Window.field50 = 12;
    g_Window.field54 = 12;
    g_Window.field58 = 0;
    g_Window.field5C = 12;
    g_Window.field60 = 12;
    g_Window.field64 = 0;
    g_Window.field68 = 8;
    g_Window.field6C = 8;
    g_Window.field70 = 0;
    g_Window.field40 = 0;

    HWND window;
    if (!g_CursorAlwaysVisible)
    {
        window = CreateWindowExA(0, "BASE", "\x97" "d\x90\xb8\x91\xe5\x90\xed\x91\x88\x81@\x81` \x93\x8c\x95\xfb\x8eO\x8c\x8e\x90\xb8. ver 1.00a", WS_POPUP | WS_VISIBLE,
                                 0, 0, 640, 480, NULL, NULL, instance, NULL);
    }
    else
    {
        int width;
        int height;
        if (g_Window.mode == 3)
        {
            width = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 1280;
            height = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 960;
        }
        else if (g_Window.mode == 2)
        {
            width = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 960;
            height = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 720;
        }
        else
        {
            width = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 640;
            height = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 480;
        }
        height += GetSystemMetrics(SM_CYCAPTION);
        window = CreateWindowExA(0, "BASE", "\x97" "d\x90\xb8\x91\xe5\x90\xed\x91\x88\x81@\x81` \x93\x8c\x95\xfb\x8eO\x8c\x8e\x90\xb8. ver 1.00a",
                                 WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
                                 g_ConfigWindowX, g_ConfigWindowY, width, height,
                                 NULL, NULL, instance, NULL);
    }
    g_MainWindow = window;
    GetWindowRect(window, &g_MainWindowRect);
    g_DeviceWindow = g_MainWindow;
    if (g_MainWindow == NULL)
    {
        return 1;
    }
    SendMessageA(g_MainWindow, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    Sleep(16);
    SendMessageA(g_MainWindow, WM_SYSCOMMAND, SC_RESTORE, 0);
    return 0;
}
