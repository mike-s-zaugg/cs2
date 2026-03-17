#include "Triggerbot.h"
#include <chrono>
#include <cmath>

#define OFFSET_VIEW_ANGLES      0x2314F98
#define OFFSET_LOCAL_PLAYER     0x22EF0B8
#define OFFSET_PAWN             0x2064AE0
#define OFFSET_WEAPON_INDEX     0x3F4

Triggerbot::Triggerbot(HMODULE hClient) : m_hClient(hClient) {}

void Triggerbot::Update() {
    if (!config.enabled) return;
    if (!m_hClient) return; // FIX: Null check

    if (GetAsyncKeyState(config.keyBind) & 0x8000) {
        auto now = std::chrono::high_resolution_clock::now();
        float currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count() / 1000.0f;

        if ((currentTime - lastFireTime) > (config.delay_ms / 1000.0f)) { // FIX: ms zu s konvertieren
            
            Vector localOrigin = GetLocalPlayerOrigin();
            Vector viewAngles = GetViewAngles();
            
            if (IsTargetInCrosshair(localOrigin)) { // FIX: localOrigin übergeben, nicht viewAngles
                FireWeapon();
                lastFireTime = currentTime;
            }
        }
    }
}

Vector Triggerbot::GetLocalPlayerOrigin() {
    if (!m_hClient) return {0, 0, 0}; // FIX: Null check
    
    uintptr_t localPawn = Memory::ReadMemory((uintptr_t)m_hClient + OFFSET_PAWN, sizeof(uintptr_t));
    if (!localPawn) return {0, 0, 0}; // FIX: Null check
    
    return *(Vector*)(localPawn + 0x134);
}

Vector Triggerbot::GetViewAngles() {
    Vector angles = {0, 0, 0};
    if (!m_hClient) return angles; // FIX: Null check
    
    uintptr_t viewAnglesPtr = Memory::ReadMemory((uintptr_t)m_hClient + OFFSET_VIEW_ANGLES, sizeof(uintptr_t));
    if (viewAnglesPtr) {
        ReadProcessMemory(GetCurrentProcess(), (LPCVOID)viewAnglesPtr, &angles, sizeof(Vector), NULL);
    }
    return angles;
}

bool Triggerbot::IsTargetInCrosshair(Vector viewOrigin) {
    if (!m_hClient) return false; // FIX: Null check
    
    uintptr_t entityList = Memory::ReadMemory((uintptr_t)m_hClient + 0x24AA0D8, sizeof(uintptr_t));
    if (!entityList) return false; // FIX: Null check
    
    for (int i = 1; i < 65; i++) {
        uintptr_t entityPtr = Memory::ReadMemory(entityList + (i * sizeof(uintptr_t)), sizeof(uintptr_t));
        
        if (!entityPtr) continue;

        int team = *(int*)(entityPtr + 0x10); 
        if (team == 2) continue; // Skip eigenes Team

        Vector targetPos = GetTargetBone(i, 0);
        
        // FIX: Richtige Distanzberechnung von Position zu Position
        float dist = std::sqrt(
            pow(targetPos.x - viewOrigin.x, 2) + 
            pow(targetPos.y - viewOrigin.y, 2) + 
            pow(targetPos.z - viewOrigin.z, 2)
        );

        if (dist < 10.0f) return true;
    }
    
    return false;
}

Vector Triggerbot::GetTargetBone(int entityId, int boneIndex) {
    if (!m_hClient) return {0, 0, 0}; // FIX: Null check
    
    uintptr_t entityList = Memory::ReadMemory((uintptr_t)m_hClient + 0x24AA0D8, sizeof(uintptr_t));
    if (!entityList) return {0, 0, 0}; // FIX: Null check
    
    uintptr_t entityPtr = Memory::ReadMemory(entityList + (entityId * sizeof(uintptr_t)), sizeof(uintptr_t));
    if (!entityPtr) return {0, 0, 0}; // FIX: Null check
    
    Vector origin = *(Vector*)(entityPtr + 0x134); 
    
    if (boneIndex == 0) return {origin.x, origin.y, origin.z + 75.0f}; // Head
    return origin;
}

void Triggerbot::FireWeapon() {
    if (!m_hClient) return; // FIX: Null check
    
    uintptr_t viewAnglesPtr = Memory::ReadMemory((uintptr_t)m_hClient + OFFSET_VIEW_ANGLES, sizeof(uintptr_t));
    if (!viewAnglesPtr) return; // FIX: Null check
    
    // FIX: Nur einen Float modifizieren, nicht zwei auslesen
    float* anglePtr = (float*)(viewAnglesPtr + 0x4);
    *anglePtr += 0.5f; 
}
