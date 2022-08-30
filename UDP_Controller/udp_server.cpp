#include "udp_server.h"
#include "Clipboard_util.h"

#pragma warning(disable:4996)//ȫ���ص�

#define BUFFER_SIZE 1024


boolean UDP_SERVER_STARTED = false;

void open_server(const char* ip, int port)
{
	PVOID addr;
	WSADATA WSAData;
	char receBuf[BUFFER_SIZE];
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		printf("Initialize failed \n");
		exit(1);
	}
	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockServer == INVALID_SOCKET)
	{
		printf("Failed socket() \n");
		return;
	}
	SOCKADDR_IN addr_Server; //�������ĵ�ַ����Ϣ
	addr_Server.sin_family = AF_INET;
	addr_Server.sin_port = htons(port);
	addr_Server.sin_addr.S_un.S_addr = inet_addr(ip); //127.0.0.1Ϊ������IP��ַ
	if (bind(sockServer, (SOCKADDR*)&addr_Server, sizeof(addr_Server)) == SOCKET_ERROR)
	{
		//�������뱾�ص�ַ��
		printf("Failed socket() %d \n", WSAGetLastError());
		return;
	}
	SOCKADDR_IN addr_Clt;
	sockaddr_in sa;
	char str[INET_ADDRSTRLEN];

	int fromlen = sizeof(SOCKADDR);
	cout << "UDP Server is started" << endl;
	while (true)
	{
		string responce("Got it");
		UDP_SERVER_STARTED = true;
		int last = recvfrom(sockServer, receBuf, 1024, 0, (SOCKADDR*)&addr_Clt, &fromlen);
		cout << inet_ntoa(addr_Clt.sin_addr) << " : " << receBuf << endl;
		if (last > 0)
		{
			//�жϽ��յ��������Ƿ�Ϊ��
			receBuf[last] = '\0'; //���ַ������һ��'\0'����ʾ�����ˡ���Ȼ���������
			if (!strcmp(receBuf, "bye"))
			{
				cout << " Client exited..." << endl;
				closesocket(sockServer);
				return;
			}
			else if (!strcmp(receBuf, "shutdown"))
			{
				system("shutdown /p /f");
			}
			else
			{
				CopyTextToClipboard(receBuf);
			}
		}
		sendto(sockServer, responce.c_str(), responce.length(), 0, (SOCKADDR*)&addr_Clt, sizeof(SOCKADDR));
	}

	closesocket(sockServer);

	WSACleanup();
}

boolean udp_server_is_started()
{
	return UDP_SERVER_STARTED;
}