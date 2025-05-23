# Check for Administrator privileges
$isAdmin = ([Security.Principal.WindowsPrincipal] `
            [Security.Principal.WindowsIdentity]::GetCurrent() `
           ).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Error "Error: This script must be run as Administrator."
    exit 1
}

$definition = @"
using System;
using System.Runtime.InteropServices;
public static class WinAPI {
    [DllImport("ntdll.dll", SetLastError = true)]
    public static extern int RtlAdjustPrivilege(
        int Privilege,
        bool Enable,
        bool CurrentThread,
        out bool Enabled
    );

    [DllImport("fltlib.dll", SetLastError = true, CharSet = CharSet.Unicode)]
    public static extern int FilterUnload(string lpFilterName);
}
"@
Add-Type -TypeDefinition $definition

# Enable SeLoadDriverPrivilege (constant 10)
$enabled = $false
$status = [WinAPI]::RtlAdjustPrivilege(10, $true, $false, [ref] $enabled)
if ($status -ne 0) {
    Write-Warning "Warning: Failed to enable SeLoadDriverPrivilege (NTSTATUS: 0x{0:X8})." -f $status
} else {
    Write-Host "SeLoadDriverPrivilege enabled."
}

# Attempt to unload "SysmonDrv"
$driverName = "SysmonDrv"
$hr = [WinAPI]::FilterUnload($driverName)
if ($hr -eq 0) {
    Write-Host "SysmonDrv minifilter unloaded successfully."
} else {
    Write-Host ("Failed to unload SysmonDrv minifilter. HRESULT: 0x{0:X8}" -f $hr)
    # HRESULT_FROM_WIN32(ERROR_PRIVILEGE_NOT_HELD) == 0x80070222
    if ($hr -eq 0x80070222) {
        Write-Host "A required privilege is not held. Please run this script as Administrator."
    }
}
