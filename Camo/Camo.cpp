// Christos Kokkalis 2025
//

#include "framework.h"
#include "Camo.h"
#include <array>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>

// Returns the HWND of the first top-level Notepad window, or nullptr if not found
HWND FindNotepadWindow()
{
    HWND hwnd = nullptr;
    DWORD notepadPid = 0;

    // Find notepad.exe process
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return nullptr;

    PROCESSENTRY32W pe = { sizeof(pe) };
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"notepad.exe") == 0) {
                notepadPid = pe.th32ProcessID;
                break;
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
    if (!notepadPid) return nullptr;

    // Find window belonging to notepad.exe
    struct {
        DWORD pid;
        HWND hwnd;
    } data = { notepadPid, nullptr };

    auto EnumWindowsProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        auto* d = reinterpret_cast<decltype(data)*>(lParam);
        if (pid == d->pid && IsWindowVisible(hWnd)) {
            d->hwnd = hWnd;
            return FALSE; // Stop enumeration
        }
        return TRUE;
    };

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}

// Brings Notepad to the foreground and sets keyboard focus
void BringNotepadToForeground()
{
    HWND hwnd = FindNotepadWindow();
    if (!hwnd) return;

    // Restore if minimized
    if (IsIconic(hwnd))
        ShowWindow(hwnd, SW_RESTORE);

    // Attach input threads
    DWORD notepadThreadId = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();
    AttachThreadInput(currentThreadId, notepadThreadId, TRUE);

    // Bring to foreground and set focus
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // Detach input threads
    AttachThreadInput(currentThreadId, notepadThreadId, FALSE);
}

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

    // Bring Notepad to the foreground
    BringNotepadToForeground();

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