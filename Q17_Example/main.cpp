#include <iostream>
#include <process.h>
#include <Windows.h>

#pragma comment(lib, "winmm.lib")

using namespace std;

int Gold = 0;

HANDLE MutexHandle;
CRITICAL_SECTION CS;

unsigned WINAPI WorkThread(void* Arg)
{
	for (int i = 0; i < 10000; ++i)
	{
		WaitForSingleObject(MutexHandle, INFINITE);
		Gold++;
		ReleaseMutex(MutexHandle);
	}

	cout << Gold << endl;

	return 0;
}

unsigned WINAPI WorkThread2(void* Arg)
{
	for (int i = 0; i < 10000; ++i)
	{
		EnterCriticalSection(&CS);
		Gold++;
		LeaveCriticalSection(&CS);
	}

	cout << Gold << endl;

	return 0;
}



int main()
{
	WCHAR MultexName[] = L"이건 하나만";
	MutexHandle = CreateMutexA(0, false, "Hello");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		printf("이미 실행중입니다.");
		return -1;
	}

	Sleep(3000);

	for (;;);

	CloseHandle(MutexHandle);

	return 0;
}


//
//	MutexHandle = CreateMutex(nullptr, false, nullptr);
//	InitializeCriticalSection(&CS);
//
//	int StartTime = timeGetTime();
//	HANDLE ThreadHandles[2];
//	ThreadHandles[0] = (HANDLE)_beginthreadex(0, 0, WorkThread, 0, 0, 0);
//	ThreadHandles[1] = (HANDLE)_beginthreadex(0, 0, WorkThread, 0, 0, 0);
//
//	WaitForMultipleObjects(2, ThreadHandles, true, INFINITE);
//
//	cout << timeGetTime() - StartTime << endl;
//
//	StartTime = timeGetTime();
//	ThreadHandles[0] = (HANDLE)_beginthreadex(0, 0, WorkThread2, 0, 0, 0);
//	ThreadHandles[1] = (HANDLE)_beginthreadex(0, 0, WorkThread2, 0, 0, 0);
//
//	WaitForMultipleObjects(2, ThreadHandles, true, INFINITE);
//
//	cout << timeGetTime() - StartTime << endl;
//
//	DeleteCriticalSection(&CS);
//
//	CloseHandle(MutexHandle);
//}