#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <Windows.h>
#include <vector>
#include <sstream>


using namespace std;

#pragma comment(lib, "WS2_32.lib")


vector<SOCKET> UserList;
vector<string> CharacterList;

CRITICAL_SECTION ServerCS;

////			ä�� ������ 
////		������
//unsigned WINAPI WorkThread(void* Arg)
//{
//	//		Ŭ�� ����
//	SOCKET ClientSocket = *(SOCKET*)Arg;
//	char Buffer[1024] = { 0, };
//
//	while (true)
//	{
//		//		�޼��� �ޱ� 
//		int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
//		//	������ 
//		if (RecvLength <= 0)
//		{
//			// ���� �����  
//			closesocket(ClientSocket);
//			EnterCriticalSection(&ServerCS);
//			UserList.erase(find(UserList.begin(), UserList.end(), ClientSocket));
//			LeaveCriticalSection(&ServerCS);
//			break;
//		}
//		else
//		{
//			//		��������ŭ 
//			for (int i = 0; i < UserList.size(); ++i)
//			{
//				//		�޼��� ������ 
//				int SentLength = send(UserList[i], Buffer, (int)strlen(Buffer) + 1, 0);
//				//	������  
//				if (SentLength <= 0)
//				{
//					//	���� ����� 
//					closesocket(ClientSocket);
//					EnterCriticalSection(&ServerCS);
//					UserList.erase(find(UserList.begin(), UserList.end(), ClientSocket));
//					LeaveCriticalSection(&ServerCS);
//					break;
//				}
//			}
//		}
//	}
//
//	return 0;
//}

//		������2
unsigned WINAPI CharactersThread(void* Arg)
{
	//		Ŭ�� ����
	SOCKET ClientSocket = *(SOCKET*)Arg;
	char Buffer[1024] = { 0, };
	char Message[1024] = { 0, };

	while (true)
	{
		//			ĳ���� �̸� ã�� 
		for (int i = 0; i < size(CharacterList)+1; i++)
		{
			//	ĳ���� ���� 
			//cout << "ĳ���͸��� : " << CharacterList[i] << endl;
			int CharacterNumber = NULL;
			stringstream ssInt(CharacterList[i].substr(9));
			ssInt >> CharacterNumber;
			//cout << "ĳ���� ���ڴ� : " << CharacterNumber << endl;
			//	ĳ���� ���
			if(CharacterNumber == (int)ClientSocket)
			{
				//	char������ ��ȯ
				strcpy_s(Message, CharacterList[i].c_str());
				
				//	�޼��� ������ 
				cout << "�÷��̾� : " << CharacterList[i] << endl;
				send(ClientSocket, Message, (int)strlen(Message) + 1, 0);
			}
		}
		
		//		�޼��� �ޱ� 
		int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		//	������ 
		if (RecvLength <= 0)
		{
			// ���� �����  
			closesocket(ClientSocket);
			EnterCriticalSection(&ServerCS);
			UserList.erase(find(UserList.begin(), UserList.end(), ClientSocket));
			LeaveCriticalSection(&ServerCS);
			break;
		}
		else
		{
			//		��������ŭ 
			for (int i = 0; i < UserList.size(); ++i)
			{
				//		�޼��� ������ 
				int SentLength = send(UserList[i], Buffer, (int)strlen(Buffer) + 1, 0);
				//	������  
				if (SentLength <= 0)
				{
					//	���� ����� 
					closesocket(ClientSocket);
					EnterCriticalSection(&ServerCS);
					UserList.erase(find(UserList.begin(), UserList.end(), ClientSocket));
					LeaveCriticalSection(&ServerCS);
					break;
				}
			}
		}
	}

	return 0;
}


int main()
{
	//			�ʱ�ȭ
	//		CriticalSection
	InitializeCriticalSection(&ServerCS);
	//		����
	WSAData wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ListenSocketAddr;
	memset(&ListenSocketAddr, 0, sizeof(ListenSocketAddr));
	ListenSocketAddr.sin_family = PF_INET;
	ListenSocketAddr.sin_port = htons(4949);
	ListenSocketAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(ListenSocket, (SOCKADDR*)&ListenSocketAddr, sizeof(ListenSocketAddr));

	listen(ListenSocket, 0);

	while (true)
	{
		//		����
		SOCKADDR_IN ClientSockAddr;
		memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
		int ClientSockAddrLength = sizeof(ClientSockAddr);
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);

		cout << "connect : " << ClientSocket << endl;
		//	���� �߰� 
		EnterCriticalSection(&ServerCS);
		UserList.push_back(ClientSocket);
		CharacterList.push_back("Character" + to_string(int(ClientSocket)));	// ĳ���� �� ���� 
		// cout << "ĳ���͸��� : " << CharacterList[0] << endl;
		LeaveCriticalSection(&ServerCS);

		//	������ ����� 
		HANDLE CharactersHandle = (HANDLE)_beginthreadex(nullptr,
			0,
			CharactersThread,
			(void*)&ClientSocket,
			0,
			nullptr
		);
		
		////	������ ����� 
		//HANDLE WorkHandle = (HANDLE)_beginthreadex(nullptr,
		//	0,
		//	WorkThread,
		//	(void*)&ClientSocket,
		//	0,
		//	nullptr
		//);
	}


	closesocket(ListenSocket);

	DeleteCriticalSection(&ServerCS);


	WSACleanup();
}