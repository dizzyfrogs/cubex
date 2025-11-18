#include "input_utils.h"
#include <Windows.h>
#include <string>

std::string Utils::Input::getKeyName(int vk) {
	if (vk == VK_LBUTTON) return "Mouse1";
	if (vk == VK_RBUTTON) return "Mouse2";
	if (vk == VK_MBUTTON) return "Mouse3";
	if (vk == VK_XBUTTON1) return "Mouse4";
	if (vk == VK_XBUTTON2) return "Mouse5";
	if (vk >= VK_F1 && vk <= VK_F24) return "F" + std::to_string(vk - VK_F1 + 1);
	if (vk == VK_SPACE) return "Space";
	if (vk == VK_TAB) return "Tab";
	if (vk == VK_SHIFT) return "Shift";
	if (vk == VK_CONTROL) return "Ctrl";
	if (vk == VK_MENU) return "Alt";
	if (vk == VK_RETURN) return "Enter";
	if (vk == VK_BACK) return "Backspace";
	if (vk == VK_ESCAPE) return "Escape";
	if (vk == VK_INSERT) return "Insert";
	if (vk == VK_DELETE) return "Delete";
	if (vk == VK_HOME) return "Home";
	if (vk == VK_END) return "End";
	if (vk == VK_PRIOR) return "Page Up";
	if (vk == VK_NEXT) return "Page Down";
	if (vk == VK_UP) return "Up";
	if (vk == VK_DOWN) return "Down";
	if (vk == VK_LEFT) return "Left";
	if (vk == VK_RIGHT) return "Right";
	if (vk >= 'A' && vk <= 'Z') return std::string(1, (char)vk);
	if (vk >= '0' && vk <= '9') return std::string(1, (char)vk);
	if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) return "Numpad " + std::to_string(vk - VK_NUMPAD0);
	return "Key " + std::to_string(vk);
}

int Utils::Input::detectKeyPress() {
	for (int i = VK_LBUTTON; i <= VK_XBUTTON2; i++) {
		if (GetAsyncKeyState(i) & 0x8000) {
			return i;
		}
	}
	
	for (int i = 1; i < 256; i++) {
		// skip mouse buttons
		if (i >= VK_LBUTTON && i <= VK_XBUTTON2) continue;
		// skip some special keys
		if (i == VK_CAPITAL || i == VK_NUMLOCK || i == VK_SCROLL) continue;
		
		if (GetAsyncKeyState(i) & 0x8000) {
			return i;
		}
	}
	return 0;
}

