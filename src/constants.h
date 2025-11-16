#pragma once
#include <Windows.h>
#include "genCode.h"

extern HMODULE hModule;
extern uintptr_t exeBaseAddress;

extern Player* localPlayerPtr;
extern uintptr_t entityListBase;
extern float fov;
extern int numPlayers;

void resetPointers();