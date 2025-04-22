#pragma once
#include "Utils/Utils.h"
#include "Utils/ProcessOperator.h"
class WeChatHelper
{
	ProcessOperator* op = NULL;
	ULONG64 SendTextMsg = NULL;
	ULONG64 SendImageMsg = NULL;
	ULONG64 SendPatMsg = NULL;
	ULONG64 FreeChatMsg = NULL;
	ULONG64 GetAccountService = NULL;
	ULONG64 ChatMsgInstanceCounter = NULL;
	ULONG64 GetContactMgr = NULL;
	ULONG64 GetMemberFromChatRoom = NULL;
	ULONG64 NewChatRoom = NULL;
	ULONG64 FreeChatRoom = NULL;
	UINT32 ContactSize = NULL;
public:
	WeChatHelper();
	void SendTextMessage(std::wstring wxIdstr, std::wstring msgstr);
};

