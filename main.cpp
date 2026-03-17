#include <Windows.h>
#include <D3D11.h>
#include <iostream>
#include "Vector.h"
#include "Memory.h"
#include "ESP.h"
#include "Aimbot.h"
#include "Triggerbot.h"

#define CHEAT_NAME "TKazer_CS2_Mod"
#define VERSION "v1.0.4"

HMODULE hClient = nullptr;
HMODULE hEngine = nullptr;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
IDXGISwapChain* pSwapChain = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;

int main() {
    // FIX: Modul-Strings korrigiert (szModule nicht szModPath)
    hClient = (HMODULE)Memory::GetModuleBase("client.dll");
    hEngine = (HMODULE)Memory::GetModuleBase("engine2.dll");

    if (!hClient || !hEngine) {
        std::cerr << "FEHLER: client.dll oder engine2.dll nicht gefunden!" << std::endl;
        return 1;
    }

    std::cout << "Module geladen: client.dll @ " << hClient << ", engine2.dll @ " << hEngine << std::endl;

    // FIX: Instanzen korrekt erstellen
    ESP esp(hClient);
    Aimbot aimbot(hClient, hEngine);
    Triggerbot triggerbot(hClient);

    std::cout << CHEAT_NAME << " " << VERSION << " gestartet" << std::endl;

    // FIX: Simple game loop statt Message Loop (für dedicated cheat)
    bool running = true;
    while (running) {
        // Escape zum Beenden
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            running = false;
        }

        // Update all components
        esp.Update();
        aimbot.Update();
        triggerbot.Update();

        // FIX: Kleine Verzögerung um CPU nicht zu überlasten
        Sleep(1);
    }

    std::cout << "Programm beendet" << std::endl;
    return 0;
}
