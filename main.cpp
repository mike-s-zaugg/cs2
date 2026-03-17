#include <Windows.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <iostream>
#include "Vector.h"
#include "Memory.h"
#include "ESP.h"
#include "Aimbot.h"
#include "Triggerbot.h"

// --- KONFIGURATION & BACKDOOR-CHECKS ---
#define CHEAT_NAME "TKazer_CS2_Mod"
#define VERSION "v1.0.4"

const char* TELEMETRY_URL = "http://localhost"; // Kein Datenleck

HMODULE hClient;
HMODULE hEngine;
ID3D11Device* g_pd3dDevice;
ID3D11DeviceContext* g_pImmediateContext;
IDXGISwapChain* pSwapChain = nullptr;
ID3D11RenderTargetView* mainRenderTargetView;

int main() {
    HWND hWnd = FindWindowA("ValveSteamOverlay", NULL);
    if (!hWnd) hWnd = FindWindowA("GameWindowClass", NULL);
    
    hClient = Memory::GetModuleBase("client.dll");
    hEngine = Memory::GetModuleBase("engine2.dll");

    if (!hClient || !hEngine) {
        std::cout << "Fehler: Module nicht gefunden!" << std::endl;
        return 1;
    }

    ESP esp(hClient);
    Aimbot aimbot(hClient, hEngine);
    Triggerbot triggerbot(hClient);

    MSG msg = {0};
    
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        esp.Update();
        aimbot.Update();
        triggerbot.Update();
    }

    return 0;
}
