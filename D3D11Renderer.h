#pragma once
#include <Windows.h>
#include <D3D11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "Vector.h"

using Microsoft::WRL::ComPtr;

// Einfache 2D Vertex-Struktur für Box-Drawing
struct SimpleVertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
};

class D3D11Renderer {
public:
    D3D11Renderer();
    ~D3D11Renderer();

    // Initialisierung
    bool Initialize(IDXGISwapChain* swapChain);
    
    // Rendering-Funktionen
    void BeginFrame();
    void EndFrame();
    
    // Zeichenfunktionen
    void DrawBox(float x, float y, float w, float h, DirectX::XMFLOAT4 color);
    void DrawLine(float x1, float y1, float x2, float y2, DirectX::XMFLOAT4 color);
    void DrawFilledRect(float x, float y, float w, float h, DirectX::XMFLOAT4 color);
    void DrawText(float x, float y, const char* text, DirectX::XMFLOAT4 color);

    // Hilfsfunktionen
    void SetViewport(unsigned int width, unsigned int height);
    DirectX::XMFLOAT4 ColorFromRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

private:
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_constantBuffer;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;
    ComPtr<ID3D11BlendState> m_blendState;

    bool CreateShaders();
    bool CreatePipeline();
    
    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
};
