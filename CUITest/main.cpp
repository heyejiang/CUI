#include "DemoWindow.h"
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
int main()
{
	Application::EnsureDpiAwareness();
	DemoWindow fm;
	fm.Show();
	auto notify = TestNotifyIcon(fm.Handle);
	notify->ShowNotifyIcon();
	while (1)
	{
		Form::DoEvent();
		if (Application::Forms.size() == 0)
			break;
	}
	notify->HideNotifyIcon();
	return 0;
}