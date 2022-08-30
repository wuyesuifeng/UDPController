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
        //��Ϣѭ��
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
    Shell_NotifyIcon(NIM_DELETE, &nid);//��������ɾ��ͼ��
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
    // ��Ҫ�޸�TaskbarCreated������ϵͳ�������Զ������Ϣ
    WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
    switch (message)
    {
    case WM_CREATE:

        nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = IDR_MAINFRAME;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER;//�Զ������Ϣ ��������ͼ���¼�

        HICON *phiconLarge;
        phiconLarge = new HICON();
        ::ExtractIconEx(IDI_SMALL, 0, phiconLarge, NULL, 2);
        nid.hIcon = *phiconLarge;
        nid.dwInfoFlags = NIIF_USER;
        nid.uTimeout = 1000;//���ձ�����⣬��ЩϵͳʧЧ
        nid.uVersion = NOTIFYICON_VERSION_4;
        wcscpy_s(nid.szTip, APP_NAME);//����������ͼ����ʱ��ʾ������
        wcscpy_s(nid.szInfo, L"ϵͳ����");
        wcscpy_s(nid.szInfoTitle, L"ʵ�����ݵ����");
        Shell_NotifyIcon(NIM_SETVERSION, &nid);
        Shell_NotifyIcon(NIM_ADD, &nid);//�����������ͼ��

        hMenu = CreatePopupMenu();//���ɲ˵�
        //Ϊ���̲˵��������ѡ��
        AppendMenu(hMenu, MF_STRING, ID_SHOW, TEXT("��ʾ/����"));
        AppendMenu(hMenu, MF_STRING, ID_CLOSE, TEXT("�ر�"));
        break;
    case WM_USER://����ʹ�øó���ʱ�����Ϣ.
        switch (lParam)
        {
        case WM_LBUTTONDBLCLK:
            switch_tray_show();
            break;
        case WM_RBUTTONDOWN:
            POINT pt;
            GetCursorPos(&pt);//ȡ�������
            ::SetForegroundWindow(hwnd);//����ڲ˵��ⵥ������˵�����ʧ������
            //EnableMenuItem(hMenu, ID_SHOW, MF_GRAYED);//�ò˵��е�ĳһ����
            int xx = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hwnd, NULL);//��ʾ�˵�����ȡѡ��ID
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
    case WM_DESTROY://��������ʱ�����Ϣ.
        destroy_tray();
        break;
    default:
        /*
        *   ��ֹ��Explorer.exe �����Ժ󣬳�����ϵͳϵͳ�����е�ͼ�����ʧ
            ԭ��Explorer.exe �����������ؽ�ϵͳ����������ϵͳ������������ʱ�����ϵͳ��
            ����ע�����TaskbarCreated ��Ϣ�Ķ������ڷ���һ����Ϣ������ֻ��Ҫ��׽�����
            Ϣ�����ؽ�ϵͳ���̵�ͼ�꼴�ɡ�
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

    // �˴�ʹ��WS_EX_TOOLWINDOW ������������ʾ���������ϵĴ��ڳ���ť
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