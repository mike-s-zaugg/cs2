#include "D3D11Renderer.h"
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Einfache HLSL Shader als Strings
const char* VERTEX_SHADER = R"(
    cbuffer MatrixBuffer : register(b0) {
        float4x4 projection;
    };

    struct VS_INPUT {
        float3 position : POSITION;
        float4 color : COLOR;
    };

    struct VS_OUTPUT {
        float4 position : SV_POSITION;
        float4 color : COLOR;
    };

    VS_OUTPUT main(VS_INPUT input) {
        VS_OUTPUT output;
        output.position = mul(float4(input.position, 1.0f), projection);
        output.color = input.color;
        return output;
    }
)";

const char* PIXEL_SHADER = R"(
    struct PS_INPUT {
        float4 position : SV_POSITION;
        float4 color : COLOR;
    };

    float4 main(PS_INPUT input) : SV_TARGET {
        return input.color;
    }
)";

D3D11Renderer::D3D11Renderer()
    : m_screenWidth(1920), m_screenHeight(1080) {}

D3D11Renderer::~D3D11Renderer() {
    // COM-Objekte werden automatisch freigegeben durch ComPtr
}

bool D3D11Renderer::Initialize(IDXGISwapChain* swapChain) {
    if (!swapChain) return false;

    // Hole Device und DeviceContext aus SwapChain
    HRESULT hr = swapChain->GetDevice(IID_PPV_ARGS(&m_device));
    if (FAILED(hr)) {
        std::cerr << "ERROR: Konnte Device nicht aus SwapChain holen" << std::endl;
        return false;
    }

    m_device->GetImmediateContext(&m_deviceContext);

    // Erstelle RenderTargetView
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr)) {
        std::cerr << "ERROR: Konnte BackBuffer nicht holen" << std::endl;
        return false;
    }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
    if (FAILED(hr)) {
        std::cerr << "ERROR: Konnte RenderTargetView nicht erstellen" << std::endl;
        return false;
    }

    // Setze RenderTarget
    m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

    // Erstelle Viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_screenWidth);
    viewport.Height = static_cast<float>(m_screenHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_deviceContext->RSSetViewports(1, &viewport);

    // Erstelle Shaders
    if (!CreateShaders()) return false;

    // Erstelle Pipeline
    if (!CreatePipeline()) return false;

    std::cout << "[D3D11] Renderer erfolgreich initialisiert" << std::endl;
    return true;
}

bool D3D11Renderer::CreateShaders() {
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    // Kompiliere Vertex Shader
    HRESULT hr = D3DCompile(
        VERTEX_SHADER, strlen(VERTEX_SHADER), nullptr,
        nullptr, nullptr, "main", "vs_5_0",
        D3DCOMPILE_DEBUG, 0, &vsBlob, &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            std::cerr << "Vertex Shader Error: " << (char*)errorBlob->GetBufferPointer() << std::endl;
        }
        return false;
    }

    // Kompiliere Pixel Shader
    hr = D3DCompile(
        PIXEL_SHADER, strlen(PIXEL_SHADER), nullptr,
        nullptr, nullptr, "main", "ps_5_0",
        D3DCOMPILE_DEBUG, 0, &psBlob, &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            std::cerr << "Pixel Shader Error: " << (char*)errorBlob->GetBufferPointer() << std::endl;
        }
        return false;
    }

    // Erstelle Shaders
    hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
                                      nullptr, &m_vertexShader);
    if (FAILED(hr)) return false;

    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
                                     nullptr, &m_pixelShader);
    if (FAILED(hr)) return false;

    // Erstelle Input Layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_device->CreateInputLayout(
        layoutDesc, 2,
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        &m_inputLayout
    );

    if (FAILED(hr)) return false;

    std::cout << "[D3D11] Shaders erfolgreich kompiliert" << std::endl;
    return true;
}

