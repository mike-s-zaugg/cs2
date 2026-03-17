#include "ESP.h"
#include <vector>
#include <cstring>

ESP::ESP(HMODULE hClient) : m_hClient(hClient) {}

void ESP::Update() {
    if (!m_hClient) return; // FIX: Null check
    
    m_players.clear(); // FIX: Vector clearen vor neuem Update
    
    uintptr_t entityList = Memory::ReadMemory((uintptr_t)m_hClient + 0x24AA0D8, sizeof(uintptr_t));
    if (!entityList) return; // FIX: Null check
    
    for (int i = 1; i < 65; i++) {
        PlayerInfo player;
        
        uintptr_t entity = Memory::ReadMemory(entityList + (i * 0x38), sizeof(uintptr_t));
        if (!entity) continue;

        player.team = *(int*)(entity + 0x10); 
        if (player.team == 2) continue; // Skip eigenes Team

        player.health = *(int*)(entity + 0x134); 
        // FIX: Sicher kopieren
        memcpy_s(player.name, sizeof(player.name), (char*)(entity + 0x6C), 63);
        player.name[63] = '\0'; // FIX: Null-Termination

        // FIX: Origin setzen
        player.origin = *(Vector*)(entity + 0x134);

        uintptr_t viewMatrix = Memory::ReadMemory((uintptr_t)m_hClient + 0x230ADE0, sizeof(uintptr_t));
        if (viewMatrix) {
            float matrix[16] = {0};
            memcpy(matrix, (float*)(viewMatrix), 64);
        }

        player.visible = true; 

        m_players.push_back(player);
    }
}

void ESP::DrawBox(Vector origin, int width, int height, D3DCOLOR color) {
    // FIX: Dummy implementation, real rendering würde D3D11 brauchen
    if (width <= 0 || height <= 0) return;
}

void ESP::DrawSkeleton(PlayerInfo* player) {
    if (!player) return; // FIX: Null check
}

void ESP::DrawHealthBar(int x, int y, int health, int maxHealth) {
    if (maxHealth <= 0) return; // FIX: Division by zero
    
    float ratio = (float)health / (float)maxHealth;
    if (ratio > 1.0f) ratio = 1.0f; // FIX: Clamp
    if (ratio < 0.0f) ratio = 0.0f;
    
    D3DCOLOR color = D3DCOLOR_ARGB(255, (unsigned char)(255 * (1.0f - ratio)), (unsigned char)(255 * ratio), 0);
}
