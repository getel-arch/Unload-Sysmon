# Unload-Sysmon

[![Build & Release](https://github.com/getel-arch/Unload-Sysmon/actions/workflows/build_and_release.yaml/badge.svg)](https://github.com/getel-arch/Unload-Sysmon/actions/workflows/build_and_release.yaml)

## Overview

Unload-Sysmon provides utilities to unload the Sysmon minifilter driver from Windows systems.  
This repository contains implementations in:

- **C** (`src/main.c`)
- **PowerShell** (`Unload-Sysmon.ps1`)
- **Golang** (`cmd/unload-sysmon/main.go`)

## Usage

### PowerShell

```powershell
# Run as Administrator
.\Unload-Sysmon.ps1
```

### C

Build with Visual Studio or MinGW, then run as Administrator:

```sh
unload_sysmon_x64.exe
```

### Golang

```sh
go build -o unload_sysmon_go_x64.exe ./cmd/unload-sysmon
unload_sysmon_go_x64.exe
```

## Structure

- `src/main.c` — C implementation
- `Unload-Sysmon.ps1` — PowerShell implementation
- `cmd/unload-sysmon/main.go` — Golang implementation