// Memory.h
#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include <cstdint>

class Memory {
public:
    static uintptr_t ReadMemory(uintptr_t address, size_t size);
    static void WriteMemory(uintptr_t address, const void* data, size_t size);
    static uintptr_t GetModuleBase(const char* moduleName);  // FIX: uintptr_t statt DWORD
};
