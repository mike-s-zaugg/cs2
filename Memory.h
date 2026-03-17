#pragma once
#include <Windows.h>

class Memory {
public:
    static uintptr_t ReadMemory(uintptr_t address, size_t size);
    static void WriteMemory(uintptr_t address, const void* data, size_t size);
    static DWORD GetModuleBase(const char* moduleName); // Korrekte Deklaration
};
