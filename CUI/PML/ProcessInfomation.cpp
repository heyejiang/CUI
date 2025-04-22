#include "ProcessInfomation.h"
LDR_DATA_TABLE_ENTRY64* ModuleList::NULL_MODULE = NULL;
LDR_DATA_TABLE_ENTRY64& ModuleList::operator[](const wchar_t* name)
{
	for (int i = 0; i < this->size(); i++){
		const wchar_t* wname = this->at(i).BaseDllName.Buffer;
		if (!_wcsicmp(wname, name))
			return this->at(i);
	}
	return *NULL_MODULE;
}
LDR_DATA_TABLE_ENTRY64& ModuleList::operator[](const char* name)
{
	int size = MultiByteToWideChar(CP_ACP, 0, name, strlen(name) + 1, NULL, 0);
	wchar_t* tchar = new wchar_t[sizeof(wchar_t) * size];
	MultiByteToWideChar(CP_ACP, 0, name, strlen(name) + 1, tchar, size);
	for (int i = 0; i < this->size(); i++){
		const wchar_t* wname = this->at(i).BaseDllName.Buffer;
		if (!_wcsicmp(wname, tchar)){
			delete[] tchar;
			return this->at(i);
		}
	}
	delete[] tchar;
	return *NULL_MODULE;
}
ProcessInfomation::ProcessInfomation(HANDLE processId)
{
	ProcessId = processId;
	if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ProcessId, &Process)))
	{
		Process = NULL;
	}
}
ULONG64 ProcessInfomation::SectionBase()
{
	return (ULONG64)PsGetProcessSectionBaseAddress(Process);
}
BOOL ProcessInfomation::IsAlive()
{
	return PsGetProcessExitStatus(Process) == STATUS_PENDING;
}
ModuleList ProcessInfomation::Modules()
{
	ModuleList result = ModuleList();
	int index = 0;
	while (1)
	{
		LDR_DATA_TABLE_ENTRY64 mi = PsLookProcessModuleIndex(Process, index++);
		if (!mi.DllBase)
			break;
		wchar_t* newFullDllNamebuffer = new wchar_t[mi.FullDllName.MaximumLength];
		wchar_t* newBaseDllNamebuffer = new wchar_t[mi.BaseDllName.MaximumLength];
		ReadMemory((ULONG64)mi.FullDllName.Buffer, newFullDllNamebuffer, mi.FullDllName.MaximumLength);
		ReadMemory((ULONG64)mi.BaseDllName.Buffer, newBaseDllNamebuffer, mi.BaseDllName.MaximumLength);
		mi.FullDllName.Buffer = newFullDllNamebuffer;
		mi.BaseDllName.Buffer = newBaseDllNamebuffer;
		result.push_back(mi);
	}
	return result;
}
BOOL ProcessInfomation::ReadMemory(ULONG64 addr, PVOID buffer, SIZE_T size)
{
	return NT_SUCCESS(SafeReadProcessMemory(Process, addr, buffer, size));
}
BOOL ProcessInfomation::WriteMemory(ULONG64 addr, PVOID buffer, SIZE_T size)
{
	return NT_SUCCESS(WriteProcessMemory(Process, addr, buffer, size));
}

std::vector<ULONG64> ProcessInfomation::SerachInProcess(const char* key)
{
	return SerachProcessEx(Process, key);
}
std::vector<ULONG64> ProcessInfomation::SerachInModule(const wchar_t* _module, const char* key)
{
	return SerachProcessEx(Process, _module, key);
}