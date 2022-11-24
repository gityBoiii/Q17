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

////			채팅 보내기 
////		쓰레드
//unsigned WINAPI WorkThread(void* Arg)
//{
//	//		클라 소켓
//	SOCKET ClientSocket = *(SOCKET*)Arg;
//	char Buffer[1024] = { 0, };
//
//	while (true)
//	{
//		//		메세지 받기 
//		int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
//		//	없으면 
//		if (RecvLength <= 0)
//		{
//			// 소켓 지우기  
//			closesocket(ClientSocket);
//			EnterCriticalSection(&ServerCS);
//			UserList.erase(find(UserList.begin(), UserList.end(), ClientSocket));
//			LeaveCriticalSection(&ServerCS);
//			break;
//		}
//		else
//		{
//			//		유저수만큼 
//			for (int i = 0; i < UserList.size(); ++i)
//			{
//				//		메세지 보내기 
//				int SentLength = send(UserList[i], Buffer, (int)strlen(Buffer) + 1, 0);
//				//	없으면  
//				if (SentLength <= 0)
//				{
//					//	소켓 지우기 
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

//		쓰레드2
unsigned WINAPI CharactersThread(void* Arg)
{
	//		클라 소켓
	SOCKET ClientSocket = *(SOCKET*)Arg;
	char Buffer[1024] = { 0, };
	char Message[1024] = { 0, };

	while (true)
	{
		//			캐릭터 이름 찾기 
		for (int i = 0; i < size(CharacterList)+1; i++)
		{
			//	캐릭터 숫자 
			//cout << "캐릭터명은 : " << CharacterList[i] << endl;
			int CharacterNumber = NULL;
			stringstream ssInt(CharacterList[i].substr(9));
			ssInt >> CharacterNumber;
			//cout << "캐릭터 숫자는 : " << CharacterNumber << endl;
			//	캐릭터 출력
			if(CharacterNumber == (int)ClientSocket)
			{
				//	char형으로 변환
				strcpy_s(Message, CharacterList[i].c_str());
				
				//	메세지 보내기 
				cout << "플레이어 : " << CharacterList[i] << endl;
				send(ClientSocket, Message, (int)strlen(Message) + 1, 0);
			}
		}
		
		//		메세지 받기 
		int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		//	없으면 
		if (RecvLength <= 0)
		{
			// 소켓 지우기  
			closesocket(ClientSocket);
			EnterCriticalSection(&ServerCS);
			UserList.erase(find(UserList.begin(), UserList.end(), ClientSocket));
			LeaveCriticalSection(&ServerCS);
			break;
		}
		else
		{
			//		유저수만큼 
			for (int i = 0; i < UserList.size(); ++i)
			{
				//		메세지 보내기 
				int SentLength = send(UserList[i], Buffer, (int)strlen(Buffer) + 1, 0);
				//	없으면  
				if (SentLength <= 0)
				{
					//	소켓 지우기 
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
	//			초기화
	//		CriticalSection
	InitializeCriticalSection(&ServerCS);
	//		소켓
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
		//		연결
		SOCKADDR_IN ClientSockAddr;
		memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
		int ClientSockAddrLength = sizeof(ClientSockAddr);
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);

		cout << "connect : " << ClientSocket << endl;
		//	유저 추가 
		EnterCriticalSection(&ServerCS);
		UserList.push_back(ClientSocket);
		CharacterList.push_back("Character" + to_string(int(ClientSocket)));	// 캐릭터 명 생성 
		// cout << "캐릭터명은 : " << CharacterList[0] << endl;
		LeaveCriticalSection(&ServerCS);

		//	쓰레드 만들기 
		HANDLE CharactersHandle = (HANDLE)_beginthreadex(nullptr,
			0,
			CharactersThread,
			(void*)&ClientSocket,
			0,
			nullptr
		);
		
		////	쓰레드 만들기 
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