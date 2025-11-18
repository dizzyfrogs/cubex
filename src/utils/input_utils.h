#pragma once
#include <string>
#include <Windows.h>

namespace Utils {
	namespace Input {
		std::string getKeyName(int vk);
		int detectKeyPress();
	}
}

