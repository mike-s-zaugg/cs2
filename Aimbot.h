#pragma once
#include <Windows.h>
#include "Vector.h"
#include "Memory.h"

struct Matrix3x4 {
    float matrix[3][4];
};

class Aimbot {
public:
    Aimbot(HMODULE hClient, HMODULE hEngine);
    void Update();
    Vector GetTargetBone(int entityId, int boneIndex);
    Vector CalculateAngle(Vector src, Vector dst);
    void ApplySilentAim(Vector angle);
    void ApplyRCS();

private:
    HMODULE m_hClient;
    HMODULE m_hEngine;
    
    struct Config {
        bool enabled = true;
        int targetBone = 0;
        float fov = 35.0f;
        bool silentAim = true;
        bool rcsEnabled = true;
    } config;

    Vector GetLocalPlayerOrigin();
    Matrix3x4 GetViewMatrix();
};
