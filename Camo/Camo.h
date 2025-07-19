#pragma once

#include <windows.h>
#include "resource.h"
#include <tlhelp32.h>

#define MAX_LOADSTRING 100
#define ID_BUTTON_CONFIRM 1001
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 180
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 30
#define IDM_TOGGLEDARK 4001

extern HINSTANCE hInst;
extern WCHAR szTitle[MAX_LOADSTRING];
extern WCHAR szWindowClass[MAX_LOADSTRING];
extern HWND hButton;
extern HANDLE hScriptProcess;
extern HANDLE hNotepadProcess;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
void TerminateAllNotepadInstances();