bool D3D11Renderer::CreatePipeline() {
    // Erstelle Rasterizer State
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthClipEnable = TRUE;

    HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
    if (FAILED(hr)) return false;

    // Erstelle Blend State (für Transparenz)
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = m_device->CreateBlendState(&blendDesc, &m_blendState);
    if (FAILED(hr)) return false;

    // Erstelle Constant Buffer für Matrizen
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = m_device->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer);
    if (FAILED(hr)) return false;

    std::cout << "[D3D11] Pipeline erfolgreich erstellt" << std::endl;
    return true;
}

void D3D11Renderer::BeginFrame() {
    if (!m_deviceContext || !m_renderTargetView) return;

    // Setze RenderTarget
    m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

    // Setze Input Layout, Shaders und States
    m_deviceContext->IASetInputLayout(m_inputLayout.Get());
    m_deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    m_deviceContext->RSSetState(m_rasterizerState.Get());

    float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_deviceContext->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);

    // Setze Primitive Topology
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void D3D11Renderer::EndFrame() {
    // Hier würde SwapChain->Present() aufgerufen, aber das machen wir extern
}

void D3D11Renderer::DrawBox(float x, float y, float w, float h, DirectX::XMFLOAT4 color) {
    if (!m_deviceContext) return;

    // Erstelle 5 Vertices für Box (Rechteck mit Linie zurück zum Start)
    SimpleVertex vertices[] = {
        { DirectX::XMFLOAT3(x, y, 0.0f), color },
        { DirectX::XMFLOAT3(x + w, y, 0.0f), color },
        { DirectX::XMFLOAT3(x + w, y + h, 0.0f), color },
        { DirectX::XMFLOAT3(x, y + h, 0.0f), color },
        { DirectX::XMFLOAT3(x, y, 0.0f), color }
    };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = vertices;

    ComPtr<ID3D11Buffer> vertexBuffer;
    if (FAILED(m_device->CreateBuffer(&bufferDesc, &initialData, &vertexBuffer))) {
        return;
    }

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    m_deviceContext->Draw(5, 0);
}

void D3D11Renderer::DrawLine(float x1, float y1, float x2, float y2, DirectX::XMFLOAT4 color) {
    if (!m_deviceContext) return;

    SimpleVertex vertices[] = {
        { DirectX::XMFLOAT3(x1, y1, 0.0f), color },
        { DirectX::XMFLOAT3(x2, y2, 0.0f), color }
    };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = vertices;

    ComPtr<ID3D11Buffer> vertexBuffer;
    if (FAILED(m_device->CreateBuffer(&bufferDesc, &initialData, &vertexBuffer))) {
        return;
    }

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    m_deviceContext->Draw(2, 0);
}

void D3D11Renderer::DrawFilledRect(float x, float y, float w, float h, DirectX::XMFLOAT4 color) {
    if (!m_deviceContext) return;

    SimpleVertex vertices[] = {
        { DirectX::XMFLOAT3(x, y, 0.0f), color },
        { DirectX::XMFLOAT3(x + w, y, 0.0f), color },
        { DirectX::XMFLOAT3(x + w, y + h, 0.0f), color },
        { DirectX::XMFLOAT3(x, y + h, 0.0f), color }
    };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = vertices;

    ComPtr<ID3D11Buffer> vertexBuffer;
    if (FAILED(m_device->CreateBuffer(&bufferDesc, &initialData, &vertexBuffer))) {
        return;
    }

    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    m_deviceContext->Draw(4, 0);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void D3D11Renderer::DrawText(float x, float y, const char* text, DirectX::XMFLOAT4 color) {
    // TODO: Text Rendering braucht Font-Handling
    // Für jetzt skip
}

void D3D11Renderer::SetViewport(unsigned int width, unsigned int height) {
    m_screenWidth = width;
    m_screenHeight = height;

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    if (m_deviceContext) {
        m_deviceContext->RSSetViewports(1, &viewport);
    }
}

DirectX::XMFLOAT4 D3D11Renderer::ColorFromRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return DirectX::XMFLOAT4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}
