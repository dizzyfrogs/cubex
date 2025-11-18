#include "render_utils.h"
#include "imgui/imgui.h"

void Utils::Render::drawCenteredText(std::string text, float x, float y) {
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x - textWidth / 2, y), IM_COL32(255, 255, 255, 255), text.c_str());
}

ImColor Utils::Render::getHealthColor(float health, float maxHealth) {
	float healthPercent = health / maxHealth;
	
	if (healthPercent > 0.5f) {
		float t = (healthPercent - 0.5f) / 0.5f;
		int r = (int)(255 * (1.0f - t));
		int g = 255;
		int b = 0;
		return ImColor(r, g, b, 255);
	} else {
		float t = healthPercent / 0.5f;
		int r = 255;
		int g = (int)(255 * t);
		int b = 0;
		return ImColor(r, g, b, 255);
	}
}

void Utils::Render::drawHealthBar(float centerX, float boxWidth, float headY, float feetY, float health) {
	float barWidth = 3.0f;
	float barOffset = 5.0f;
	float barLeftX = centerX - boxWidth - barOffset - barWidth;
	float barRightX = centerX - boxWidth - barOffset;
	
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(barLeftX, headY), ImVec2(barRightX, feetY), IM_COL32(1, 1, 1, 255), 0, 0, 1.0f);
	
	float healthPercent = health / 100.0f;
	float filledHeight = (feetY - headY) * healthPercent;
	float filledTopY = feetY - filledHeight;
	
	ImColor healthColor = getHealthColor(health, 100.0f);
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(barLeftX, filledTopY), ImVec2(barRightX, feetY), healthColor);
}

