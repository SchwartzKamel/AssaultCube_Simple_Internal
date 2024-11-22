// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"
#include "proc.h"

DWORD WINAPI HackThread(HMODULE hModule)
{
    // Create console
    AllocConsole();
    FILE * f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "OG for a fee, stay sippin' fam\n";
    // Get module base
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    bool bHealth = false, bAmmo = false, bRecoil = false;

    // Hack loop
    while (true)
    {
        // Key input
        if (GetAsyncKeyState(VK_DELETE) & 1)
        {
            break;
        }
        if (GetAsyncKeyState(VK_END) & 1)
        {
            bHealth = !bHealth;
        }
        if (GetAsyncKeyState(VK_HOME) & 1)
        {
            bAmmo = !bAmmo;
        }
        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            bRecoil = !bRecoil;
            if (bRecoil)
            {
                // nop
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
            }

            else
            {
                // write back original instructions
                // 50 8D 4C 24 1C 51 8B CE FF D2 -> original stack setup and call
                mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x8B\xCE\xFF\xD2", 10);
            }
        }
    }

    // Continuous write/freeze
    uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);

    if (localPlayerPtr)
    {
        if (bHealth)
        {
            *(int*)(*localPlayerPtr + 0xF8) = 1337;
        }
        if (bAmmo)
        {
            /*
            uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
            int* ammo = (int*)ammoAddr;
            *ammo = 1337;
            */

            // or in one line
            *(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;
        }
        Sleep(5);
    }

    // Cleanup & eject
    fclose(f);
    FreeConsole;
    FreeLibraryAndExitThread(hModule, 0);
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
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

