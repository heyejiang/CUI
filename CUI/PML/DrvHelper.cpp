#include "DrvHelper.h"

using namespace std;
BOOL InstallDriver(const char* DriverPath, const char* ServiceName)
{
	DWORD m_errorCode = 0;
	SC_HANDLE sc_manage = NULL;
	SC_HANDLE sc_service = NULL;
	BOOL bRet = FALSE;

	do
	{
		sc_manage = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (NULL == sc_manage)
		{
			m_errorCode = GetLastError();
			break;
		}
		sc_service = CreateServiceA(
			sc_manage,
			ServiceName,
			ServiceName,
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE,
			DriverPath,
			NULL, NULL, NULL, NULL, NULL);

		if (NULL == sc_service)
		{
			m_errorCode = GetLastError();
			if (m_errorCode == 1073)
			{
				return true;
			}
			break;
		}

		bRet = TRUE;
	} while (0);

	if (sc_service != NULL)
	{
		CloseServiceHandle(sc_service);
		sc_service = NULL;
	}
	if (sc_manage != NULL)
	{
		CloseServiceHandle(sc_manage);
		sc_manage = NULL;
	}
	return bRet;
}
BOOL StartDriver(const char* ServiceName)
{
	SC_HANDLE sc_manage = NULL;
	SC_HANDLE sc_service = NULL;
	BOOL bRet = FALSE;
	DWORD m_errorCode = 0;

	do
	{
		sc_manage = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (NULL == sc_manage)
		{
			m_errorCode = GetLastError();
			break;
		}

		sc_service = OpenServiceA(sc_manage, ServiceName, SERVICE_ALL_ACCESS);
		if (NULL == sc_service)
		{
			m_errorCode = GetLastError();
			break;
		}

		if (StartService(sc_service, NULL, NULL) == FALSE)
		{
			m_errorCode = GetLastError();
			break;
		}

		bRet = TRUE;
	} while (0);

	if (sc_service != NULL)
	{
		CloseServiceHandle(sc_service);
		sc_service = NULL;
	}

	if (sc_manage != NULL)
	{
		CloseServiceHandle(sc_manage);
		sc_manage = NULL;
	}

	return bRet;
}
BOOL StopDriver(const char* ServiceName)
{
	SC_HANDLE sc_manage = NULL;
	SC_HANDLE sc_service = NULL;
	SERVICE_STATUS ss = { 0 };
	BOOL bRet = FALSE;
	DWORD m_errorCode = 0;

	do
	{
		sc_manage = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (NULL == sc_manage)
		{
			m_errorCode = GetLastError();
			break;
		}

		sc_service = OpenServiceA(sc_manage, ServiceName, SERVICE_ALL_ACCESS);
		if (NULL == sc_service)
		{
			m_errorCode = GetLastError();
			break;
		}

		if (ControlService(sc_service, SERVICE_CONTROL_STOP, &ss) == FALSE)
		{
			m_errorCode = GetLastError();
			break;
		}

		bRet = TRUE;
	} while (0);

	if (sc_service != NULL)
	{
		CloseServiceHandle(sc_service);
		sc_service = NULL;
	}

	if (sc_manage != NULL)
	{
		CloseServiceHandle(sc_manage);
		sc_manage = NULL;
	}

	return bRet;
}
BOOL UnInstallDriver(const char* ServiceName)
{
	SC_HANDLE sc_manage = NULL;
	SC_HANDLE sc_service = NULL;
	BOOL bRet = FALSE;
	DWORD m_errorCode = 0;

	do
	{
		sc_manage = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (sc_manage == NULL)
		{
			m_errorCode = GetLastError();
			break;
		}

		sc_service = OpenServiceA(sc_manage, ServiceName, SERVICE_ALL_ACCESS);
		if (sc_service == NULL)
		{
			m_errorCode = GetLastError();
			break;
		}

		if (DeleteService(sc_service) == FALSE)
		{
			m_errorCode = GetLastError();
			break;
		}

		bRet = TRUE;
	} while (0);

	if (sc_service != NULL)
	{
		CloseServiceHandle(sc_service);
		sc_service = NULL;
	}

	if (sc_manage != NULL)
	{
		CloseServiceHandle(sc_manage);
		sc_manage = NULL;
	}

	return bRet;
}