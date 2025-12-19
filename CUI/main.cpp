#include "DemoWindow.h"
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

int main()
{
    auto form = DemoWindow();
    form.Show();
	NotifyIcon* notifyIcon = TestNotifyIcon(form.Handle);

	int index = 0;
	while (1)
	{
		Form::DoEvent();
		if (Application::Forms.size() == 0)
			break;
	}
    notifyIcon->HideNotifyIcon();
	return 0;
}