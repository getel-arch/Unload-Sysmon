package main

import (
	"fmt"
	"golang.org/x/sys/windows"
	"syscall"
	"unsafe"
)

func enableSeLoadDriverPrivilege() error {
	var hToken windows.Token
	err := windows.OpenProcessToken(windows.CurrentProcess(), windows.TOKEN_ADJUST_PRIVILEGES|windows.TOKEN_QUERY, &hToken)
	if err != nil {
		return err
	}
	defer hToken.Close()

	var luid windows.LUID
	err = windows.LookupPrivilegeValue(nil, syscall.StringToUTF16Ptr("SeLoadDriverPrivilege"), &luid)
	if err != nil {
		return err
	}

	tp := windows.Tokenprivileges{
		PrivilegeCount: 1,
		Privileges: [1]windows.LUIDAndAttributes{{
			Luid:       luid,
			Attributes: windows.SE_PRIVILEGE_ENABLED,
		}},
	}

	return windows.AdjustTokenPrivileges(hToken, false, &tp, 0, nil, nil)
}

func isAdmin() bool {
	var sid *windows.SID
	// WinBuiltinAdministratorsSid = 26
	sid, _ = windows.CreateWellKnownSid(windows.WinBuiltinAdministratorsSid, nil)
	token := windows.Token(0)
	member, _ := token.IsMember(sid)
	return member
}

func filterUnload(filterName string) (uint32, error) {
	fltlib := windows.NewLazySystemDLL("fltlib.dll")
	proc := fltlib.NewProc("FilterUnload")
	namePtr, _ := syscall.UTF16PtrFromString(filterName)
	r1, _, err := proc.Call(uintptr(unsafe.Pointer(namePtr)))
	return uint32(r1), err
}

func main() {
	if !isAdmin() {
		fmt.Println("Error: This program must be run as Administrator.")
		return
	}

	if err := enableSeLoadDriverPrivilege(); err != nil {
		fmt.Printf("Warning: Failed to enable SeLoadDriverPrivilege: %v\n", err)
	} else {
		fmt.Println("SeLoadDriverPrivilege enabled.")
	}

	hr, _ := filterUnload("SysmonDrv")
	if hr == 0 {
		fmt.Println("SysmonDrv minifilter unloaded successfully.")
	} else {
		fmt.Printf("Failed to unload SysmonDrv minifilter. HRESULT: 0x%08X\n", hr)
		if hr == 0x80070222 {
			fmt.Println("A required privilege is not held. Please run this program as Administrator.")
		}
	}
}
