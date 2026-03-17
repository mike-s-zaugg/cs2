#pragma once
#include <D3D11.h>
#include "Memory.h"

struct PlayerInfo {
    int health = 100;
    int team = 2; // CT=2, T=3
    Vector origin = {0, 0, 0};
    Vector aimPunch = {0, 0, 0};
    char name[64] = "";
    bool visible = false;
};

class ESP {
public:
    ESP(HMODULE hClient);
    void Update();
    void DrawBox(Vector origin, int width, int height, D3DCOLOR color);
    void DrawSkeleton(PlayerInfo* player);
    void DrawHealthBar(int x, int y, int health, int maxHealth);

private:
    HMODULE m_hClient;
    std::vector<PlayerInfo> m_players;
};