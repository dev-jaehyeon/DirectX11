#pragma once
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_
//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

using namespace DirectX;

class ModelClass
{
public:
	HWND hwndTemp;

private:

	//VertexType이 color에서 texture로 바뀐다. 이제 이전에 쓰이던 green color가 texture coordinates로 바뀐다.
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

public:
	ModelClass();
	ModelClass(const ModelClass& other);
	~ModelClass();

	//이곳의 함수들은 모델의 vertex와 index buffer를 초기화 또는 종료하는 기능을 한다.
	//Render function은 모델의 지오메트리를 비디오 카드에 놓고 color shader에 의해 그려지도록 준비한다.
	//TextureClass가 추가되면서 deviceContext, textureFilename이 인자로 추가된다.
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

	//텍스처 클래스가 추가되면서 이제 ModelClass는 GetTexture를 통해 Shader에게 텍스처를 넘긴다.
	ID3D11ShaderResourceView* GetTexture();


private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	//TextureClass가 추가되면서 LoadTexture와 ReleaseTexture를 가진다. 이들은 이 모델을 렌더링할 때 사용된다.
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	//ModelClass의 private 변수는 vertex buffer와 indexbuffer 뿐만 아니라 이들의 크기를 추적한다. dx11의 버퍼는 제네릭 버퍼이며
	//최초에 생성될 때 그 타입이 정해진다.
private:
	ID3D11Buffer * m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	//TextureClass가 추가되면서 로드하고 해제하고 접근할 수 있는 텍스처 리소스.
	TextureClass* m_Texture;
};

#endif

