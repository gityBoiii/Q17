#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <Windows.h>


using namespace std;

#pragma comment(lib, "WS2_32.lib")

char Buffer[1024] = { 0, };

unsigned WINAPI WorkThread(void* Arg)
{
	SOCKET ServerSocket;
	char Buffer[1024] = { 0, };

	ServerSocket = *(SOCKET*)Arg;

	while (true)
	{
		int RecvBytes = recv(ServerSocket, Buffer, sizeof(Buffer), 0);
		if (RecvBytes <= 0)
		{
			// 메세지 없으면 종료
			break;
		}
		else
		{
			//	메세지 표시 
			cout << Buffer << endl;
		}
	}

	return 0;
}


int main()
{
	//		소켓 초기화 
	WSAData wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_port = htons(4949);
	ServerSockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//		연결
	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	//		쓰레드 핸들 생성
	HANDLE WorkHandle = (HANDLE)_beginthreadex(nullptr, 0, WorkThread, (void*)&ServerSocket, 0, nullptr);

	//		메세지 받기
	int RecvLength = recv(ServerSocket, Buffer, sizeof(Buffer), 0);
	cout << "받은 내용 : " << Buffer << endl;
	
	while (true)
	{
		//		메세지 입력 
		char Message[1024] = { 0, };
		cin >> Message;
		//		보내기 
		int SentBytes = send(ServerSocket, Message, (int)strlen(Message) + 1, 0);
	}

	WSACleanup();
}