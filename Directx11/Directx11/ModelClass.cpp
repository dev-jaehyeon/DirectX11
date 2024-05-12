#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	//TextureClass �߰��� ���� ����
	m_Texture = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	bool result;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	//�ؽ�ó�� �ε��ϴ� ���ο� �Լ� ȣ��
	// 
	// Load the texture for this model.
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	//TextureClass�� �߰��Ǹ鼭 �߰���
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

/// <summary>
/// Render�Լ��� ApplicationClass::Render �Լ��κ��� ȣ��ȴ�. �� �Լ��� RenderBuffers�� ȣ���ϸ�, vertex�� index Buffer��
/// �׷��Ƚ� ���������ο� ������, Color Shader�� �׵��� ������ �� �ְ� �Ѵ�.
/// </summary>
/// <param name="deviceContext"></param>
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

/// <summary>
/// ���� �ε��� ������ ��ȯ�Ѵ�. ColorShader�� �� ���� �׸� �� �ʿ��� ������.
/// </summary>
/// <returns></returns>
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

/// <summary>
/// �� �Լ��� vertex�� index buffer�� �����ϴ� ���̴�. �Ϲ������δ� �� ���Ϸκ��� �����͸� �о� ���۵��� ���������� ���⼱ �׳� �������� ä���.
/// </summary>
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//ó���� �켱 vertex�� index �����͸� ������ �� ���� �ӽ� �迭�� �����. �̰��� ���� ���۸� ���� �� ���ȴ�.
	// Set the number of vertices in the vertex array.
	m_vertexCount = 3;

	// Set the number of indices in the index array.
	m_indexCount = 3;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//���� vertex�� index �迭�� �ﰢ���� �� vertex �� �� vertex�� index�� ä�� �ִ´�. �� ���������� �ð�������� ä���.
	//���� �ﰢ���� �ݴ�� �׸��ٸ� �ﰢ���� �ݴ� ������ ���Եǰ� �̰��� backface culling ������ ���鿡���� ������ ���� ���̴�.
	//GPU�� ������ vertex ������ ���� �߿��ϴ�.
	//vertex ���� �ִ´�� ���� ���� �����ȴ�. ���⼱ �ʷϻ����� �����Ѵ�.

	//TextureClass�� �߰���, ���� vertex �迭�� color�� �ƴ� texture ��ǥ�� ������. texture vector�� ������ u�� v��. 
	//ù ��° ��ǥ(u)�� ���� �Ʒ��� �ǹ��Ѵ�. �̴� U=0.0, v=1.0�� �ǹ��Ѵ�.

	//#6
	//����Ʈ �߰��� �븻 ���� ���� �߰��ȴ�.

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	//ä���� vertex �迭�� index �迭�� ���� vertex buffer�� index buffer�� ���� �� �ִ�. ���� ���� ���μ����� ���������.
	//����, ������ ���� ä���. ������ ByteWidth�� BindFlags�� ��Ȯ�� ä�������� Ȯ���� �ʿ䰡 �ִ�. ���� ä���� ��������
	//subresource �����͸� ä�����Ѵ�. �̰��� ������ �������� vertex�� index �迭�� ����Ų��. ���� subresurce �����ͷ� CreateBuffer��
	//D3D device���� ȣ���ϸ� ���ο� ���ۿ� ���� �����͸� ��ȯ�� ���̴�.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//vertex buffer�� index buffer�� ������ �������� vertex�� index �迭�� �����. �̹� ���۷� ī�ǵǾ��� �����̴�.

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

/// <summary>
/// InitializeBuffer�� ������ vertex�� index ���۸� �����Ѵ�.
/// </summary>
void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

/// <summary>
/// RenderBuffers�� Render�Լ��κ��� ȣ��ȴ�. �� �Լ��� ������ GPU�� �Է� ������(input assembler)���� vertex�� index buffer�� Ȱ��ȭ�Ͽ�
/// �����ϴ� ���̴�. GPU�� Ȱ��ȭ�� vertex buffer�� ������ �ȴٸ�, shader�� �̿��� �̰��� �������� �� �ִ�.
/// �� �Լ��� ���� �� buffer���� �ﰢ��, ��, ������ �������Ǵ����� �����Ѵ�. �� ������ �Է� �����⿡ vertex�� index ���۸� �ﰢ������ �׸����
/// �� ���̴�.
/// </summary>
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// Create and initialize the texture object.
	m_Texture = new TextureClass;
	m_Texture->hwndTemp = hwndTemp;
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
