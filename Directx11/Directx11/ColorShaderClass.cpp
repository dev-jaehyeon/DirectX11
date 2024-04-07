#include "ColorShaderClass.h"

ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

/// <summary>
/// Initialize �Լ��� HLSL ���̴� ������ �̸��� �ѱ��.
/// </summary>
bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;


	// Set the filename of the vertex shader.
	//error = wcscpy_s(vsFilename, 128, L"../Engine/Color.vs"); //�̰� �ȵǴ���
	error = wcscpy_s(vsFilename, 128, L"Color.vs");
	if (error != 0)
	{
		return false;
	}

	// Set the filename of the pixel shader.
	//error = wcscpy_s(psFilename, 128, L"../Engine/Color.ps"); //�̰� �ȵǴ���
	error = wcscpy_s(psFilename, 128, L"Color.ps");
	if (error != 0)
	{
		return false;
	}

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

/// <summary>
/// Render�Լ��� SetShaderParameter �Լ��� ���� �Ķ���͵��� �ʱ�ȭ�Ѵ�. �Ķ���͵��� �����Ǹ� RenderShader�� ȣ���Ͽ� �ʷϻ� �ﰢ���� �׸���.
/// </summary>
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

/// <summary>
/// ���̴��� �ʱ�ȭ�Ѵ�. �� �Լ��� ���̴������� �ε��Ͽ� DirectX�� GPU���� ����� �� �ְ� ���� ���̴�.
/// ���ؽ� ������ �����Ͱ� GPU �׷��Ƚ� ���������ο� ��� ���̴����� �����ؾ��Ѵ�. ���̾ƿ��� modelclass.h�� VertexType�� ��ġ�ؾ� �Ѵ�.
/// </summary>
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//�̰����� ���̴��� �������Ͽ� buffer�鿡 ����ִ´�. ���̴������� �̸�, ���̴��� �̸�, ���̴� ������ �ѱ��. 
	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//���ؽ��� �ȼ� ���̴� �ڵ尡 ���������� ������ �Ǿ� ���ۿ� ���ٸ� �� ���۵��� �̿��ؼ� ���̴� ������Ʈ�� �����. �� ������Ʈ �����͸� ����Ͽ�
	//���ؽ� ���̴��� �ȼ� ���̴��� ������ ���̴�.

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//���� �ܰ�� ���� �������� ���̾ƿ��� ����� ���̴�. �� ���̴��� Position�� Color Vector�� ����ϱ� ������ �� ������ ���� �ʿ䰡 �ִ�.
	//ù ��°�� ä�� ���� SementicName. ���̴��� ���̾ƿ��� �Ӽ����� ������� �����Ѵ�.
	//������ Format.
	//�������� AlignedByteOffset�̴�. �̰��� ���ۿ� �����Ͱ� ��� �����ִ����� ��Ÿ����.
	//���̾ƿ��� ù 12����Ʈ�� ��ġ��, ���� 16����Ʈ�� �÷��� ��Ÿ����.
	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//���̾ƿ� ���� �¾��Ǿ����� D3DDevice�� �̿��� ���̾ƿ� �Է��� ���� �� �ִ�. ���� ���ؽ��� �ȼ����̴� ���۸� �����Ѵ�.
	//�� �̻� ������� �����Ƿ�
	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//��������, ���̴��� Ȱ���ϱ� ���� �¾��� ������۴�. ���ؽ� ���̴����� ���ҵ���, 
	//Buffer�� Usage�� Dynamic���� �ؾ��Ѵ�. �� �����Ӹ��� ������Ʈ �Ǵϱ�.
	//Bind Flag�� �� ���۰� ������۶�� ��Ÿ����.
	// CPU Access Flag�� Usage�� ��Ī�Ǿ���Ѵ�. 
	// �̷��� ���� �Ϸᰡ �Ǹ� ��� ���� �������̽��� ���� �� �ִ�.
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

/// <summary>
/// OutputShaderErrorMessage�� ���ؽ��� �ȼ����̴� �������� �� �� �߻��� ������ ����ش�.
/// </summary>
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

/// <summary>
/// ���̴��� ���� ���� ������ ���� ���ش�.
/// ���⿡�� ���Ǵ� ��ĵ��� ApplicationClass���� ���������, �� �Լ��� ȣ��� �������� Render �Լ��� ȣ��� �� ���ؽ����̴��� ���޵ȴ�.
/// </summary>
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//���̴��� �������� ��ġ��ķ� �����...? DX11�� �䱸�����̶�� �Ѵ�.
	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//m_matrixBuffer�� ��װ� ���ο� ��Ʈ������ �����ϰ� ����Ѵ�.
	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	//���� vertex shader�� ������Ʈ�� ��Ʈ���� ���۸� �����Ѵ�.

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

/// <summary>
/// RenderShader�� Render �Լ����� ȣ��Ǵ� �� ��° �Լ��̴�. SetShaderParameter�� �� �Լ����� ���� ȣ��Ǿ� ���̴� �Ķ���Ͱ� �����
/// �¾��Ǿ� �־�� �Ѵ�.
/// 
/// �� �Լ��� ù ��° �ܰ�� Input Assembler�� Input Layout�� Ȱ��ȭ�ϴ� ���̴�. �̰��� GPU�� vertex buffer�� �������� ������ �˰� ���� ���̴�.
/// �� ��° �ܰ�� �� vertex buffer�� vertex shader�� pixel shader�� �����ϴ� ���̴�. ���̴��� �����ǰ� ���� D3D device context�� ����Ͽ�
/// DrawIndexed �Լ��� ȣ���Ͽ� �ﰢ���� �׸���.
/// </summary>
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
