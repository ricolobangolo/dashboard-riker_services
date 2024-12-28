#include <windows.h>
#include <stdio.h>
#include <winsvc.h>

#define IDC_LISTBOX 1001
#define IDC_BTN_SERVICES 1002
#define IDC_BTN_CLEANRAM 1003
#define IDC_BTN_OPENURL 1004

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void EnumServices(HWND hwndList);
void CleanRam();
void OpenURL();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "RikerServicesWindowClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Riker Services Dashboard",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,  // <-- Taille de la fenêtre augmentée
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndList, hwndBtnServices, hwndBtnCleanRam, hwndBtnOpenURL, hwndTitle;
    switch (uMsg) {
        case WM_CREATE:
        {
            hwndTitle = CreateWindowW(
                L"STATIC", L"RIKER SERVICES",
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                200, 10, 400, 40,  // <-- Taille et position ajustées
                hwnd, NULL, NULL, NULL
            );

            hwndBtnServices = CreateWindowW(
                L"BUTTON", L"Afficher Services",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                50, 60, 200, 40,  // <-- Taille et position ajustées
                hwnd, (HMENU)IDC_BTN_SERVICES, GetModuleHandle(NULL), NULL
            );

            hwndBtnCleanRam = CreateWindowW(
                L"BUTTON", L"Nettoyer RAM",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                300, 60, 200, 40,  // <-- Taille et position ajustées
                hwnd, (HMENU)IDC_BTN_CLEANRAM, GetModuleHandle(NULL), NULL
            );

            hwndBtnOpenURL = CreateWindowW(
                L"BUTTON", L"Ouvrir le Site",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                550, 60, 200, 40,  // <-- Taille et position ajustées
                hwnd, (HMENU)IDC_BTN_OPENURL, GetModuleHandle(NULL), NULL
            );

            hwndList = CreateWindowW(
                L"LISTBOX", NULL,
                WS_CHILD | WS_VISIBLE | LBS_STANDARD,
                10, 120, 760, 420,  // <-- Taille et position ajustées
                hwnd, (HMENU)IDC_LISTBOX, NULL, NULL
            );

            return 0;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_BTN_SERVICES) {
                EnumServices(hwndList);
            } else if (LOWORD(wParam) == IDC_BTN_CLEANRAM) {
                CleanRam();
            } else if (LOWORD(wParam) == IDC_BTN_OPENURL) {
                OpenURL();
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void EnumServices(HWND hwndList) {
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
    SC_HANDLE hSCManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSCManager == NULL) {
        MessageBox(NULL, "OpenSCManager failed", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    ENUM_SERVICE_STATUS ssStatus[1024];
    DWORD dwBytesNeeded, dwServicesReturned, dwResumeHandle = 0;
    BOOL bEnumResult;

    do {
        bEnumResult = EnumServicesStatus(
            hSCManager,
            SERVICE_WIN32,
            SERVICE_STATE_ALL,
            ssStatus,
            sizeof(ssStatus),
            &dwBytesNeeded,
            &dwServicesReturned,
            &dwResumeHandle);

        if (!bEnumResult) {
            if (GetLastError() != ERROR_MORE_DATA) {
                MessageBox(NULL, "EnumServicesStatus failed", "Error", MB_OK | MB_ICONERROR);
                CloseServiceHandle(hSCManager);
                return;
            }
        }

        for (DWORD i = 0; i < dwServicesReturned; i++) {
            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)ssStatus[i].lpServiceName);
        }
    } while (!bEnumResult);

    CloseServiceHandle(hSCManager);
}

void CleanRam() {
    SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
    MessageBox(NULL, "Nettoyage de la RAM effectué!", "Information", MB_OK | MB_ICONINFORMATION);
}

void OpenURL() {
    ShellExecute(0, 0, "https://tikbisness2.wixsite.com/rikerservice-secure", 0, 0, SW_SHOW);
}
