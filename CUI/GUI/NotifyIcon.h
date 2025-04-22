#pragma once
#include <string>
#include <vector>
#include "Utils/Event.h"

// 菜单项类
class NotifyIconMenuItem
{
public:
    std::string Text;
    int ID;
    bool Enabled;
    bool Separator;
    bool HasSubMenu;
    HMENU SubMenu;
    std::vector<NotifyIconMenuItem> SubItems;

    NotifyIconMenuItem(const std::string& text, int id, bool enabled = true)
        : Text(text), ID(id), Enabled(enabled), Separator(false), HasSubMenu(false), SubMenu(NULL) {
    }

    static NotifyIconMenuItem CreateSeparator();
    // 添加子菜单项
    void AddSubItem(const NotifyIconMenuItem& item);
};
class NotifyIcon
{
private:
    NOTIFYICONDATAA NotifyIconData;
    int iconID;
    HMENU popupMenu;
    std::vector<NotifyIconMenuItem> menuItems;

public:
    HWND hWnd;
    static class NotifyIcon* Instance;
    NotifyIcon();
    ~NotifyIcon();
    void InitNotifyIcon(HWND hWnd, int iconID);
    void SetIcon(HICON hIcon);
    void ShowNotifyIcon();
    void HideNotifyIcon();
    void SetToolTip(const char* text);
    void ShowBalloonTip(const char* title, const char* text, int timeout = 5000, int type = NIIF_INFO);

    void AddMenuItem(const NotifyIconMenuItem& item);
    void AddMenuSeparator();
    void ShowContextMenu(int x, int y);
    void ClearMenu();
    void EnableMenuItem(int id, bool enable);
    void SetMenuItemText(int id, const std::string& text);

    // 新增子菜单相关方法
    NotifyIconMenuItem* CreateSubMenu(const std::string& text);
    void AddSubMenuItem(int parentId, const NotifyIconMenuItem& item);

    // 查找菜单项方法（内部使用）
    NotifyIconMenuItem* FindMenuItem(int id, std::vector<NotifyIconMenuItem>& items);
    NotifyIconMenuItem* FindMenuItem(int id);

    // 事件定义
    typedef Event<void(class NotifyIcon*, MouseEventArgs)> NotifyIconMouseDownEvent;
    NotifyIconMouseDownEvent OnNotifyIconMouseDown;

    typedef Event<void(class NotifyIcon*, int)> NotifyIconMenuClickEvent;
    NotifyIconMenuClickEvent OnNotifyIconMenuClick;
};
