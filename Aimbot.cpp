#include "Aimbot.h"
#include <cmath>
#include <algorithm>

// Konstanten aus dem Prompt (Offsets)
#define OFFSET_ENTITY_LIST      0x24AA0D8
#define OFFSET_VIEW_MATRIX      0x230ADE0
#define OFFSET_LOCAL_PLAYER     0x22EF0B8 // Controller
#define OFFSET_PAWN             0x2064AE0 // Pawn (Position)
#define OFFSET_VIEW_ANGLES      0x2314F98

Aimbot::Aimbot(HMODULE hClient, HMODULE hEngine) 
    : m_hClient(hClient), m_hEngine(hEngine) {}

void Aimbot::Update() {
    if (!config.enabled) return;

    // 1. Hole lokale Spielerdaten (Position & ViewAngles)
    uintptr_t localPlayerPtr = Memory::ReadMemory(m_hClient + OFFSET_LOCAL_PLAYER, sizeof(uintptr_t));
    Vector localOrigin = GetLocalPlayerOrigin();
    
    // 2. Hole ViewMatrix für Projektion
    Matrix3x4 viewMatrix = GetViewMatrix();

    // 3. Suche besten Gegner (Entity Loop)
    uintptr_t entityList = Memory::ReadMemory(m_hClient + OFFSET_ENTITY_LIST, sizeof(uintptr_t));
    Vector bestTarget = {0, 0, 0};
    float minDist = config.fov; 
    bool foundTarget = false;

    for (int i = 1; i < 65; i++) { // CS2 Max Players usually around 64
        uintptr_t entityPtr = Memory::ReadMemory(entityList + (i * sizeof(uintptr_t)), sizeof(uintptr_t));
        
        if (!entityPtr) continue;

        // Team Check (Verhindert Aim auf Mitspieler)
        int team = *(int*)(entityPtr + 0x10); 
        if (team == 2) continue; // CT Team (Annahme: Spieler ist T oder umgekehrt)

        // Hole Bone Position (Vereinfacht für External Cheat via Origin + Offset)
        Vector targetPos = GetTargetBone(i, config.targetBone);
        
        // Distanz berechnen (FOV Check)
        float dist = std::sqrt(
            pow(targetPos.x - localOrigin.x, 2) + 
            pow(targetPos.y - localOrigin.y, 2) + 
            pow(targetPos.z - localOrigin.z, 2)
        );

        if (dist < minDist && dist > 5.0f) { // Distanz-Filter gegen "Nackten" Aim
            bestTarget = targetPos;
            foundTarget = true;
            break; // Erster Treffer reicht für Performance
        }
    }

    if (foundTarget) {
        Vector angle = CalculateAngle(localOrigin, bestTarget);
        
        // 4. Silent Aim anwenden
        ApplySilentAim(angle);
        
        // 5. RCS (Recoil Control System)
        if (config.rcsEnabled) {
            ApplyRCS();
        }

        // Optional: Auto-Trigger (wenn Taste gedrückt oder immer aktiv)
        // if (GetAsyncKeyState(VK_LBUTTON)) FireWeapon(); 
    }
}

Vector Aimbot::GetLocalPlayerOrigin() {
    uintptr_t localPawn = Memory::ReadMemory(m_hClient + OFFSET_PAWN, sizeof(uintptr_t));
    return *(Vector*)(localPawn + 0x134); // Origin Offset (Beispielwert)
}

Matrix3x4 Aimbot::GetViewMatrix() {
    Matrix3x4 matrix;
    uintptr_t viewMatPtr = Memory::ReadMemory(m_hClient + OFFSET_VIEW_MATRIX, sizeof(uintptr_t));
    
    // ViewMatrix ist oft ein Array von 12 Floats (3x4)
    memcpy(&matrix.matrix, (float*)(viewMatPtr), sizeof(matrix.matrix));
    return matrix;
}

Vector Aimbot::GetTargetBone(int entityId, int boneIndex) {
    uintptr_t entityList = Memory::ReadMemory(m_hClient + OFFSET_ENTITY_LIST, sizeof(uintptr_t));
    uintptr_t entityPtr = Memory::ReadMemory(entityList + (entityId * sizeof(uintptr_t)), sizeof(uintptr_t));
    
    // Bone Array Offset (Variiert je nach Patch)
    uintptr_t boneArray = entityPtr + 0x268; 
    Vector origin = *(Vector*)(entityPtr + 0x134); // Body Origin
    
    // Vereinfachte Skelett-Logik: Wir nehmen den Körper und addieren Offset für Kopf/Hals
    if (boneIndex == 0) return {origin.x, origin.y, origin.z + 75.0f}; // Head
    if (boneIndex == 1) return {origin.x, origin.y, origin.z + 45.0f}; // Neck
    return origin; // Body
}

Vector Aimbot::CalculateAngle(Vector src, Vector dst) {
    Vector delta = {dst.x - src.x, dst.y - src.y, dst.z - src.z};
    float hypot = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    
    Vector angles;
    angles.x = std::atan2(-delta.z, hypot) * (180.0f / 3.14159265f); // Pitch
    angles.y = std::atan2(delta.y, delta.x) * (180.0f / 3.14159265f); // Yaw
    
    return angles;
}

void Aimbot::ApplySilentAim(Vector angle) {
    uintptr_t viewAnglesPtr = Memory::ReadMemory(m_hClient + OFFSET_VIEW_ANGLES, sizeof(uintptr_t));
    
    // Schreibe Winkel direkt in den Speicher (External Silent Aim)
    float currentAngles[2];
    ReadProcessMemory(GetCurrentProcess(), (LPCVOID)viewAnglesPtr, &currentAngles, 8, NULL);
    
    // Nur Yaw und Pitch ändern, damit der Gegner es nicht sieht
    *(float*)(viewAnglesPtr + 0x4) = angle.y; 
    *(float*)(viewAnglesPtr + 0x0) = angle.x;
}

void Aimbot::ApplyRCS() {
    // RCS Logik: Basierend auf aktueller Waffe und Feuermodus
    // In externen Cheats oft durch "ViewPunch" Kompensation gelöst
    uintptr_t viewAnglesPtr = Memory::ReadMemory(m_hClient + OFFSET_VIEW_ANGLES, sizeof(uintptr_t));
    
    float recoilScale = 2.0f; // Beispielwert für Rückstoßreduktion
    
    // Hier müsste man die aktuelle Waffe auslesen (dwWeaponIndex)
    // Und dann den Recoil-Wert der Waffe abziehen
}
