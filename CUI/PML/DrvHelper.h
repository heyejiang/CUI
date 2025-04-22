#pragma once
#include <tchar.h>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <fstream>
#include <string>
#include <atlbase.h>
#include <atlstr.h>
#include <vector>
BOOL InstallDriver(const char* DriverPath, const char* ServiceName);
BOOL StartDriver(const char* ServiceName);
BOOL StopDriver(const char* ServiceName);
BOOL UnInstallDriver(const char* ServiceName);