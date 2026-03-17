#include "OffsetManager.h"
#include <iostream>

std::unordered_map<std::string, uintptr_t> OffsetManager::s_offsets;
bool OffsetManager::s_initialized = false;

void OffsetManager::InitializeStaticOffsets() {
    if (s_initialized) return;

    // Client.dll Offsets
    s_offsets["entity_list"] = 0x19A4C58;
    s_offsets["local_player_controller"] = 0x1B938A8;
    s_offsets["local_player_pawn"] = 0x1B7E828;
    s_offsets["view_matrix"] = 0x1A51C80;

    // Engine2.dll Offsets
    s_offsets["window_width"] = 0x3E1E38;
    s_offsets["window_height"] = 0x3E1E3C;
    s_offsets["view_angles"] = 0x1A52280;

    // Entity Offsets
    s_offsets["entity_health"] = 0x328;
    s_offsets["entity_team"] = 0x3C3;
    s_offsets["entity_origin"] = 0x10C4;
    s_offsets["entity_velocity"] = 0x10D0;
    s_offsets["entity_name"] = 0xC68;
    s_offsets["entity_bone_matrix"] = 0x1100;
    s_offsets["entity_flags"] = 0x3F8;

    // Weapon Offsets
    s_offsets["weapon_list"] = 0x17D0;
    s_offsets["weapon_active"] = 0x20;
    s_offsets["weapon_clip_ammo"] = 0x3DC;

    s_initialized = true;
    std::cout << "[OffsetManager] " << s_offsets.size() << " offsets geladen" << std::endl;
}

bool OffsetManager::LoadOffsetsFromGitHub() {
    // TODO: Implement HTTP request zu GitHub
    // Würde offsets.json von sezzyaep/CS2-OFFSETS laden
    // Für jetzt: static offsets reichen
    return true;
}

uintptr_t OffsetManager::GetOffset(const std::string& name) {
    if (!s_initialized) {
        InitializeStaticOffsets();
    }

    auto it = s_offsets.find(name);
    if (it == s_offsets.end()) {
        std::cerr << "[ERROR] Offset nicht gefunden: " << name << std::endl;
        return 0;
    }

    return it->second;
}

void OffsetManager::SetOffset(const std::string& name, uintptr_t value) {
    if (!s_initialized) {
        InitializeStaticOffsets();
    }

    s_offsets[name] = value;
    std::cout << "[OffsetManager] Offset gesetzt: " << name << " = 0x" << std::hex << value << std::dec << std::endl;
}
