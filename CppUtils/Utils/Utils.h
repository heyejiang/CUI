#pragma once
#include "defines.h"
#include <vector>
#include <string>

#include "TimeSpan.h"
#include "StopWatch.h"
#include "StringBuilder.h"
#include "Event.h"
#include "List.h"
#include "File.h"
#include "Guid.h"
#include "Tuple.h"
#include "Dialog.h"
#include "Convert.h"
#include "Process.h"
#include "CRandom.h"
#include "FileInfo.h"
#include "DateTime.h"
#include "Registry.h"
#include "Socket.h"
#include "FileStream.h"
#include "Dictionary.h"
#include "HttpHelper.h"
#include "Environment.h"
#include "StringHelper.h"
#include "json.h"
#include "Thread.h"
#include "DataPack.h"
#include "Clipboard.h"
#include "zlib/zlib.h"

using json = JsonLib::json;

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#ifndef RELOC
#define RELOC(p,o) (void*)((char*)p ? (((char*)p + o + 4) + (*(int*)((char*)p + o))) : NULL)
#endif

DWORD NtBuildVersion();
std::vector<__int16> ParserPattern(const char* text);
void* FindU64(const char* szModule, ULONG64 value);
void* FindPattern(const char* szModule, const char* sPattern, int offset = 0);
std::vector<void*> FindAllPattern(const char* szModule, const char* sPattern, int offset = 0);
void* FindPattern(void* _begin, const char* sPattern, int search_size, int offset = 0);
std::vector<void*> FindAllPattern(void* _begin, const char* sPattern, int search_size, int offset = 0);
void PrintHex(void* ptr, int count, int splitLine);
void PrintHex(void* ptr, int count);
void MakePermute(std::vector<int> nums, std::vector<std::vector<int>>& result, int start = 0);
std::wstring GetErrorMessage(DWORD err);
PIMAGE_NT_HEADERS RtlImageNtHeader(PVOID Base);
SIZE_T GetSectionSize(_In_ PVOID DllBase);
void EnableDump();
List<void*> CaptureStackTraceEx();
#ifdef ZLIB_VERSION
std::vector<uint8_t> Compress(uint8_t* buffer, int len);
std::vector<uint8_t> Decompress(uint8_t* buffer, int len, int maxlen);
std::vector<uint8_t> GDecompress(std::vector<uint8_t> compressedBytes);
std::vector<uint8_t> GCompress(std::vector<uint8_t> input);
std::vector<uint8_t> GDecompress(uint8_t* compressedBytes, ULONG len);
std::vector<uint8_t> GCompress(uint8_t* input, ULONG len);
std::vector<uint8_t> GCompress(std::initializer_list<uint8_t> input);
std::vector<uint8_t> GCompress(std::initializer_list<uint8_t>* input);
std::string MakeDialogFilterStrring(std::string description, std::vector<std::string> filter);
std::string MakeDialogFilterStrring(std::string description, std::string filter);
#endif