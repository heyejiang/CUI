#pragma once
#include "PML.h"
#include <DbgHelp.h>

class ModuleList : public std::vector<LDR_DATA_TABLE_ENTRY64>
{
public:
	static LDR_DATA_TABLE_ENTRY64* NULL_MODULE;
	LDR_DATA_TABLE_ENTRY64& operator[](const wchar_t* name);
	LDR_DATA_TABLE_ENTRY64& operator[](const char* name);
};
class ProcessInfomation
{
public:
	PEPROCESS Process;
	HANDLE ProcessId;
	ProcessInfomation(HANDLE processId);
	ULONG64 SectionBase();
	BOOL IsAlive();
	ModuleList Modules();
	BOOL ReadMemory(ULONG64 addr, PVOID buffer, SIZE_T size);
	BOOL WriteMemory(ULONG64 addr, PVOID buffer, SIZE_T size);
	std::vector<ULONG64> SerachInProcess(const char* key);
	std::vector<ULONG64> SerachInModule(const wchar_t* _module, const char* key);
	template<typename T>
	T Read(ULONG64 addr)
	{
		T value = T();
		ReadProcessMemory(Process, addr, &value, sizeof(T));
		return value;
	}
	template<typename T>
	BOOL Write(ULONG64 addr, T value)
	{
		return NT_SUCCESS(WriteProcessMemory(Process, addr, &value, sizeof(T)));
	}
	template<typename T>
	std::vector<ULONG64> SerachInProcess(T value,SIZE_T aligning = 4)
	{
		MEMORY_BASIC_INFORMATION info = { 0 };
		SIZE_T dwsiz = 0;
		PVOID CurrentAddress = NULL;
		std::vector<ULONG64> result = std::vector<ULONG64>();
		while NT_SUCCESS(QueryVirtualMemory(Process, CurrentAddress, 0, &info, sizeof(MEMORY_BASIC_INFORMATION), &dwsiz))
		{
			if (info.State == MEM_COMMIT)
			{
				BYTE* BUFFER = new BYTE[info.RegionSize];
				if (BUFFER)
				{
					ZeroMemory(BUFFER, info.RegionSize);
					if (NT_SUCCESS(ReadProcessMemory(Process, (ULONG64)info.BaseAddress, BUFFER, info.RegionSize)))
					{
						for (int i = 0; i <= info.RegionSize - sizeof(T); i += aligning)
						{
							if (*(T*)(BUFFER + i) == value)
								result.push_back((ULONG64)info.BaseAddress + i);
						}
					}
					delete[] BUFFER;
				}
			}
			CurrentAddress = (PVOID)((UINT64)info.BaseAddress + info.RegionSize);
		}
	}
	template<typename T>
	std::vector<ULONG64> SerachInModule(const wchar_t* _module, T value, SIZE_T aligning = 4)
	{
		MEMORY_BASIC_INFORMATION info = { 0 };
		LDR_DATA_TABLE_ENTRY64 moduleinfo = PsLookProcessModule(Process, _module);
		SIZE_T dwsiz = 0;
		std::vector<ULONG64> result = std::vector<ULONG64>();
		BYTE* BUFFER = new BYTE[moduleinfo.SizeOfImage];
		if (BUFFER)
		{
			ZeroMemory(BUFFER, moduleinfo.SizeOfImage);
			if (NT_SUCCESS(ReadProcessMemory(Process, (ULONG64)moduleinfo.DllBase, BUFFER, moduleinfo.SizeOfImage)))
			{
				for (int i = 0; i <= info.RegionSize - sizeof(T); i += aligning)
				{
					if (*(T*)(BUFFER + i) == value)
						result.push_back((ULONG64)info.BaseAddress + i);
				}
			}
			delete[] BUFFER;
		}
		return result;
	}

};

