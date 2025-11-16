// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include "constants.h"
#include "genCode.h"

using namespace std;

void hook() {

}

void console() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONIN$", "r", stdin);
    cout << "Console created!" << endl;
    while (true) {
        string input;
        cin >> input;
        if (input == "exit")
            break;
        if (input == "up")
            localPlayerPtr->pos.z += 5;
        if (input == "down")
            localPlayerPtr->pos.z -= 5;
        if (input == "print")
            cout << "Local Player Ptr: " << localPlayerPtr << endl;
    }
    FreeConsole();
}





BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)console, nullptr, 0, nullptr);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)hook, nullptr, 0, nullptr);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

