#pragma once
#include <Windows.h>
#include "Vector.h"
#include "Memory.h"

struct TriggerConfig {
    bool enabled = true;
    float delay_ms = 50.0f;
    bool autoFire = true;
    int keyBind = VK_LBUTTON;
};

class Triggerbot {
public:
    Triggerbot(HMODULE hClient);
    void Update();
    void FireWeapon();
    bool IsTargetInCrosshair(Vector aimAngle);

private:
    HMODULE m_hClient;
    TriggerConfig config;
    float lastFireTime = 0.0f;
};
