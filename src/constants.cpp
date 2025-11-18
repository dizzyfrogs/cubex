#include "constants.h"

HMODULE hModule = GetModuleHandle(nullptr);
uintptr_t exeBaseAddress = (uintptr_t)GetModuleHandle(nullptr);

extern Player* localPlayerPtr = *(Player**)(exeBaseAddress + 0x0017E0A8);

uintptr_t entityListBase = exeBaseAddress + 0x18AC04;

extern float fov = *(float*)(exeBaseAddress + 0x18A7CC);
extern int* screenWidthPtr = (int*)(exeBaseAddress + 0x191EE0);
extern int* screenHeightPtr = (int*)(exeBaseAddress + 0x191EE4);
extern int numPlayers = *(int*)(exeBaseAddress + 0x18AC0C);

extern float* viewMatrix = (float*)(exeBaseAddress + 0x17DFD0);

void resetPointers() {
	localPlayerPtr = *(Player**)(exeBaseAddress + 0x0017E0A8);
	entityListBase = exeBaseAddress + 0x18AC04;
	fov = *(float*)(exeBaseAddress + 0x18A7CC);
	numPlayers = *(int*)(exeBaseAddress + 0x18AC0C);
}

_wglSwapBuffers originalSwapBuffers = (_wglSwapBuffers)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");
_SDL_SetRelativeMouseMode originalSetRelativeMouseMode = (_SDL_SetRelativeMouseMode)GetProcAddress(GetModuleHandle(L"SDL2.dll"), "SDL_SetRelativeMouseMode");