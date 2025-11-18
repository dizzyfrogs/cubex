// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include "constants.h"
#include "genCode.h"
#include "esp.h"
#include "detours/detours.h"
#include "menu.h"

using namespace std;

void aimbot() {

}

void hook() {
    Sleep(1000);
    DisableThreadLibraryCalls(hModule);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)originalSwapBuffers, Menu::newSwapBuffers);
    DetourTransactionCommit();
    while (true) {
        resetPointers();
        Sleep(300);
        if (GetAsyncKeyState(VK_INSERT))
            Menu::toggleMenu();
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)hook, nullptr, 0, nullptr);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)aimbot, nullptr, 0, nullptr);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH: 
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

