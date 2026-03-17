#include "Aimbot.h"
#include <cmath>
#include <algorithm>

#define OFFSET_ENTITY_LIST      0x24AA0D8
#define OFFSET_VIEW_MATRIX      0x230ADE0
#define OFFSET_LOCAL_PLAYER     0x22EF0B8
#define OFFSET_PAWN             0x2064AE0
#define OFFSET_VIEW_ANGLES      0x2314F98

Aimbot::Aimbot(HMODULE hClient, HMODULE hEngine) 
    : m_hClient(hClient), m_hEngine(hEngine) {}

void Aimbot::Update() {
    if (!config.enabled) return;

    uintptr_t localPlayerPtr = Memory::ReadMemory(m_hClient + OFFSET_LOCAL_PLAYER, sizeof(uintptr_t));
    Vector localOrigin = GetLocalPlayerOrigin();
    
    Matrix3x4 viewMatrix = GetViewMatrix();

    uintptr_t entityList = Memory::ReadMemory(m_hClient + OFFSET_ENTITY_LIST, sizeof(uintptr_t));
    Vector bestTarget = {0, 0, 0};
    float minDist = config.fov; 
    bool foundTarget = false;

    for (int i = 1; i < 65; i++) {
        uintptr_t entityPtr = Memory::ReadMemory(entityList + (i * sizeof(uintptr_t)), sizeof(uintptr_t));
        
        if (!entityPtr) continue;

        int team = *(int*)(entityPtr + 0x10); 
        if (team == 2) continue; 

        Vector targetPos = GetTargetBone(i, config.targetBone);
        
        float dist = std::sqrt(
            pow(targetPos.x - localOrigin.x, 2) + 
            pow(targetPos.y - localOrigin.y, 2) + 
            pow(targetPos.z - localOrigin.z, 2)
        );

        if (dist < minDist && dist > 5.0f) {
            bestTarget = targetPos;
            foundTarget = true;
            break;
        }
    }

    if (foundTarget) {
        Vector angle = CalculateAngle(localOrigin, bestTarget);
        ApplySilentAim(angle);
        
        if (config.rcsEnabled) {
            ApplyRCS();
        }
    }
}

Vector Aimbot::GetLocalPlayerOrigin() {
    uintptr_t localPawn = Memory::ReadMemory(m_hClient + OFFSET_PAWN, sizeof(uintptr_t));
    return *(Vector*)(localPawn + 0x134);
}

Matrix3x4 Aimbot::GetViewMatrix() {
    Matrix3x4 matrix;
    uintptr_t viewMatPtr = Memory::ReadMemory(m_hClient + OFFSET_VIEW_MATRIX, sizeof(uintptr_t));
    
    memcpy(&matrix.matrix, (float*)(viewMatPtr), sizeof(matrix.matrix));
    return matrix;
}

Vector Aimbot::GetTargetBone(int entityId, int boneIndex) {
    uintptr_t entityList = Memory::ReadMemory(m_hClient + OFFSET_ENTITY_LIST, sizeof(uintptr_t));
    uintptr_t entityPtr = Memory::ReadMemory(entityList + (entityId * sizeof(uintptr_t)), sizeof(uintptr_t));
    
    Vector origin = *(Vector*)(entityPtr + 0x134); 
    
    if (boneIndex == 0) return {origin.x, origin.y, origin.z + 75.0f}; // Head
    if (boneIndex == 1) return {origin.x, origin.y, origin.z + 45.0f}; // Neck
    return origin;
}

Vector Aimbot::CalculateAngle(Vector src, Vector dst) {
    Vector delta = {dst.x - src.x, dst.y - src.y, dst.z - src.z};
    float hypot = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    
    Vector angles;
    angles.x = std::atan2(-delta.z, hypot) * (180.0f / 3.14159265f);
    angles.y = std::atan2(delta.y, delta.x) * (180.0f / 3.14159265f);
    
    return angles;
}

void Aimbot::ApplySilentAim(Vector angle) {
    uintptr_t viewAnglesPtr = Memory::ReadMemory(m_hClient + OFFSET_VIEW_ANGLES, sizeof(uintptr_t));
    
    float currentAngles[2];
    ReadProcessMemory(GetCurrentProcess(), (LPCVOID)viewAnglesPtr, &currentAngles, 8, NULL);
    
    *(float*)(viewAnglesPtr + 0x4) = angle.y; 
    *(float*)(viewAnglesPtr + 0x0) = angle.x;
}

void Aimbot::ApplyRCS() {
    uintptr_t viewAnglesPtr = Memory::ReadMemory(m_hClient + OFFSET_VIEW_ANGLES, sizeof(uintptr_t));
    
    float recoilScale = 2.0f;
}
