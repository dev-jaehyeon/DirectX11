#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include <windows.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

class ShaderClass
{
private:
	enum ShaderType
	{
		ShaderToy,
		Texture,
		MultiTexture,
		ShaderToyTexture,
		TEST_ShaderToyTexture
	};
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct ShaderToyConstantBuffer
	{
		float dx;
		float dy;
		float threshold;
		float strength;
		float iTime;
		float dummy[3];
	};

public:
	ShaderClass();
	~ShaderClass();
	bool InitializeTextureShader(ID3D11Device* _device, HWND _hwnd, const wchar_t* _texFilename);
	bool InitializeShaderToyShader(ID3D11Device* _device, HWND _hwnd);
	bool InitializeShaderToyTextureShader(ID3D11Device* _device, HWND _hwnd, const wchar_t* _psFilename, const wchar_t** _texFilenames, int _texNum);
	bool TEST_InitSToyOneTextureShader(ID3D11Device* _device, HWND _hwnd, const wchar_t* _psFilename, const wchar_t* _texFilename);

	void Shutdown();
	/*bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix);*/

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix);

private:
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

	bool SetTextureShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

	bool SetShaderToyParameters(ID3D11DeviceContext* deviceContext, ComPtr<ID3D11Buffer>& buffer);
	bool SetShaderToyTextureParameters(ID3D11DeviceContext* deviceContext, ComPtr<ID3D11Buffer>& buffer, 
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView** textures, int _texNum);
	bool TEST_SetShaderToyTextureParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture);

	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

	//There is a new private variable for the sampler state pointer.This pointer will be used to interface with the texture shader.
	//이것은 sampler state 포인터를 위한 새로운 변수다. 이 포인터가 texture shader로의 인터페이스로 쓰일 것이다.
	ID3D11SamplerState* m_sampleState;
	HWND hwnd;
	ShaderType shaderType;
	class TextureClass* textures;

	ShaderToyConstantBuffer cBufferData;
	ComPtr<ID3D11Buffer> cBuffer_ShToyPS;

	int texNum;
};

