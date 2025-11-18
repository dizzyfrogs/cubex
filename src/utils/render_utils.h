#pragma once
#include "imgui/imgui.h"
#include <string>

namespace Utils {
	namespace Render {
		void drawCenteredText(std::string text, float x, float y);
		ImColor getHealthColor(float health, float maxHealth);
		void drawHealthBar(float centerX, float boxWidth, float headY, float feetY, float health);
	}
}

