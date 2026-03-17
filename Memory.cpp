#include "Memory.h"
#include <tlhelp32.h>

// FIX: Buffer muss groß genug sein und cast muss korrekt sein
uintptr_t Memory::ReadMemory(uintptr_t address, size_t size) {
    BYTE buffer[256] = {0};
    HANDLE hProcess = GetCurrentProcess();
    
    if (hProcess && address) {
        ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, NULL);
    }
    
    // FIX: Richtige cast basierend auf size
    if (size == sizeof(uintptr_t)) {
        return *(uintptr_t*)buffer;
    } else if (size == sizeof(DWORD)) {
        return (uintptr_t)*(DWORD*)buffer;
    }
    return 0;
}

// FIX: Error handling hinzugefügt
void Memory::WriteMemory(uintptr_t address, const void* data, size_t size) {
    if (!address || !data) return;
    
    HANDLE hProcess = GetCurrentProcess();
    if (hProcess) {
        WriteProcessMemory(hProcess, (LPVOID)address, data, size, NULL);
    }
}

// FIX: Richtige Modul-Suche mit szModName statt szModPath
DWORD Memory::GetModuleBase(const char* moduleName) {
    MODULEENTRY32 me32 = {0};
    me32.dwSize = sizeof(MODULEENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    
    if (hSnap == INVALID_HANDLE_VALUE) return 0;
    
    if (Module32First(hSnap, &me32)) {
        do {
            // FIX: Vergleich mit szModName (Modulname) nicht szModPath (voller Pfad)
            if (!_stricmp(me32.szModule, moduleName)) {
                CloseHandle(hSnap);
                return (DWORD)me32.hModule;
            }
        } while (Module32Next(hSnap, &me32));
    }
    CloseHandle(hSnap);
    return 0;
}
