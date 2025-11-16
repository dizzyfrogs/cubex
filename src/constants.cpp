#include "constants.h"

HMODULE hModule = GetModuleHandle(nullptr);
uintptr_t exeBaseAddress = (uintptr_t)GetModuleHandle(nullptr);

extern Player* localPlayerPtr = *(Player**)(exeBaseAddress + 0x0017E0A8);