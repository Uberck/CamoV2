#include "Camo.h"
#include <fstream>
#include <windows.h>
#include <string>
#include "resource.h"

// Extracts the embedded PowerShell script to a temp .ps1 file and returns its path
std::wstring ExtractScriptToTempFile()
{
    // Locate resource in the main executable
    HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(IDR_PS_SCRIPT), L"TEXT");
    if (!hRes) return L"";

    HGLOBAL hData = LoadResource(nullptr, hRes);
    if (!hData) return L"";

    DWORD size = SizeofResource(nullptr, hRes);
    const char* pData = static_cast<const char*>(LockResource(hData));
    if (!pData || size == 0) return L"";

    // Get temp path and create a unique .ps1 file name
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);

    wchar_t tempFile[MAX_PATH];
    // Use a unique name with .ps1 extension
    wsprintfW(tempFile, L"%sPS_%08X.ps1", tempPath, GetTickCount());

    // Write resource data to temp file
    std::ofstream ofs(tempFile, std::ios::binary);
    ofs.write(pData, size);
    ofs.close();

    return tempFile;
}

// Terminates all running Notepad instances
void TerminateAllNotepadInstances()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"notepad.exe") == 0) {
                HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProc) {
                    TerminateProcess(hProc, 0);
                    CloseHandle(hProc);
                }
            }
        } while (Process32NextW(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
}

// Finds the main window handle for a process by its PID
HWND FindMainWindow(DWORD pid) {
    struct HandleData {
        DWORD pid;
        HWND hwnd;
    } data = { pid, nullptr };

    auto EnumWindowsCallback = [](HWND hwnd, LPARAM lParam) -> BOOL {
        HandleData& data = *(HandleData*)lParam;
        DWORD windowPid = 0;
        GetWindowThreadProcessId(hwnd, &windowPid);
        if (windowPid == data.pid && IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == nullptr) {
            data.hwnd = hwnd;
            return FALSE;
        }
        return TRUE;
    };

    EnumWindows(EnumWindowsCallback, (LPARAM)&data);
    return data.hwnd;
}

// Waits for a window to appear for a process, with a timeout
HWND WaitForWindow(DWORD pid, int timeoutMs)
{
    HWND hwnd = nullptr;
    int elapsed = 0;
    while (elapsed < timeoutMs) {
        hwnd = FindMainWindow(pid);
        if (hwnd) break;
        Sleep(100);
        elapsed += 100;
    }
    return hwnd;
}

// Launches a process and snaps its window to a specified location
bool LaunchAndSnap(const wchar_t* cmd, int winWidth, int winHeight)
{
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };
    BOOL success = CreateProcessW(
        nullptr,
        (LPWSTR)cmd,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );
    if (success) {
        CloseHandle(pi.hThread);
        HWND hwnd = WaitForWindow(pi.dwProcessId);
        if (hwnd) {
            SnapWindowBottomLeft(hwnd, winWidth, winHeight);
            return true;
        }
        CloseHandle(pi.hProcess);
    }
    return false;
}