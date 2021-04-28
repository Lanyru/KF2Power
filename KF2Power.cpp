#include <iostream>
#include <Windows.h>
#include <vector>
#include <tchar.h> // _tcscmp
#include <TlHelp32.h>


    std::uintptr_t GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    std::uintptr_t dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (std::uintptr_t)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}


    int main(int argc_, char** argv_)
    {
        HWND hGameWindow = FindWindow(NULL, "Killing Floor 2 (64-bit, DX11) v1112");
        if (hGameWindow == NULL) {
            std::cout << "Start the game!\n";
            return 1;
        }
        DWORD pID = NULL; // ID of our Game
        GetWindowThreadProcessId(hGameWindow, &pID);
        HANDLE processHandle = NULL;
        processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
        if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL) { // error handling
            std::cout << "Failed to open process\n";
            return 2;
        }

        char gameName[] = "KFGame.exe";
        std::uintptr_t gameBaseAddress = GetModuleBaseAddress(_T(gameName), pID);
        std::cout << "Game: " << gameName << "\nID: " << pID << '\n';
        DWORD offsetGameToBaseAdress = 0x0220A068;
        std::vector<std::uintptr_t> pointsOffsets{ 0x420,0x40,0xD00,0x40,0x49C,0x244,0x5FC };
        std::uintptr_t baseAddress = NULL;
        //Get value at gamebase+offset -> store it in baseAddress
        ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAdress), &baseAddress, sizeof(baseAddress), NULL);
        std::cout << "debugginfo: baseaddress = " << std::hex << baseAddress << '\n';
        std::uintptr_t pointsAddress = baseAddress; //the Adress we need -> change now while going through offsets
        for (int i = 0; i < pointsOffsets.size() - 1; i++) // -1 because we dont want the value at the last offset
        {
            ReadProcessMemory(processHandle, (LPVOID)(pointsAddress + pointsOffsets.at(i)), &pointsAddress, sizeof(pointsAddress), NULL);
            std::cout << "debugginfo: Value at offset = " << std::hex << pointsAddress << '\n';
        }
        pointsAddress += pointsOffsets.at(pointsOffsets.size() - 1); //Add Last offset -> done!!

        //"MENU"
        std::cout << "Sirumal's KF2 Hack\n";
        std::cout << "Press Numpad 0 to EXIT\n";
        std::cout << "Press Numpad 1 to set Bullets\n";
        while (true)
        {
            Sleep(50);
            if (GetAsyncKeyState(VK_NUMPAD0))
            { // Exit
                return 0;
            }
            if (GetAsyncKeyState(VK_NUMPAD1))
            {//Mouseposition
                std::cout << "How many bullets do you want?\n";
                int newPoints = 0;
                std::cin >> newPoints;
                WriteProcessMemory(processHandle, (LPVOID)(pointsAddress), &newPoints, 4, 0);
            }
        }
    }
