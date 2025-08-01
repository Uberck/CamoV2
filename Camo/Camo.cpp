// Christos Kokkalis 2025
//

#include "framework.h"
#include "Camo.h"
#include <array>

// Entry point for the app.
// Initializes the main window and runs the message loop.

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Set the application window title
    wcscpy_s(szTitle, MAX_LOADSTRING, L"Camo by Christos");
    LoadStringW(hInstance, IDC_CAMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Create and show the main window
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    // Load keyboard accelerators
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CAMO));
    MSG msg;

    // Main message loop
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}