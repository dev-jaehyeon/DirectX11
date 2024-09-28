#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

/////////////
// LINKING //
/////////////

//First thing in the header is to specify the libraries to link when using this object module
#pragma comment (lib, "d3d11.lib") //Direct3D의 3D 그래픽 드로잉에 대한 기능을 포함함
#pragma comment (lib, "dxgi.lib") //모니터의 주사율, 그래픽카드 등에 대한 정보에 대한 하드웨어 인터페이스
#pragma comment (lib, "d3dcompiler.lib") //셰이더 컴파일에 대한 기능을 포함

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h> // ComPtr
using namespace DirectX;
using Microsoft::WRL::ComPtr;
//원래 view matrix 변수가 있어야하지만 카메라에 넣기로 함
class D3DClass
{
public:
    D3DClass();
    D3DClass(const D3DClass&);
    ~D3DClass();

    bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix(XMMATRIX&);
    void GetWorldMatrix(XMMATRIX&);
    void GetOrthoMatrix(XMMATRIX&);

    void GetVideoCardInfo(char*, int&);

    void SetBackBufferRenderTarget();
    void ResetViewport();

private:
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11Device> m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11RasterizerState* m_rasterState;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
    D3D11_VIEWPORT m_viewport;
};

#endif
