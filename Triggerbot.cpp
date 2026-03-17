#include "Triggerbot.h"
#include <chrono>

// Konstanten aus dem Prompt (Offsets)
#define OFFSET_VIEW_ANGLES      0x2314F98
#define OFFSET_LOCAL_PLAYER     0x22EF0B8
#define OFFSET_PAWN             0x2064AE0
#define OFFSET_WEAPON_INDEX     0x3F4

Triggerbot::Triggerbot(HMODULE hClient) : m_hClient(hClient) {}

void Triggerbot::Update() {
    if (!config.enabled) return;

    // Prüfen ob Maus-Button gedrückt ist
    if (GetAsyncKeyState(config.keyBind) & 0x8000) {
        auto now = std::chrono::high_resolution_clock::now();
        float currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count() / 1000.0f;

        // Delay-Check (verhindert zu schnelles Feuern)
        if ((currentTime - lastFireTime) > config.delay_ms) {
            
            Vector localOrigin = GetLocalPlayerOrigin();
            Vector viewAngles = GetViewAngles();
            
            // Prüfen ob Ziel im Fadenkreuz ist
            if (IsTargetInCrosshair(viewAngles)) {
                FireWeapon();
                lastFireTime = currentTime;
            }
        }
    }
}

Vector Triggerbot::GetLocalPlayerOrigin() {
    uintptr_t localPawn = Memory::ReadMemory(m_hClient + OFFSET_PAWN, sizeof(uintptr_t));
    return *(Vector*)(localPawn + 0x134); // Origin Offset (Beispielwert)
}

Vector Triggerbot::GetViewAngles() {
    Vector angles;
    uintptr_t viewAnglesPtr = Memory::ReadMemory(m_hClient + OFFSET_VIEW_ANGLES, sizeof(uintptr_t));
    
    ReadProcessMemory(GetCurrentProcess(), (LPCVOID)viewAnglesPtr, &angles, sizeof(Vector), NULL);
    return angles;
}

bool Triggerbot::IsTargetInCrosshair(Vector viewAngle) {
    // Vereinfachte Logik: Prüfen ob Ziel in der Blickrichtung liegt
    // In einem echten Cheat würde man Raycasting verwenden
    
    uintptr_t entityList = Memory::ReadMemory(m_hClient + 0x24AA0D8, sizeof(uintptr_t));
    
    for (int i = 1; i < 65; i++) {
        uintptr_t entityPtr = Memory::ReadMemory(entityList + (i * sizeof(uintptr_t)), sizeof(uintptr_t));
        
        if (!entityPtr) continue;

        // Team Check (nur Gegner)
        int team = *(int*)(entityPtr + 0x10); 
        if (team == 2) continue; 

        Vector targetPos = GetTargetBone(i, 0); // Kopf als Ziel
        
        float dist = std::sqrt(
            pow(targetPos.x - viewAngle.x, 2) + 
            pow(targetPos.y - viewAngle.y, 2) + 
            pow(targetPos.z - viewAngle.z, 2)
        );

        if (dist < 10.0f) return true; // Ziel ist nah genug im FOV
    }
    
    return false;
}

Vector Triggerbot::GetTargetBone(int entityId, int boneIndex) {
    uintptr_t entityList = Memory::ReadMemory(m_hClient + 0x24AA0D8, sizeof(uintptr_t));
    uintptr_t entityPtr = Memory::ReadMemory(entityList + (entityId * sizeof(uintptr_t)), sizeof(uintptr_t));
    
    Vector origin = *(Vector*)(entityPtr + 0x134); // Body Origin
    
    if (boneIndex == 0) return {origin.x, origin.y, origin.z + 75.0f}; // Head
    return origin;
}

void Triggerbot::FireWeapon() {
    uintptr_t viewAnglesPtr = Memory::ReadMemory(m_hClient + OFFSET_VIEW_ANGLES, sizeof(uintptr_t));
    
    // Simuliere Schuss durch ViewAngle-Änderung (External Silent Aim)
    float currentAngles[2];
    ReadProcessMemory(GetCurrentProcess(), (LPCVOID)viewAnglesPtr, &currentAngles, 8, NULL);
    
    // Minimaler "Punch" für realistischen Effekt
    *(float*)(viewAnglesPtr + 0x4) += 0.5f; 
}
