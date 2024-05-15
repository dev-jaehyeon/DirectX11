#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class LightShaderClass
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    //���ο� LightBufferType�̴�. �� ����ü�� ������ ������ ������ ���� ���� ���̴�. �̰��� ���ο� pixel shader�� �Ȱ��� ���δ�.
    //��, ������ float �� �߰��Ͽ� 16�� ����� ũ�⸦ �����. �ֳ��ϸ� float ���̴� 28bytes�� �Ǹ�, CreateBuffer���� ������ ���̱� �����̴�.

    struct LightBufferType
    {
        XMFLOAT4 diffuseColor;
        XMFLOAT3 lightDirection;
        float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
    };

public:
    LightShaderClass();
    LightShaderClass(const LightShaderClass&);
    ~LightShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

private:
    bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11SamplerState* m_sampleState;
    ID3D11Buffer* m_matrixBuffer;

    //���ο� ��� ���۴�. ���� ����Ʈ ������ ���ؼ� ���δ�. �̰��� �� Ŭ������ ���� ���Ǹ�, HLSL Pixel Shader���� ���� ����Ʈ ������ �����ϴµ� ���δ�.
    ID3D11Buffer* m_lightBuffer;

};

#endif