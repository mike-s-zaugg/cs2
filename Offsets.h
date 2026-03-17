#pragma once
#include <cstdint>

// CS2 OFFSETS - Aus https://github.com/sezzyaep/CS2-OFFSETS
// Aktualisiert für neueste CS2 Version

namespace Offsets {
    // client.dll
    namespace Client {
        constexpr uintptr_t entity_list = 0x19A4C58;           // EntityList
        constexpr uintptr_t local_player_controller = 0x1B938A8;  // CCSPlayerController
        constexpr uintptr_t local_player_pawn = 0x1B7E828;      // C_CSPlayerPawn
    }

    // engine2.dll
    namespace Engine {
        constexpr uintptr_t window_width = 0x3E1E38;
        constexpr uintptr_t window_height = 0x3E1E3C;
    }

    // Matrix für 3D -> 2D Projektion
    namespace Matrix {
        constexpr uintptr_t view_matrix = 0x1A51C80;  // ViewMatrix3x4 (client.dll)
    }

    // Player Entity Offsets
    namespace Entity {
        constexpr uintptr_t next = 0x120;
        constexpr uintptr_t health = 0x328;          // m_iHealth
        constexpr uintptr_t team = 0x3C3;            // m_iTeamNum
        constexpr uintptr_t origin = 0x10C4;         // m_vOldOrigin
        constexpr uintptr_t velocity = 0x10D0;       // m_vecVelocity
        constexpr uintptr_t view_offset = 0x10D8;    // CameraOffset
        constexpr uintptr_t flags = 0x3F8;           // m_fFlags
        constexpr uintptr_t name = 0xC68;            // m_iszPlayerName (name offset)
        constexpr uintptr_t bone_matrix = 0x1100;    // m_BoneMatrix (Skeleton)
    }

    // View Angles / Camera
    namespace Camera {
        constexpr uintptr_t view_angles = 0x1A52280;  // ViewAngles (engine2.dll)
        constexpr uintptr_t eye_angles = 0x1020;      // m_angEyeAngles (entity)
    }

    // Weapon System
    namespace Weapon {
        constexpr uintptr_t weapon_list = 0x17D0;     // m_WeaponServices
        constexpr uintptr_t active_weapon = 0x20;     // ActiveWeapon
        constexpr uintptr_t clip_ammo = 0x3DC;        // m_iClipAmmo
        constexpr uintptr_t ammo = 0x3E0;             // m_iPrimaryAmmoType
    }

    // Buttons / Input
    namespace Input {
        constexpr uintptr_t button_code = 0x17A18;    // ButtonCode (from engine2.dll)
    }

    // Teams
    constexpr int TEAM_SPECTATOR = 1;
    constexpr int TEAM_TERRORIST = 2;
    constexpr int TEAM_CT = 3;

    // Flags
    constexpr uint32_t FL_ONGROUND = (1 << 0);       // On ground flag
    constexpr uint32_t FL_INWATER = (1 << 4);        // In water flag
}

// Matrix4x4 für View Matrix
struct Matrix3x4 {
    float matrix[3][4];
};

// Einfache WorldToScreen Funktion
inline bool WorldToScreen(const float* viewMatrix, float x, float y, float z, 
                         float& screenX, float& screenY, int width, int height) {
    float w = viewMatrix[12] * x + viewMatrix[13] * y + viewMatrix[14] * z + viewMatrix[15];

    if (w < 0.01f) return false;

    screenX = (viewMatrix[0] * x + viewMatrix[1] * y + viewMatrix[2] * z + viewMatrix[3]) / w;
    screenY = (viewMatrix[4] * x + viewMatrix[5] * y + viewMatrix[6] * z + viewMatrix[7]) / w;

    screenX = (width / 2.0f) + (screenX * width / 2.0f);
    screenY = (height / 2.0f) - (screenY * height / 2.0f);

    return true;
}
