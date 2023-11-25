#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

DWORD GetProcessIdByName(const char* processName) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (_stricmp(pe32.szExeFile, processName) == 0) {
                CloseHandle(hProcessSnap);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return 0;
}

HWND hwndOutput; // Global variable for the output box

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            CreateWindowA("STATIC", "Process Name:", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 10, 100, 20, hwnd, NULL, NULL, NULL);

            CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 120, 10, 170, 20, hwnd, (HMENU)1, NULL, NULL);

            hwndOutput = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 10, 40, 280, 100, hwnd, NULL, NULL, NULL);

            // create button
            CreateWindowA("BUTTON", "Detect Process", WS_VISIBLE | WS_CHILD, 10, 150, 100, 30, hwnd, (HMENU)2, NULL, NULL);

            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 2) { // Use ID 2 for the button
                char processName[256];
                GetWindowTextA(GetDlgItem(hwnd, 1), processName, 256); // Use ID 1 for the edit control

                DWORD processId = GetProcessIdByName(processName);

                if (processId) {
                    SendMessageA(hwndOutput, EM_SETSEL, -1, -1);
                    SendMessageA(hwndOutput, EM_REPLACESEL, 0, (LPARAM)"Process found lmaoo\n");
                } else {
                    SendMessageA(hwndOutput, EM_SETSEL, -1, -1);
                    SendMessageA(hwndOutput, EM_REPLACESEL, 0, (LPARAM)"Process not found\n");
                }
            }
            break;
        }
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, CreateSolidBrush(RGB(192, 192, 192))); // RGB values for grey
            return 1; // Tell Windows that we've handled erasing the background
        }
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "MyWindowClass";
    wc.hbrBackground = CreateSolidBrush(RGB(192, 192, 192)); // Set initial background color

    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Error registering window class.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowA("MyWindowClass", "Process Detector", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 300, 250, 0, 0, 0, 0);

    if (!hwnd) {
        MessageBoxA(NULL, "Error creating window.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
