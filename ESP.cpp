#include "ESP.h"
#include <d3dx9.h>

ESP::ESP(HMODULE hClient) : m_hClient(hClient) {}

void ESP::Update() {
    uintptr_t entityList = Memory::ReadMemory(m_hClient + 0x24AA0D8, sizeof(uintptr_t)); // dwEntityList
    
    for (int i = 1; i < 65; i++) {
        PlayerInfo player;
        
        // Team Filter (nur Gegner)
        uintptr_t entity = Memory::ReadMemory(entityList + (i * 0x38), sizeof(uintptr_t));
        if (!entity) continue;

        player.team = *(int*)(entity + 0x10); // dwTeam Offset
        if (player.team == 2) continue; // CT Team Filter

        // Health & Name lesen
        player.health = *(int*)(entity + 0x134); // dwHealth
        memcpy(player.name, (char*)(entity + 0x6C), 64); // dwName Offset

        // Position aus ViewMatrix projizieren
        uintptr_t viewMatrix = Memory::ReadMemory(m_hClient + 0x230ADE0, sizeof(uintptr_t)); // dwViewMatrix
        float matrix[16];
        memcpy(matrix, (float*)(viewMatrix), 64);

        // Simple Projection Logic (vereinfacht)
        player.visible = true; // Visibility Check hier implementieren

        m_players.push_back(player);
    }
}

void ESP::DrawBox(Vector origin, int width, int height, D3DCOLOR color) {
    // D3D11 Draw Box Implementation
    ID3D11DeviceContext* ctx = g_pImmediateContext;
    // ... Zeichnen mit ID3D11DeviceContext
}

void ESP::DrawSkeleton(PlayerInfo* player) {
    // Bone-Verbindungen zeichnen (Kopf, Hals, Wirbelsäule)
}

void ESP::DrawHealthBar(int x, int y, int health, int maxHealth) {
    float ratio = (float)health / (float)maxHealth;
    D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 255 * ratio, 0); // Grün zu Rot
}