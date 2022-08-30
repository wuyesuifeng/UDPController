#include "udp_server.h"
#include "Hide_util.h"
#include <string>
#include <iostream>
#include <thread>

using namespace std;

void main(int cnt, char** args) {
	if (cnt != 3) {
		cout << "Bad params" << endl;
		return;
	}

	HWND handle = GetConsoleHwnd();
	ShowWindow(handle, SW_HIDE);
	thread t1(ToTray);

	char* ip = args[1];
	int port = stoi(args[2]);
	thread t2(open_server, ip, port);
	
	string input;
	while (input != "exit") {
		cin >> input;
	}
	destroy_tray();
}