// just take this function as it is.

#include "stdafx.h"
#include "globFunctions.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <stdlib.h>
#include <WinBase.h>

extern int sessionCount = 0;
extern bool sessionDisconnected = false;

void gotoxy(int xpos, int ypos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xpos; scrn.Y = ypos;
	SetConsoleCursorPosition(hOuput, scrn);
}

void setcursor(bool visible, DWORD size) // set bool visible = 0 - invisible, bool visible = 1 - visible
{
	if (size == 0)
	{
		size = 20;	// default cursor size Changing to numbers from 1 to 20, decreases cursor width
	}
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = size;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(console, &lpCursor);
}


// you can loop k higher to see more color choices
void setCursorColor(int k)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// pick the colorattribute k you want
	SetConsoleTextAttribute(hConsole, k);
}

/* //Check Out Colors
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
// you can loop k higher to see more color choices
for(int k = 1; k < 255; k++)
{
// pick the colorattribute k you want
SetConsoleTextAttribute(hConsole, k);
cout << k << " I want to be nice today!" << endl;
}
*/


std::wstring GetProcName(DWORD aPid)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
	{
		//std::wcout << "can't get a process snapshot ";
		return 0;
	}

	for (BOOL bok = Process32First(processesSnapshot, &processInfo); bok; bok = Process32Next(processesSnapshot, &processInfo))
	{
		if (aPid == processInfo.th32ProcessID)
		{
			//std::wcout << "found running process: " << processInfo.szExeFile;
			CloseHandle(processesSnapshot);
			return processInfo.szExeFile;
		}

	}
	//std::wcout << "no process with given pid" << aPid;
	CloseHandle(processesSnapshot);
	return std::wstring();
}

/*
struct param_enum
{
	DWORD pid;
	HWND hWnd_out;
};

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	param_enum& param_data = *(param_enum*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (param_data.pid != process_id)
	{
		return TRUE;
	}
	param_data.hWnd_out = handle;

	return FALSE;
}

LPTSTR get_process_name(DWORD process_id)
{
	const int size = 255;
	TCHAR buffer[size] = { 0 };
	param_enum param_data;
	param_data.pid = process_id;
	param_data.hWnd_out = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&param_data);
	int textlen = GetWindowTextLengthW(param_data.hWnd_out) + 1;
	GetWindowTextW(param_data.hWnd_out, buffer, size);
	return buffer;
}
*/