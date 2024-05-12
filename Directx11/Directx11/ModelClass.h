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

	//VertexType�� color���� texture�� �ٲ��. ���� ������ ���̴� green color�� texture coordinates�� �ٲ��.
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

	//�̰��� �Լ����� ���� vertex�� index buffer�� �ʱ�ȭ �Ǵ� �����ϴ� ����� �Ѵ�.
	//Render function�� ���� ������Ʈ���� ���� ī�忡 ���� color shader�� ���� �׷������� �غ��Ѵ�.
	//TextureClass�� �߰��Ǹ鼭 deviceContext, textureFilename�� ���ڷ� �߰��ȴ�.
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

	//�ؽ�ó Ŭ������ �߰��Ǹ鼭 ���� ModelClass�� GetTexture�� ���� Shader���� �ؽ�ó�� �ѱ��.
	ID3D11ShaderResourceView* GetTexture();


private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	//TextureClass�� �߰��Ǹ鼭 LoadTexture�� ReleaseTexture�� ������. �̵��� �� ���� �������� �� ���ȴ�.
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	//ModelClass�� private ������ vertex buffer�� indexbuffer �Ӹ� �ƴ϶� �̵��� ũ�⸦ �����Ѵ�. dx11�� ���۴� ���׸� �����̸�
	//���ʿ� ������ �� �� Ÿ���� ��������.
private:
	ID3D11Buffer * m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	//TextureClass�� �߰��Ǹ鼭 �ε��ϰ� �����ϰ� ������ �� �ִ� �ؽ�ó ���ҽ�.
	TextureClass* m_Texture;
};

#endif

