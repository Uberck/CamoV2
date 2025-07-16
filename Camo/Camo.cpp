// Camo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Camo.h"
#include <windows.h>
#include <shellapi.h>
#include <string>

#define MAX_LOADSTRING 100
#define ID_BUTTON_CONFIRM 1001

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hButton = nullptr;                         // <-- Add this line
HANDLE hScriptProcess = nullptr;                // Add this global variable to store the process handle

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CAMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CAMO));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CAMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CAMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(
        szWindowClass,
        szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Non-resizable
        CW_USEDEFAULT, CW_USEDEFAULT, // x, y position
        320, 180,                    // width, height
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Get the client area size
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    // Button size
    const int buttonWidth = 100;
    const int buttonHeight = 30;

    // Calculate centered position
    int x = (rcClient.right - rcClient.left - buttonWidth) / 2;
    int y = (rcClient.bottom - rcClient.top - buttonHeight) / 2;

    hButton = CreateWindowW(
        L"BUTTON",
        L"Press Me",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x, y, buttonWidth, buttonHeight,
        hWnd,
        (HMENU)ID_BUTTON_CONFIRM,
        hInstance,
        nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_BUTTON_CONFIRM:
                {
                    wchar_t currentText[32];
                    GetWindowTextW(hButton, currentText, 32);

                    if (wcscmp(currentText, L"Press Me") == 0)
                    {
                        // Build the command line for PowerShell
                        std::wstring command = L"powershell.exe -ExecutionPolicy Bypass -File \"C:\\bin\\scripts\\test2.ps1\"";

                        STARTUPINFOW si = { 0 };
                        si.cb = sizeof(si);
                        PROCESS_INFORMATION pi = { 0 };

                        BOOL success = CreateProcessW(
                            nullptr,
                            &command[0],
                            nullptr,
                            nullptr,
                            FALSE,
                            CREATE_NO_WINDOW,
                            nullptr,
                            nullptr,
                            &si,
                            &pi
                        );

                        if (success) {
                            hScriptProcess = pi.hProcess; // Save process handle for later termination
                            CloseHandle(pi.hThread);
                            SetWindowTextW(hButton, L"Stop");

                            // Pin window to bottom right
                            RECT rcWork;
                            SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);

                            int winWidth = 200;  // Your window width
                            int winHeight = 150; // Your window height

                            int x = rcWork.right - winWidth;
                            int y = rcWork.bottom - winHeight;

                            SetWindowPos(hWnd, nullptr, x, y, winWidth, winHeight, SWP_NOZORDER | SWP_NOACTIVATE);
                        } else {
                            MessageBoxW(hWnd, L"Failed to launch script.", L"Error", MB_OK | MB_ICONERROR);
                        }
                    }
                    else if (wcscmp(currentText, L"Stop") == 0)
                    {
                        if (hScriptProcess)
                        {
                            TerminateProcess(hScriptProcess, 0);
                            CloseHandle(hScriptProcess);
                            hScriptProcess = nullptr;
                        }
                        SetWindowTextW(hButton, L"Press Me");
                    }
                }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_SIZE:
        {
            const int buttonWidth = 100;
            const int buttonHeight = 30;
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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
