// WindowsSoundManger3.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "globFunctions.h"
#include <numeric>
#include <future>
#include <thread>
#include <functional>

int main()
{
	setcursor(false, 0);
	setCursorColor(15);

	printf(R"EOF(
 __          ___           _                      _____                       _ 
 \ \        / (_)         | |                    / ____|                     | |
  \ \  /\  / / _ _ __   __| | _____      _____  | (___   ___  _   _ _ __   __| |
   \ \/  \/ / | | '_ \ / _` |/ _ \ \ /\ / / __|  \___ \ / _ \| | | | '_ \ / _` |
    \  /\  /  | | | | | (_| | (_) \ V  V /\__ \  ____) | (_) | |_| | | | | (_| |
     \/  \/   |_|_| |_|\__,_|\___/ \_/\_/ |___/ |_____/ \___/ \__,_|_| |_|\__,_|
                                                                                
                                                                                

  __  __                                     ____     ___  
 |  \/  |                                   |___ \   / _ \ 
 | \  / | __ _ _ __   __ _  __ _  ___ _ __    __) | | | | |
 | |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|  |__ <  | | | |
 | |  | | (_| | | | | (_| | (_| |  __/ |     ___) |_| |_| |
 |_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|    |____/(_)\___/ 
                            __/ |                          
                           |___/   

			by Wojciech Kasperski <wojciech.kasperski97@gmail.com>


Use UP/DOWN arrows to navigate
RIGHT/LEFT for Increase/Decrease volume
M is for Muting audio stream
ESC for exit application

Enjoy ;)
	)EOF");

	Sleep(4000);
	system("cls");
	
	MasterVolumeManipulator *masterVolume = new MasterVolumeManipulator(); 
	gotoxy(0, 0);
	std::cout << " -> ";

	gotoxy(0, 2);
	SessionManager *sessManager = new SessionManager();
	sessManager->Initialize();

	int menu_item = 0, x = 0;
	bool running = true;
	int i=0;

	std::thread([running, masterVolume, sessManager]() {
		while (running == true) {
			auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
			if (sessionDisconnected) {
				system("cls");
				masterVolume->updateMasterVolumeLevel();
				sessManager->Update();
				sessionDisconnected = false;
			}
			std::this_thread::sleep_until(x);
		}
	}).detach();

	std::thread([running, masterVolume, sessManager]() {
		auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
		system("cls");
		masterVolume->updateMasterVolumeLevel();
		sessManager->Update();
		sessionDisconnected = false;
		std::this_thread::sleep_until(x);
	}).detach();
	
	while (running) {
		
		if (sessionDisconnected) {
			system("cls");
			masterVolume->updateMasterVolumeLevel();
			sessManager->Update();
			sessionDisconnected = false;
		}
		

		system("pause>nul");// the >nul bit causes it the print no message

		if (GetAsyncKeyState(VK_DOWN)) //down button pressed
		{
			gotoxy(0, x); std::cout << "    ";
			if (x == sessionCount) {
				x = 0;
				menu_item = 0;
			}
			else {
				x++;
				menu_item++;
			}
			gotoxy(0, x); std::cout << " -> ";
			
			continue;

		}

		if (GetAsyncKeyState(VK_UP)) //up button pressed
		{
			gotoxy(0, x); std::cout << "    ";
			if (x == 0) {
				x = sessionCount;
				menu_item = sessionCount;
			}
			else {
				x--;
				menu_item--;
			}
			gotoxy(0, x); std::cout << " -> ";
			continue;
		}

		if (GetAsyncKeyState(VK_RIGHT)) //right button pressed
		{
			if (menu_item == 0) {
				masterVolume->setMasterVolumeLevel(masterVolume->getMasterVolumeLevel() + (float)1);
			}
			else if (menu_item > 0 && menu_item < sessionCount) {
				sessManager->changeVolumeLevel(menu_item - 1, sessManager->getVolumeLevel(menu_item - 1) + 1);
			}
			continue;
		}

		if (GetAsyncKeyState(VK_LEFT)) //right button pressed
		{
			if (menu_item == 0) {
				masterVolume->setMasterVolumeLevel(masterVolume->getMasterVolumeLevel() - (float)1);
			}
			else if (menu_item > 0 && menu_item < sessionCount) {
				sessManager->changeVolumeLevel(menu_item - 1, sessManager->getVolumeLevel(menu_item - 1) - 1);
			}
			continue;

		}

		if (GetAsyncKeyState(VK_RETURN)) { // Enter key pressed

			if(menu_item == 0) {
				gotoxy(0, 20);
				std::cout << "Input new volume Level (0-1000): ";
				float vLevel;
				setcursor(1, 1);
				std::cin >> vLevel;
				setcursor(0, 0);
				gotoxy(0, 20);
				std::cout << "                                       ";
				masterVolume->setMasterVolumeLevel(vLevel);
				continue;
			} 
			else if(menu_item > 0 && menu_item < sessionCount){
				gotoxy(0, 20);
				std::cout << "Input new volume Level (0-1000): ";
				float vLevel;
				setcursor(1, 1);
				std::cin >> vLevel;
				setcursor(0, 0);
				gotoxy(0, 20);
				std::cout << "                                            ";
				sessManager->changeVolumeLevel(menu_item - 1, vLevel);
				continue;
			}
			else {
				//gotoxy(20, 16);
				//std::cout << "The program has now terminated!!";
				running = false;
			}
			
		}

		if (GetAsyncKeyState(0x4D)) //M button pressed
		{
			if (menu_item == 0) {
				masterVolume->switchMute();
			}
			else if (menu_item > 0 && menu_item < sessionCount) {
				sessManager->switchMute(menu_item-1);
			}
			continue;
		}

		if (GetAsyncKeyState(VK_ESCAPE)) //Escape button pressed
		{
			running = false;
		}
	}
    return 0;
}

