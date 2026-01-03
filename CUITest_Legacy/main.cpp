#include "DemoWindow_Legacy.h"
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
int main()
{
	Application::EnsureDpiAwareness();
	DemoWindow_Legacy fm;
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