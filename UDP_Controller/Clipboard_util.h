#pragma once
#include <windows.h>
#include <string>

#pragma warning(disable:4996)//全部关掉

using namespace std;

bool CopyTextToClipboard(const char* text);
string GetClipboardText(char* buf);

// 读取剪贴板中的文本
string GetClipboardText(char* buf)
{
    OpenClipboard(nullptr);
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL)
        return "Not is CF_TEXT!";

    char* pszText = static_cast<char*>(GlobalLock(hData));
    strcpy(buf, pszText);

    GlobalUnlock(hData);
    CloseClipboard();
    return buf;
}

// 复制文本到剪贴板
bool CopyTextToClipboard(const char* text)
{
    int i = 0, j = 0;
    for (; i < 6; i++) {
        //打开剪切板
        j = OpenClipboard(NULL);

        //判断是否打开成功，如果打开失败则重新尝试5次
        if (j == 0 && i < 5)
            Sleep(60);
        if (j == 0 && i == 5)
            return false;
        if (j != 0) {
            i = 0;
            j = 0;
            break;
        }
    }
    //分配字节数，HGLOBAL表示一个内存块
    HGLOBAL hmem = GlobalAlloc(GHND, strlen(text) + 1);

    //锁定内存中指定的内存块，并返回一个地址值令其指向内存块的起始处
    char* pmem = (char*)GlobalLock(hmem);

    //清空剪切板并释放剪切板内数据的句柄
    EmptyClipboard();
    memcpy(pmem, text, strlen(text) + 1);
    SetClipboardData(CF_TEXT, hmem);        //写入数据
    CloseClipboard();                       //关闭剪切板
    GlobalFree(hmem);                       //释放内存块

    return true;     //返回成功
}