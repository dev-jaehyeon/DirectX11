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

	//�̰��� ModelClass���� Vertex Buffer�� ���� ���� vertex type�� ���� ���Ǵ�.
	//�̰��� �ݵ�� ColorShaderClass�� ���̾ƿ��� ���ƾ� �Ѵ�.
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	ModelClass();
	ModelClass(const ModelClass& other);
	~ModelClass();

	//�̰��� �Լ����� ���� vertex�� index buffer�� �ʱ�ȭ �Ǵ� �����ϴ� ����� �Ѵ�.
	//Render function�� ���� ������Ʈ���� ���� ī�忡 ���� color shader�� ���� �׷������� �غ��Ѵ�.
	bool Initialize(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	//ModelClass�� private ������ vertex buffer�� indexbuffer �Ӹ� �ƴ϶� �̵��� ũ�⸦ �����Ѵ�. dx11�� ���۴� ���׸� �����̸�
	//���ʿ� ������ �� �� Ÿ���� ��������.
private:
	ID3D11Buffer * m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif

