#include <windows.h>
#include <stdio.h>
#include <fltuser.h> // Requires WDK

#pragma comment(lib, "FltLib.lib")

int main() {
    // Print current working directory for debugging
    char cwd[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, cwd)) {
        printf("Current working directory: %s\n", cwd);
    }

    HRESULT hr = FilterUnload(L"SysmonDrv");
    if (SUCCEEDED(hr)) {
        printf("SysmonDrv minifilter unloaded successfully.\n");
    } else {
        printf("Failed to unload SysmonDrv minifilter. HRESULT: 0x%08lx\n", hr);
    }

    return 0;
}

