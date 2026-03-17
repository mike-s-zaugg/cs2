#pragma once
#include <Windows.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <DirectXMath.h>
#include <vector>
#include "Vector.h"
#include "Memory.h"

// Forward declaration
class D3D11Renderer;

struct PlayerInfo {
    int health = 100;
    int team = 2;
    Vector origin = {0, 0, 0};
    char name[64] = "";
    bool visible = false;
};

class ESP {
public:
    ESP(HMODULE hClient, D3D11Renderer* renderer = nullptr);
    void Update();
    void Render();
    void SetRenderer(D3D11Renderer* renderer);
    void DrawBox(Vector origin, int width, int height, DirectX::XMFLOAT4 color);
    void DrawSkeleton(PlayerInfo* player);
    void DrawHealthBar(PlayerInfo* player);

private:
    HMODULE m_hClient;
    D3D11Renderer* m_renderer;
    std::vector<PlayerInfo> m_players;
    
    // Hilfsfunktionen
    Vector WorldToScreen(Vector worldPos);
    bool IsOnScreen(Vector screenPos);
};
