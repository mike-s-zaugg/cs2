#include "ESP.h"
#include "D3D11Renderer.h"
#include <cstring>
#include <cmath>

ESP::ESP(HMODULE hClient, D3D11Renderer* renderer)
    : m_hClient(hClient), m_renderer(renderer) {}

void ESP::SetRenderer(D3D11Renderer* renderer) {
    m_renderer = renderer;
}

void ESP::Update() {
    if (!m_hClient) return;

    m_players.clear();

    uintptr_t entityList = Memory::ReadMemory((uintptr_t)m_hClient + 0x24AA0D8, sizeof(uintptr_t));
    if (!entityList) return;

    for (int i = 1; i < 65; i++) {
        PlayerInfo player;

        uintptr_t entity = Memory::ReadMemory(entityList + (i * 0x38), sizeof(uintptr_t));
        if (!entity) continue;

        player.team = *(int*)(entity + 0x10);
        if (player.team == 2) continue; // Skip eigenes Team

        player.health = *(int*)(entity + 0x134);
        memcpy_s(player.name, sizeof(player.name), (char*)(entity + 0x6C), 63);
        player.name[63] = '\0';

        player.origin = *(Vector*)(entity + 0x134);
        player.visible = true;

        m_players.push_back(player);
    }
}

void ESP::Render() {
    if (!m_renderer || m_players.empty()) return;

    m_renderer->BeginFrame();

    for (auto& player : m_players) {
        if (!player.visible) continue;

        // Wandle Welt-Koordinaten in Screen-Koordinaten um
        Vector screenPos = WorldToScreen(player.origin);

        if (!IsOnScreen(screenPos)) continue;

        // Bestimme Farbe basierend auf Health
        DirectX::XMFLOAT4 color;
        if (player.health > 75) {
            color = m_renderer->ColorFromRGB(0, 255, 0, 255); // Grün
        } else if (player.health > 50) {
            color = m_renderer->ColorFromRGB(255, 255, 0, 255); // Gelb
        } else if (player.health > 25) {
            color = m_renderer->ColorFromRGB(255, 165, 0, 255); // Orange
        } else {
            color = m_renderer->ColorFromRGB(255, 0, 0, 255); // Rot
        }

        // Zeichne Box um Spieler
        DrawBox(player.origin, 40, 70, color);

        // Zeichne Health Bar
        DrawHealthBar(&player);

        // Zeichne Skeleton
        DrawSkeleton(&player);
    }

    m_renderer->EndFrame();
}

Vector ESP::WorldToScreen(Vector worldPos) {
    // TODO: Verwende View Matrix um echte Screen-Koordinaten zu berechnen
    // Für jetzt: Simplified version
    // In echtem Code würde man die View Matrix vom Game lesen
    
    // Fake-Projektion für Demo
    Vector screenPos;
    screenPos.x = 960.0f + (worldPos.x * 0.1f);
    screenPos.y = 540.0f - (worldPos.z * 0.1f);
    screenPos.z = 0.0f;
    
    return screenPos;
}

bool ESP::IsOnScreen(Vector screenPos) {
    // Überprüfe ob Position im Screen-Bereich ist
    return screenPos.x > 0 && screenPos.x < 1920 &&
           screenPos.y > 0 && screenPos.y < 1080;
}

void ESP::DrawBox(Vector origin, int width, int height, DirectX::XMFLOAT4 color) {
    if (!m_renderer) return;

    Vector screenPos = WorldToScreen(origin);
    
    // Zeichne Box (Rechteck-Umriss)
    m_renderer->DrawBox(screenPos.x - width / 2.0f, screenPos.y - height / 2.0f,
                        (float)width, (float)height, color);
}

void ESP::DrawSkeleton(PlayerInfo* player) {
    if (!player || !m_renderer) return;

    DirectX::XMFLOAT4 boneColor = m_renderer->ColorFromRGB(255, 255, 255, 200);

    // TODO: Bone Positionen vom Game lesen und verbinden
    // Für jetzt nur Placeholder
}

void ESP::DrawHealthBar(PlayerInfo* player) {
    if (!player || !m_renderer) return;

    Vector screenPos = WorldToScreen(player->origin);
    
    // Health Bar Dimensionen
    float barWidth = 50.0f;
    float barHeight = 5.0f;
    float barX = screenPos.x - barWidth / 2.0f;
    float barY = screenPos.y - 50.0f;

    // Schwarzer Hintergrund
    DirectX::XMFLOAT4 bgColor = m_renderer->ColorFromRGB(0, 0, 0, 200);
    m_renderer->DrawFilledRect(barX - 2, barY - 2, barWidth + 4, barHeight + 4, bgColor);

    // Health Bar mit Farbe basierend auf HP
    float healthRatio = std::max(0.0f, std::min(1.0f, player->health / 100.0f));
    DirectX::XMFLOAT4 healthColor;
    
    if (healthRatio > 0.75f) {
        healthColor = m_renderer->ColorFromRGB(0, 255, 0, 255);
    } else if (healthRatio > 0.5f) {
        healthColor = m_renderer->ColorFromRGB(255, 255, 0, 255);
    } else if (healthRatio > 0.25f) {
        healthColor = m_renderer->ColorFromRGB(255, 165, 0, 255);
    } else {
        healthColor = m_renderer->ColorFromRGB(255, 0, 0, 255);
    }

    m_renderer->DrawFilledRect(barX, barY, barWidth * healthRatio, barHeight, healthColor);
    
    // Weißer Rahmen
    DirectX::XMFLOAT4 frameColor = m_renderer->ColorFromRGB(255, 255, 255, 255);
    m_renderer->DrawBox(barX, barY, barWidth, barHeight, frameColor);
}
