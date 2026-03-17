#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

class OffsetManager {
public:
    // Statische Offsets (fallback wenn keine Dynamic verfügbar)
    static void InitializeStaticOffsets();
    
    // Dynamic Offsets aus GitHub laden (optional für Updates)
    static bool LoadOffsetsFromGitHub();
    
    // Getter für Offsets
    static uintptr_t GetOffset(const std::string& name);
    
    // Manual Override
    static void SetOffset(const std::string& name, uintptr_t value);
    
private:
    static std::unordered_map<std::string, uintptr_t> s_offsets;
    static bool s_initialized;
};
