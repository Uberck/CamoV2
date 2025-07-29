#include "Camo.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

// Snaps a window to the bottom left of the work area
void SnapWindowBottomLeft(HWND hwnd, int winWidth, int winHeight) {
    if (!hwnd) return;
    RECT rcWork;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    int x = rcWork.left;
    int y = rcWork.bottom - winHeight;
    SetWindowPos(hwnd, nullptr, x, y, winWidth, winHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
    SetForegroundWindow(hwnd);
}

// Enables or disables dark mode for the window's title bar (Windows 10 1809+)
void SetDarkTitleBar(HWND hwnd, bool dark)
{
    BOOL useDark = dark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &useDark, sizeof(useDark));
}

// Applies a dark background to the window's menu bar
void SetMenuDark(HWND hwnd)
{
    HMENU hMenu = GetMenu(hwnd);
    MENUINFO mi = { sizeof(mi) };
    mi.fMask = MIM_BACKGROUND;
    mi.hbrBack = CreateSolidBrush(RGB(32,32,32));
    SetMenuInfo(hMenu, &mi);
    DrawMenuBar(hwnd);
}