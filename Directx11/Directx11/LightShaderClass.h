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

    //새로운 LightBufferType이다. 이 구조체는 라이팅 정보를 가지기 위해 쓰일 것이다. 이것은 새로운 pixel shader와 똑같이 쓰인다.
    //단, 여분의 float 을 추가하여 16의 배수로 크기를 맞춘다. 왜냐하면 float 없이는 28bytes가 되며, CreateBuffer에서 실패할 것이기 때문이다.

    struct LightBufferType
    {
        XMFLOAT4 ambientColor; //#9 이제 앰비언트 컬러를 가진다.
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
    //#9 ambient color가 추가됨
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);

private:
    bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    //#9마찬가지로 ambient color가 추가됨
    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11SamplerState* m_sampleState;
    ID3D11Buffer* m_matrixBuffer;

    //새로운 상수 버퍼다. 오직 라이트 정보를 위해서 쓰인다. 이것은 이 클래스에 의해 사용되며, HLSL Pixel Shader에서 전역 라이트 변수를 설정하는데 쓰인다.
    ID3D11Buffer* m_lightBuffer;

};

#endif
