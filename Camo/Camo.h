#pragma once

// Main header for Camo application.
// Contains global declarations, constants, and function prototypes.

#include <windows.h>
#include "resource.h"
#include <tlhelp32.h>
#include <string>

// Application constants
#define MAX_LOADSTRING 100
#define ID_BUTTON_CONFIRM 1001
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 180
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 30
#define IDM_TOGGLEDARK 4001

// Global variables (defined in CamoUI.cpp)
extern HINSTANCE hInst;
extern WCHAR szTitle[MAX_LOADSTRING];
extern WCHAR szWindowClass[MAX_LOADSTRING];
extern HWND hButton;
extern HANDLE hScriptProcess;
extern HANDLE hNotepadProcess;
extern bool g_isDarkMode;

// Window and dialog management
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// Process management helpers
void TerminateAllNotepadInstances();
HWND FindMainWindow(DWORD pid);
HWND WaitForWindow(DWORD pid, int timeoutMs = 2000);
bool LaunchAndSnap(const wchar_t* cmd, int winWidth, int winHeight);

// Theme and UI helpers
void SetDarkTitleBar(HWND hwnd, bool dark);
void SetMenuDark(HWND hwnd);
void SnapWindowBottomLeft(HWND hwnd, int winWidth, int winHeight);
std::wstring ExtractScriptToTempFile();

#pragma comment(lib, "uxtheme.lib")
