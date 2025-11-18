#include "menu.h"
#include "constants.h"
#include "settings.h"
#include "esp.h"
#include "GL/GL.h"
#include "GL/GLU.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"
#include <iostream>
#include <string>

const ImVec2 initWindowSize = ImVec2(700, 600);
bool showMenu = false;
bool initialized = false;
bool contextCreated = false;
HWND gameWindow;
HGLRC myContext;
HGLRC gameContext;
HDC gHDC;
WNDPROC originalWndProc = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK newWNDProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (showMenu) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return 0;
		switch (uMsg) {
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			return 0;
		}
	}
	if (originalWndProc) {
		return CallWindowProc(originalWndProc, hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Menu::toggleMenu()
{
	if (!initialized) {
		return; // Don't toggle if ImGui isn't initialized yet
	}
	showMenu = !showMenu;
	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureMouse = showMenu;
	io.WantCaptureKeyboard = showMenu;
	io.MouseDrawCursor = showMenu;
	if (originalSetRelativeMouseMode) {
		originalSetRelativeMouseMode(!showMenu);
	}
}

void Menu::init()
{
	ImGui::CreateContext();
	gameWindow = FindWindowA(NULL, "AssaultCube");
	if (!gameWindow) {
		std::cout << "Failed to find game window!" << std::endl;
		return;
	}
	originalWndProc = (WNDPROC)SetWindowLongPtr(gameWindow, GWLP_WNDPROC, (LONG_PTR)newWNDProc);
	if (!originalWndProc) {
		std::cout << "Failed to set window procedure!" << std::endl;
		return;
	}
	ImGui_ImplWin32_Init(gameWindow);
	ImGui_ImplOpenGL2_Init();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.Fonts->AddFontDefault();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	ImGui::SetNextWindowSize(initWindowSize, ImGuiCond_FirstUseEver);
	initialized = true;
	std::cout << "Menu initialized" << std::endl;
}

void Menu::startRender()
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void testingSettings() {
	if (!ImGui::BeginTabItem("Testing"))
		return;
	ImGui::Text("Hello, World!");
	if (ImGui::Button("TP up")) {
		if (localPlayerPtr) {
			localPlayerPtr->pos.z += 5;
		}
	}
	ImGui::EndTabItem();
}

void espSettings() {
	if (!ImGui::BeginTabItem("ESP"))
		return;
	ImGui::Checkbox("Enabled", &Settings::ESP::enabled);
	ImGui::Checkbox("Draw Team", &Settings::ESP::drawTeam);
	ImGui::ColorEdit4("Team Color", (float*)&Settings::ESP::teamColor->Value);
	ImGui::ColorEdit4("Enemy Color", (float*)&Settings::ESP::enemyColor->Value);
	ImGui::EndTabItem();
}

std::string getKeyName(int vk) {
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

int detectKeyPress() {
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

void aimSettings() {
	if (!ImGui::BeginTabItem("Aim"))
		return;
	ImGui::Checkbox("Enabled", &Settings::Aimbot::enabled);
	ImGui::Checkbox("Smoothing", &Settings::Aimbot::smoothing);
	ImGui::SliderFloat("Smoothing Amount", &Settings::Aimbot::smoothingAmount, 0.1f, 10.0f);
	ImGui::Checkbox("Check FOV", &Settings::Aimbot::checkInFov);
	ImGui::SliderFloat("FOV", &Settings::Aimbot::fov, 0.0f, 500.0f);
	ImGui::Checkbox("Draw FOV Circle", &Settings::Aimbot::drawFovCircle);
	
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	
	std::string buttonText = "AIM Key: " + getKeyName(Settings::Aimbot::aimKey);
	if (Settings::Aimbot::waitingForKeyBind) {
		buttonText = "Press any key...";
		int pressedKey = detectKeyPress();
		if (pressedKey != 0 && pressedKey != VK_INSERT) {
			Settings::Aimbot::aimKey = pressedKey;
			Settings::Aimbot::waitingForKeyBind = false;
		}
	}
	
	if (ImGui::Button(buttonText.c_str(), ImVec2(200, 0))) {
		Settings::Aimbot::waitingForKeyBind = true;
	}
	
	if (Settings::Aimbot::waitingForKeyBind) {
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			Settings::Aimbot::waitingForKeyBind = false;
		}
	}
	
	ImGui::EndTabItem();
}

void Menu::render()
{
	if (!showMenu)
		return;

	if (screenWidthPtr && screenHeightPtr) {
		ImVec2 centerPos = ImVec2(
			(float)(*screenWidthPtr) / 2.0f - initWindowSize.x / 2.0f,
			(float)(*screenHeightPtr) / 2.0f - initWindowSize.y / 2.0f
		);
		ImGui::SetNextWindowPos(centerPos, ImGuiCond_FirstUseEver);
	}
	ImGui::SetNextWindowSize(initWindowSize, ImGuiCond_FirstUseEver);
	
	ImGui::Begin("Menu", &showMenu);
	if (ImGui::BeginTabBar("Tabs")) {
		espSettings();
		aimSettings();
		testingSettings();
		ImGui::EndTabBar();
	}
	
	originalSetRelativeMouseMode(!showMenu);
	ImGui::End();
}

void setupContext(HDC& hdc) {
	myContext = wglCreateContext(hdc);

	wglMakeCurrent(hdc, myContext);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	contextCreated = true;
}

void Menu::endRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

BOOL __stdcall Menu::newSwapBuffers(HDC hdc)
{
	if (!initialized) {
		Menu::init();
		originalSwapBuffers(hdc);
	}

	gameContext = wglGetCurrentContext();
	if (!contextCreated)
		setupContext(hdc);
	if (myContext) {
		wglMakeCurrent(hdc, myContext);
		Menu::startRender();
		Menu::render();
		ESP::drawESP();
		ESP::aimbot();
		Menu::endRender();
	}
	
	if (gameContext) {
		wglMakeCurrent(hdc, gameContext);
	}
	return originalSwapBuffers(hdc);
}