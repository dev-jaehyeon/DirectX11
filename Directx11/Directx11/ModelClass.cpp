#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	//TextureClass 추가로 인한 변경
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

	//텍스처를 로드하는 새로운 함수 호출
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
	//TextureClass가 추가되면서 추가됨
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

/// <summary>
/// Render함수는 ApplicationClass::Render 함수로부터 호출된다. 이 함수는 RenderBuffers를 호출하며, vertex와 index Buffer를
/// 그래픽스 파이프라인에 놓으며, Color Shader가 그들을 렌더할 수 있게 한다.
/// </summary>
/// <param name="deviceContext"></param>
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

/// <summary>
/// 모델의 인덱스 갯수를 반환한다. ColorShader가 이 모델을 그릴 때 필요한 정보다.
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
/// 이 함수가 vertex와 index buffer를 생성하는 곳이다. 일반적으로는 모델 파일로부터 데이터를 읽어 버퍼들을 생성하지만 여기선 그냥 수동으로 채운다.
/// </summary>
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//처음엔 우선 vertex와 index 데이터를 저장할 두 개의 임시 배열을 만든다. 이것은 최종 버퍼를 만들 때 사용된다.
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

	//이제 vertex와 index 배열에 삼각형의 세 vertex 와 각 vertex의 index를 채워 넣는다. 이 예제에서는 시계방향으로 채운다.
	//만약 삼각형을 반대로 그린다면 삼각형이 반대 방향을 보게되고 이것은 backface culling 때문에 정면에서는 보이지 않을 것이다.
	//GPU에 보내는 vertex 순서는 아주 중요하다.
	//vertex 명세에 있는대로 색상 또한 설정된다. 여기선 초록색으로 설정한다.

	//TextureClass의 추가로, 이제 vertex 배열은 color가 아닌 texture 좌표를 가진다. texture vector는 언제나 u와 v다. 
	//첫 번째 좌표(u)는 왼쪽 아래를 의미한다. 이는 U=0.0, v=1.0을 의미한다.

	//#6
	//라이트 추가로 노말 벡터 역시 추가된다.

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

	//채워진 vertex 배열과 index 배열로 이제 vertex buffer와 index buffer를 만들 수 있다. 둘은 같은 프로세스로 만들어진다.
	//먼저, 버퍼의 명세를 채운다. 명세에서 ByteWidth와 BindFlags는 정확히 채워졌는지 확인할 필요가 있다. 명세가 채워진 다음에는
	//subresource 포인터를 채워야한다. 이것은 이전에 만들어놨던 vertex와 index 배열을 가리킨다. 명세와 subresurce 포인터로 CreateBuffer를
	//D3D device에서 호출하면 새로운 버퍼에 대한 포인터를 반환할 것이다.

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

	//vertex buffer와 index buffer를 생성한 다음에는 vertex와 index 배열을 지운다. 이미 버퍼로 카피되었기 때문이다.

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

/// <summary>
/// InitializeBuffer로 만들어둔 vertex와 index 버퍼를 해제한다.
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
/// RenderBuffers는 Render함수로부터 호출된다. 이 함수의 목적은 GPU의 입력 조립기(input assembler)에서 vertex와 index buffer를 활성화하여
/// 설정하는 것이다. GPU가 활성화된 vertex buffer를 가지게 된다면, shader를 이용해 이것을 렌더링할 수 있다.
/// 이 함수는 또한 이 buffer들이 삼각형, 선, 면으로 렌더링되는지를 결정한다. 이 예제는 입력 조립기에 vertex와 index 버퍼를 삼각형으로 그리라고
/// 할 것이다.
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
