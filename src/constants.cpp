#include "constants.h"

HMODULE hModule = GetModuleHandle(nullptr);
uintptr_t exeBaseAddress = (uintptr_t)GetModuleHandle(nullptr);

extern Player* localPlayerPtr = *(Player**)(exeBaseAddress + 0x0017E0A8);

uintptr_t entityListBase = exeBaseAddress + 0x18AC04;

extern int numPlayers = *(int*)(exeBaseAddress + 0x18AC0C);