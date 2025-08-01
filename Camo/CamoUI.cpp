#include "Camo.h"
#include <string>
#include <uxtheme.h>

// -----------------------------------------------------------------------------
// Global variable definitions
// These are shared across the application and declared as extern in Camo.h.
// -----------------------------------------------------------------------------
WCHAR szTitle[MAX_LOADSTRING] = { 0 };         // Window title string
WCHAR szWindowClass[MAX_LOADSTRING] = { 0 };   // Window class name string
HWND hButton = nullptr;                        // Handle to the main button control
HANDLE hScriptProcess = nullptr;               // Handle to the launched PowerShell process
HANDLE hNotepadProcess = nullptr;              // Handle to the launched Notepad process
HINSTANCE hInst = nullptr;                     // Application instance handle
bool g_isDarkMode = false;                     // Tracks dark mode state
std::wstring g_scriptTempPath;

// -----------------------------------------------------------------------------
// MyRegisterClass
// Registers the main window class with Windows. This sets up the window's style,
// icon, cursor, background color, menu, and associates the window procedure.
// -----------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;                  // Redraw on horizontal/vertical resize
    wcex.lpfnWndProc = WndProc;                            // Window procedure function
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CAMO));      // Main icon
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);                   // Default cursor
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);                 // Window background color
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CAMO);                  // Menu resource
    wcex.lpszClassName = szWindowClass;                              // Window class name
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)); // Small icon
    return RegisterClassExW(&wcex);
}

