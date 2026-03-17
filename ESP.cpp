#include "ESP.h"
#include <d3dx9.h>

ESP::ESP(HMODULE hClient) : m_hClient(hClient) {}

void ESP::Update() {
    uintptr_t entityList = Memory::ReadMemory(m_hClient + 0x24AA0D8, sizeof(uintptr_t));
    
    for (int i = 1; i < 65; i++) {
        PlayerInfo player;
        
        uintptr_t entity = Memory::ReadMemory(entityList + (i * 0x38), sizeof(uintptr_t));
        if (!entity) continue;

        player.team = *(int*)(entity + 0x10); 
        if (player.team == 2) continue; 

        player.health = *(int*)(entity + 0x134); 
        memcpy(player.name, (char*)(entity + 0x6C), 64);

        uintptr_t viewMatrix = Memory::ReadMemory(m_hClient + 0x230ADE0, sizeof(uintptr_t));
        float matrix[16];
        memcpy(matrix, (float*)(viewMatrix), 64);

        player.visible = true; 

        m_players.push_back(player);
    }
}

void ESP::DrawBox(Vector origin, int width, int height, D3DCOLOR color) {
    ID3D11DeviceContext* ctx = g_pImmediateContext;
}

void ESP::DrawSkeleton(PlayerInfo* player) {}

void ESP::DrawHealthBar(int x, int y, int health, int maxHealth) {
    float ratio = (float)health / (float)maxHealth;
    D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 255 * ratio, 0);
}
