#include <windows.h>
#include <stdio.h>

typedef void (*PFN_UNLOAD)();

int main() {
    // Load the Sysmon driver DLL
    HMODULE hSysmonDrv = LoadLibraryA("sysmondrv");
    if (!hSysmonDrv) {
        printf("Failed to load sysmondrv. Error: %lu\n", GetLastError());
        return 1;
    }
    printf("Loaded sysmondrv at 0x%p\n", hSysmonDrv);

    // Get the address of the unload function
    PFN_UNLOAD pfnUnload = (PFN_UNLOAD)GetProcAddress(hSysmonDrv, "DriverUnload");
    if (!pfnUnload) {
        printf("Failed to get address of DriverUnload. Error: %lu\n", GetLastError());
        FreeLibrary(hSysmonDrv);
        return 1;
    }
    printf("Address of DriverUnload: 0x%p\n", pfnUnload);

    // Call the unload function
    printf("Calling DriverUnload...\n");
    pfnUnload();
    printf("DriverUnload called.\n");

    FreeLibrary(hSysmonDrv);

    return 0;
}

