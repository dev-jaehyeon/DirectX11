#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>


using namespace DirectX;
using namespace std;

class ModelClass2
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoord;
		XMFLOAT3 normal;
	};

	struct ModelType2
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass2();
	~ModelClass2();
	bool Initialize(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext, char* _fileName);
	bool InitializeBuffers(ID3D11Device* _device);
	void Shutdown();
	void Render(ID3D11DeviceContext* _deviceContext);
	void RenderBuffers(ID3D11DeviceContext* _deviceContext);

	int GetIndexCount();
private:
	bool LoadMesh(char* _fileName);
	void ReleaseMesh();
private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType2* m_model;
};

