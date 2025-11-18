#pragma once
#include <Windows.h>
#include "genCode.h"

extern HMODULE hModule;
extern uintptr_t exeBaseAddress;

extern Player* localPlayerPtr;
extern uintptr_t entityListBase;
extern float fov;
extern int* screenWidthPtr;
extern int* screenHeightPtr;
extern int gameMode;
extern int numPlayers;
extern float* viewMatrix;

void resetPointers();


typedef BOOL(__stdcall* _wglSwapBuffers)(HDC hdc);
extern _wglSwapBuffers originalSwapBuffers;

typedef int(__fastcall* _SDL_SetRelativeMouseMode)(int mode);
extern _SDL_SetRelativeMouseMode originalSetRelativeMouseMode;