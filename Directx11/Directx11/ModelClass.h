#pragma once
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_
//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class ModelClass
{
private:

	//이것은 ModelClass에서 Vertex Buffer와 같이 사용될 vertex type에 대한 정의다.
	//이것은 반드시 ColorShaderClass의 레이아웃과 같아야 한다.
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	ModelClass();
	ModelClass(const ModelClass& other);
	~ModelClass();

	//이곳의 함수들은 모델의 vertex와 index buffer를 초기화 또는 종료하는 기능을 한다.
	//Render function은 모델의 지오메트리를 비디오 카드에 놓고 color shader에 의해 그려지도록 준비한다.
	bool Initialize(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	//ModelClass의 private 변수는 vertex buffer와 indexbuffer 뿐만 아니라 이들의 크기를 추적한다. dx11의 버퍼는 제네릭 버퍼이며
	//최초에 생성될 때 그 타입이 정해진다.
private:
	ID3D11Buffer * m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif

