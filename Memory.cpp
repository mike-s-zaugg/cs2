#include "Memory.h"

uintptr_t Memory::ReadMemory(uintptr_t address, size_t size) {
    BYTE buffer[256];
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
    
    if (hProcess) {
        ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, NULL);
        CloseHandle(hProcess);
    }
    return *(uintptr_t)buffer;
}

void Memory::WriteMemory(uintptr_t address, const void* data, size_t size) {
    HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
    
    if (hProcess) {
        WriteProcessMemory(hProcess, (LPVOID)address, data, size, NULL);
        CloseHandle(hProcess);
    }
}

DWORD Memory::GetModuleBase(const char* moduleName) {
    MODULEENTRY32 me32 = {};
    me32.dwSize = sizeof(MODULEENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    
    if (hSnap != INVALID_HANDLE_VALUE) {
        if (Module32First(hSnap, &me32)) {
            do {
                if (!_stricmp(me32.szModPath, moduleName)) return (DWORD)me32.hModule;
            } while (Module32Next(hSnap, &me32));
        }
    }
    CloseHandle(hSnap);
    return 0;
}
