#pragma once
#include <windows.h>

const wchar_t* IDI_SMALL = L"UDP_Controller.exe", * APP_NAME = L"UDP Controller";
constexpr auto IDR_MAINFRAME = 136886;
constexpr UINT_PTR ID_SHOW = 1, ID_CLOSE = 0;

NOTIFYICONDATA nid{};
HMENU hMenu;
WNDCLASS wndclass;
HWND my_handle;
MSG msg;
HANDLE tray_mutex;
bool tray_show = false;

void handleMsg() {
    while (true) {
        //消息循环
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
    HWND hwndFound;         // This is what is returned to the caller.
    wchar_t pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
    // WindowTitle.
    wchar_t pszOldWindowTitle[MY_BUFSIZE]; // Contains original
    // WindowTitle.

// Fetch current window title.

    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

    // Format a "unique" NewWindowTitle.

    wsprintf(pszNewWindowTitle, L"%d/%d",
        GetTickCount64(),
        GetCurrentProcessId());

    // Change current window title.

    SetConsoleTitle(pszNewWindowTitle);

    // Ensure window title has been updated.

    Sleep(40);

    // Look for NewWindowTitle.

    hwndFound = FindWindow(NULL, pszNewWindowTitle);

    // Restore original window title.

    SetConsoleTitle(pszOldWindowTitle);

    return(hwndFound);
}

void DeleteTray()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);//在托盘中删除图标
}

void destroy_tray()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
    PostQuitMessage(0);
    CloseHandle(tray_mutex);
    exit(1);
}

void switch_tray_show()
{
    HWND handle = GetConsoleHwnd();
    ShowWindow(handle, tray_show ? SW_HIDE : SW_SHOW);
    tray_show = !tray_show;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT WM_TASKBARCREATED;
    // 不要修改TaskbarCreated，这是系统任务栏自定义的消息
    WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
    switch (message)
    {
    case WM_CREATE:

        nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = IDR_MAINFRAME;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER;//自定义的消息 处理托盘图标事件

        HICON *phiconLarge;
        phiconLarge = new HICON();
        ::ExtractIconEx(IDI_SMALL, 0, phiconLarge, NULL, 2);
        nid.hIcon = *phiconLarge;
        nid.dwInfoFlags = NIIF_USER;
        nid.uTimeout = 1000;//参照变量详解，有些系统失效
        nid.uVersion = NOTIFYICON_VERSION_4;
        wcscpy_s(nid.szTip, APP_NAME);//鼠标放在托盘图标上时显示的文字
        wcscpy_s(nid.szInfo, L"系统托盘");
        wcscpy_s(nid.szInfoTitle, L"实现气泡点击！");
        Shell_NotifyIcon(NIM_SETVERSION, &nid);
        Shell_NotifyIcon(NIM_ADD, &nid);//在托盘区添加图标

        hMenu = CreatePopupMenu();//生成菜单
        //为托盘菜单添加两个选项
        AppendMenu(hMenu, MF_STRING, ID_SHOW, TEXT("显示/隐藏"));
        AppendMenu(hMenu, MF_STRING, ID_CLOSE, TEXT("关闭"));
        break;
    case WM_USER://连续使用该程序时候的消息.
        switch (lParam)
        {
        case WM_LBUTTONDBLCLK:
            switch_tray_show();
            break;
        case WM_RBUTTONDOWN:
            POINT pt;
            GetCursorPos(&pt);//取鼠标坐标
            ::SetForegroundWindow(hwnd);//解决在菜单外单击左键菜单不消失的问题
            //EnableMenuItem(hMenu, ID_SHOW, MF_GRAYED);//让菜单中的某一项变灰
            int xx = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hwnd, NULL);//显示菜单并获取选项ID
            switch (xx)
            {
            case ID_SHOW:
                switch_tray_show();
                break;
            case ID_CLOSE:
                destroy_tray();
                break;
            }
            break;
        }
        break;
    case WM_DESTROY://窗口销毁时候的消息.
        destroy_tray();
        break;
    default:
        /*
        *   防止当Explorer.exe 崩溃以后，程序在系统系统托盘中的图标就消失
            原理：Explorer.exe 重新载入后会重建系统任务栏。当系统任务栏建立的时候会向系统内
            所有注册接收TaskbarCreated 消息的顶级窗口发送一条消息，我们只需要捕捉这个消
            息，并重建系统托盘的图标即可。
        */
        if (message == WM_TASKBARCREATED)
            SendMessage(hwnd, WM_CREATE, wParam, lParam);
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void ToTray()
{
    tray_mutex = CreateMutex(NULL, FALSE, L"{80A85553-1E05-4323-B4F9-43A4396A4507}");
    if (GetLastError() == ERROR_ALIAS_EXISTS)
    {
        MessageBox(NULL, L"Another Instance Running!", L"Tip", MB_OK);
        CloseHandle(tray_mutex);
        exit(1);
    }

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = NULL;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = APP_NAME;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), APP_NAME, MB_ICONERROR);
        CloseHandle(tray_mutex);
        exit(1);
    }

    // 此处使用WS_EX_TOOLWINDOW 属性来隐藏显示在任务栏上的窗口程序按钮
    my_handle = CreateWindowEx(WS_EX_TOOLWINDOW,
        APP_NAME, APP_NAME,
        WS_POPUP,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL, NULL, NULL, NULL);

    ShowWindow(my_handle, 0);
    UpdateWindow(my_handle);

    handleMsg();
}