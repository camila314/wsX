// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Hooks.h"

DWORD WINAPI osXbotSwag(void* hmod) {
    AllocConsole();
    SetConsoleTitleA("osXbot debug");
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    setupAddresses();
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0x1000, osXbotSwag, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

