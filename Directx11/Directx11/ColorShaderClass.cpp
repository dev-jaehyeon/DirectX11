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
/// Initialize 함수는 HLSL 셰이더 파일의 이름을 넘긴다.
/// </summary>
bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;


	// Set the filename of the vertex shader.
	//error = wcscpy_s(vsFilename, 128, L"../Engine/Color.vs"); //이건 안되더라
	error = wcscpy_s(vsFilename, 128, L"Color.vs");
	if (error != 0)
	{
		return false;
	}

	// Set the filename of the pixel shader.
	//error = wcscpy_s(psFilename, 128, L"../Engine/Color.ps"); //이건 안되더라
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
/// Render함수는 SetShaderParameter 함수로 먼저 파라미터들을 초기화한다. 파라미터들이 설정되면 RenderShader를 호출하여 초록색 삼각형을 그린다.
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
/// 셰이더를 초기화한다. 이 함수는 셰이더파일을 로드하여 DirectX와 GPU에서 사용할 수 있게 해줄 것이다.
/// 버텍스 버퍼의 데이터가 GPU 그래픽스 파이프라인에 어떻게 엮이는지를 관찰해야한다. 레이아웃은 modelclass.h의 VertexType과 일치해야 한다.
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

	//이곳에서 셰이더를 컴파일하여 buffer들에 집어넣는다. 셰이더파일의 이름, 셰이더의 이름, 셰이더 버전을 넘긴다. 
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

	//버텍스와 픽셀 셰이더 코드가 성공적으로 컴파일 되어 버퍼에 들어갔다면 이 버퍼들을 이용해서 셰이더 오브젝트를 만든다. 이 오브젝트 포인터를 사용하여
	//버텍스 셰이더와 픽셀 셰이더와 연결할 것이다.

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

	//다음 단계는 정점 데이터의 레이아웃을 만드는 것이다. 이 셰이더는 Position과 Color Vector를 사용하기 때문에 두 가지를 만들 필요가 있다.
	//첫 번째로 채울 것은 SementicName. 셰이더가 레이아웃의 속성들을 사용할지 결정한다.
	//다음은 Format.
	//마지막은 AlignedByteOffset이다. 이것은 버퍼에 데이터가 어떻게 놓여있는지를 나타낸다.
	//레이아웃은 첫 12바이트는 위치를, 다음 16바이트는 컬러를 나타낸다.
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

	//레이아웃 명세가 셋업되었으니 D3DDevice를 이용해 레이아웃 입력을 만들어낼 수 있다. 또한 버텍스와 픽셀셰이더 버퍼를 해제한다.
	//더 이상 사용하지 않으므로
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

	//다음으로, 셰이더를 활용하기 위한 셋업은 상수버퍼다. 버텍스 셰이더에서 보았듯이, 
	//Buffer의 Usage는 Dynamic으로 해야한다. 매 프레임마다 업데이트 되니까.
	//Bind Flag는 이 버퍼가 상수버퍼라고 나타낸다.
	// CPU Access Flag는 Usage와 매칭되어야한다. 
	// 이렇게 명세가 완료가 되면 상수 버퍼 인터페이스를 만들 수 있다.
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
/// OutputShaderErrorMessage는 버텍스나 픽셀셰이더 컴파일을 할 때 발생할 에러를 뱉어준다.
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
/// 셰이더의 전역 변수 세팅을 쉽게 해준다.
/// 여기에서 사용되는 행렬들은 ApplicationClass에서 만들어지며, 이 함수가 호출된 다음에는 Render 함수가 호출될 때 버텍스셰이더로 전달된다.
/// </summary>
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//셰이더로 가기전에 전치행렬로 만든다...? DX11의 요구사항이라고 한다.
	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//m_matrixBuffer를 잠그고 새로운 매트릭스를 세팅하고 언락한다.
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

	//이제 vertex shader에 업데이트된 매트릭스 버퍼를 세팅한다.

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

/// <summary>
/// RenderShader는 Render 함수에서 호출되는 두 번째 함수이다. SetShaderParameter가 이 함수보다 먼저 호출되어 셰이더 파라미터가 제대로
/// 셋업되어 있어야 한다.
/// 
/// 이 함수의 첫 번째 단계는 Input Assembler의 Input Layout을 활성화하는 것이다. 이것은 GPU가 vertex buffer의 데이터의 포맷을 알게 해줄 것이다.
/// 두 번째 단계는 이 vertex buffer에 vertex shader와 pixel shader를 세팅하는 것이다. 셰이더가 설정되고 나면 D3D device context를 사용하여
/// DrawIndexed 함수를 호출하여 삼각형을 그린다.
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