// -----------------------------------------------------------------------------
// InitInstance
// Creates and initializes the main application window and its controls.
// Sets up the main button in the center of the window.
// -----------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle for later use

    // Create the main application window
    HWND hWnd = CreateWindowW(
        szWindowClass, szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Window style
        CW_USEDEFAULT, CW_USEDEFAULT,                             // Default position
        WINDOW_WIDTH, WINDOW_HEIGHT,                              // Window size
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    // Center the button in the client area
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    int x = (rcClient.right - rcClient.left - BUTTON_WIDTH) / 2;
    int y = (rcClient.bottom - rcClient.top - BUTTON_HEIGHT) / 2;

    // Create the main button control
    hButton = CreateWindowW(
        L"BUTTON", L"Press Me",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x, y, BUTTON_WIDTH, BUTTON_HEIGHT,
        hWnd, (HMENU)ID_BUTTON_CONFIRM, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

// -----------------------------------------------------------------------------
// WndProc
// Main window procedure. Handles messages sent to the window such as commands,
// painting, resizing, and destruction. This is the core of the application's UI logic.
// -----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        // Handle menu and button commands
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            // Show the About dialog
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            // Exit the application
            DestroyWindow(hWnd);
            break;
        case ID_BUTTON_CONFIRM:
        {
            wchar_t currentText[32];
            GetWindowTextW(hButton, currentText, 32);

            std::wstring text(currentText);
            if (text == L"Press Me")
            {
                if (hScriptProcess) {
                    MessageBoxW(hWnd, L"PowerShell script is already running.", L"Info", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                TerminateAllNotepadInstances();
                SetWindowTextW(hButton, L"Stop");
                hScriptProcess = nullptr;
                hNotepadProcess = nullptr;

                // Extract embedded PowerShell script to temp file
                g_scriptTempPath = ExtractScriptToTempFile();
                if (g_scriptTempPath.empty()) {
                    MessageBoxW(hWnd, L"Failed to extract PowerShell script.", L"Error", MB_OK | MB_ICONERROR);
                    break;
                }

                // Build PowerShell command
                std::wstring command = L"powershell.exe -ExecutionPolicy Bypass -File \"" + g_scriptTempPath + L"\"";
                STARTUPINFOW si = { 0 };
                si.cb = sizeof(si);
                PROCESS_INFORMATION pi = { 0 };

                BOOL success = CreateProcessW(
                    nullptr, &command[0], nullptr, nullptr, FALSE,
                    CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi);

                if (success) {
                    hScriptProcess = pi.hProcess;
                    CloseHandle(pi.hThread);
                    SetWindowTextW(hButton, L"Stop");

                    // Snap main window to bottom right
                    RECT rcWork;
                    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
                    int winWidth = 200, winHeight = 150;
                    if (winWidth > (rcWork.right - rcWork.left)) winWidth = rcWork.right - rcWork.left;
                    if (winHeight > (rcWork.bottom - rcWork.top)) winHeight = rcWork.bottom - rcWork.top;
                    int x = rcWork.right - winWidth;
                    int y = rcWork.bottom - winHeight;
                    if (hWnd) SetWindowPos(hWnd, nullptr, x, y, winWidth, winHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

                    // Launch Notepad and snap to bottom left
                    wchar_t notepadCmd[] = L"notepad.exe /A";
                    if (!LaunchAndSnap(notepadCmd, 400, 300)) {
                        hNotepadProcess = nullptr;
                    }

                    // Snap PowerShell window to bottom left
                    Sleep(400);
                    HWND hwndPS = FindMainWindow(pi.dwProcessId);
                    if (hwndPS) {
                        SnapWindowBottomLeft(hwndPS, 600, 400);
                    }
                } else {
                    MessageBoxW(hWnd, L"Failed to launch PowerShell.", L"Error", MB_OK | MB_ICONERROR);
                }
            }
            else if (text == L"Stop")
            {
                // Stop PowerShell and Notepad processes
                if (hScriptProcess)
                {
                    TerminateProcess(hScriptProcess, 0);
                    CloseHandle(hScriptProcess);
                    hScriptProcess = nullptr;
                }
                TerminateAllNotepadInstances();
                SetWindowTextW(hButton, L"Press Me");
            }
        }
        break;
        case IDM_TOGGLEDARK:
            // Toggle dark mode for the UI
            g_isDarkMode = !g_isDarkMode;
            InvalidateRect(hWnd, nullptr, TRUE);
            if (hButton) {
                SetWindowTheme(hButton, g_isDarkMode ? L"" : nullptr, g_isDarkMode ? L"" : nullptr);
                InvalidateRect(hButton, nullptr, TRUE);
            }
            SetDarkTitleBar(hWnd, g_isDarkMode);
            SetMenuDark(hWnd);
            break;
        default:
            // Pass unhandled commands to default window procedure
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
    {
        // Re-center the button when the window is resized
        const int buttonWidth = BUTTON_WIDTH;
        const int buttonHeight = BUTTON_HEIGHT;
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        int x = (rcClient.right - rcClient.left - buttonWidth) / 2;
        int y = (rcClient.bottom - rcClient.top - buttonHeight) / 2;
        if (hButton)
        {
            MoveWindow(hButton, x, y, buttonWidth, buttonHeight, TRUE);
        }
    }
    break;
    case WM_PAINT:
    {
        // Paint the window background according to dark mode state
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        COLORREF bgColor = g_isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
        HBRUSH hBrush = CreateSolidBrush(bgColor);
        FillRect(hdc, &ps.rcPaint, hBrush);
        DeleteObject(hBrush);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CTLCOLORBTN:
    {
        // Paint the button background and text according to dark mode state
        if (g_isDarkMode) {
            HDC hdcBtn = (HDC)wParam;
            SetBkColor(hdcBtn, RGB(32, 32, 32));
            SetTextColor(hdcBtn, RGB(255, 255, 255));
            static HBRUSH hDarkBrush = CreateSolidBrush(RGB(32, 32, 32));
            return (INT_PTR)hDarkBrush;
        }
    }
    break;
    case WM_DESTROY:
        // Clean up the temporary PowerShell script file
        if (!g_scriptTempPath.empty()) {
            DeleteFileW(g_scriptTempPath.c_str());
            g_scriptTempPath.clear();
        }
        PostQuitMessage(0);
        break;
    default:
        // Default message handling
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// About
// Dialog procedure for the About box. Handles dialog initialization and button events.
// -----------------------------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        // Initialize the About dialog
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        // Handle OK and Cancel button clicks
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}