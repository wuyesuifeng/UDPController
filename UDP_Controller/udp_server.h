#include<winsock2.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<ws2tcpip.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

void open_server(const char* ip, int port);

boolean udp_server_is_started();
