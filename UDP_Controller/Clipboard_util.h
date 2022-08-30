#pragma once
#include <windows.h>
#include <string>

#pragma warning(disable:4996)//ȫ���ص�

using namespace std;

bool CopyTextToClipboard(const char* text);
string GetClipboardText(char* buf);

// ��ȡ�������е��ı�
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

// �����ı���������
bool CopyTextToClipboard(const char* text)
{
    int i = 0, j = 0;
    for (; i < 6; i++) {
        //�򿪼��а�
        j = OpenClipboard(NULL);

        //�ж��Ƿ�򿪳ɹ��������ʧ�������³���5��
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
    //�����ֽ�����HGLOBAL��ʾһ���ڴ��
    HGLOBAL hmem = GlobalAlloc(GHND, strlen(text) + 1);

    //�����ڴ���ָ�����ڴ�飬������һ����ֵַ����ָ���ڴ�����ʼ��
    char* pmem = (char*)GlobalLock(hmem);

    //��ռ��а岢�ͷż��а������ݵľ��
    EmptyClipboard();
    memcpy(pmem, text, strlen(text) + 1);
    SetClipboardData(CF_TEXT, hmem);        //д������
    CloseClipboard();                       //�رռ��а�
    GlobalFree(hmem);                       //�ͷ��ڴ��

    return true;     //���سɹ�
}