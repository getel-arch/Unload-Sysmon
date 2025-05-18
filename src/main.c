#include <windows.h>
#include <stdio.h>
#include <fltuser.h> // Requires WDK

#pragma comment(lib, "FltLib.lib")

BOOL EnablePrivilege(LPCWSTR privName) {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return FALSE;

    if (!LookupPrivilegeValueW(NULL, privName, &luid)) {
        CloseHandle(hToken);
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    BOOL result = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL) &&
                  GetLastError() == ERROR_SUCCESS;
    CloseHandle(hToken);
    return result;
}

int main() {
    // Check for admin privileges
    BOOL isAdmin = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isAdmin = elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    if (!isAdmin) {
        printf("Error: This program must be run as Administrator.\n");
        return 1;
    }

    // Enable SeLoadDriverPrivilege
    if (!EnablePrivilege(L"SeLoadDriverPrivilege")) {
        printf("Warning: Failed to enable SeLoadDriverPrivilege.\n");
    } else {
        printf("SeLoadDriverPrivilege enabled.\n");
    }

    HRESULT hr = FilterUnload(L"SysmonDrv");
    if (SUCCEEDED(hr)) {
        printf("SysmonDrv minifilter unloaded successfully.\n");
    } else {
        printf("Failed to unload SysmonDrv minifilter. HRESULT: 0x%08lx\n", hr);
        if (hr == HRESULT_FROM_WIN32(ERROR_PRIVILEGE_NOT_HELD)) {
            printf("A required privilege is not held. Please run this program as Administrator.\n");
        }
    }

    return 0;
}

