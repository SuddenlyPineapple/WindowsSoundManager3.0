#pragma once

#include "stdafx.h"
#include <iostream>
#include <Tchar.h>
#include <iomanip>
#include <conio.h>
#include <string>
#include <windows.h>
#include <cstddef>


extern int sessionCount;
extern bool sessionDisconnected;

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam);
LPTSTR get_process_name(DWORD process_id);


void gotoxy(int xpos, int ypos);
void setcursor(bool, DWORD);
void setCursorColor(int k);
std::wstring GetProcName(DWORD aPid);

