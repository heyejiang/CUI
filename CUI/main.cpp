#include "AudioView.h"
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
int main()
{
	AudioView* av = new AudioView();
	av->Show();
    // 创建通知图标
    NotifyIcon notifyIcon;
    notifyIcon.InitNotifyIcon(av->Handle, 1);
    notifyIcon.SetIcon(LoadIcon(NULL, IDI_APPLICATION));
    notifyIcon.SetToolTip("应用程序");
    notifyIcon.ShowNotifyIcon();

    // 添加普通菜单项
    notifyIcon.AddMenuItem(NotifyIconMenuItem("打开主窗口", 1));

    // 创建"设置"子菜单
    NotifyIconMenuItem settingsMenu("设置", 2);
    settingsMenu.HasSubMenu = true;
    settingsMenu.SubMenu = CreatePopupMenu();

    // 添加子菜单项
    settingsMenu.AddSubItem(NotifyIconMenuItem("音频设置", 21));
    settingsMenu.AddSubItem(NotifyIconMenuItem("显示设置", 22));
    settingsMenu.AddSubItem(NotifyIconMenuItem::CreateSeparator());
    settingsMenu.AddSubItem(NotifyIconMenuItem("高级设置", 23));

    // 添加到主菜单
    notifyIcon.AddMenuItem(settingsMenu);

    // 添加分隔符和退出选项
    notifyIcon.AddMenuSeparator();
    notifyIcon.AddMenuItem(NotifyIconMenuItem("退出", 3));

    // 绑定菜单点击事件
    notifyIcon.OnNotifyIconMenuClick += [](NotifyIcon* sender, int menuId) {
            switch (menuId) {
            case 1: // 打开主窗口
                ShowWindow(sender->hWnd, SW_SHOW);
                break;
            case 21: // 音频设置
                // 打开音频设置窗口的代码
                break;
            case 22: // 显示设置
                // 打开显示设置窗口的代码
                break;
            case 23: // 高级设置
                // 打开高级设置窗口的代码
                break;
            case 3: // 退出
                PostMessage(sender->hWnd, WM_CLOSE, 0, 0);
                break;
            }
        };
	int index = 0;
	while (1)
	{
		Form::DoEvent();
		if (Application::Forms.size() == 0)
			break;
	}
	return 0;
}