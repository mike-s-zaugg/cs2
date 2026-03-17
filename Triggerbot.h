#pragma once
#include <Windows.h>
#include "Memory.h"

struct TriggerConfig {
    bool enabled = true;
    float delay_ms = 50.0f;      // Verzögerung zwischen Schüssen (ms)
    bool autoFire = true;        // Automatisch feuern bei Ziel im FOV
    int keyBind = VK_LBUTTON;    // Maus-Linksklick als Trigger
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
