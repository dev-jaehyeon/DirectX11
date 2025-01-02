#include "ShaderClass.h"
#include "TextureClass.h"
#include <iostream>

ShaderClass::ShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_matrixBuffer = 0;
}

ShaderClass::~ShaderClass()
{
}

bool ShaderClass::InitializeTextureShader(ID3D11Device* _device, HWND _hwnd, const wchar_t* _texFilename)
{
	//텍스처 초기화
	textures = new TextureClass[1];
	textures[0].InitializeWIC(_device, _texFilename);

	shaderType = ShaderType::Texture;
	hwnd = _hwnd;

	const wchar_t* vsFilenamec = L"Texture.vs";
	const wchar_t* psFilenamec = L"Texture.hlsl";
	WCHAR* vsFilename = const_cast<wchar_t*>(vsFilenamec);
	WCHAR* psFilename = const_cast<wchar_t*>(psFilenamec);


	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//텍스처 샘플러를 위한 새로운 변수
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//새로운 텍스처 vertex shader와 pixel shader
	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			//OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			//OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// Create the vertex shader from the buffer.
	result = _device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		
		return false;
	}

	// Create the pixel shader from the buffer.
	result = _device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//Input Layout이 변경된다. 이제 컬러 대신 텍스처 요소를 사용한다. 첫 번째 인자는 바뀌지 않지만 SemanticName과 두 번째 인자의 포맷이 TEXCOORD, DXGI_FORMAT_R32G32_FLOAT
	//으로 바뀐다. 이 두 변화는 새로운 ModelCalss와 셰이더 파일의 VertexType과 맞물린다. 

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = _device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
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

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = _device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{		
		return false;
	}

	//Sampler State 명세는 여기서 셋업된다. 그리고 셰이더 파일로 넘어간다. Texture Sampler에서 가장 중요한 명세는 Filter다.
	//Filter는 폴리곤의 면에 그려지는 최종 텍스처를 만들기 위해 조합되거나 사용되는 픽셀들을 결정한다.
	//이 예시에서는 D3D11_FILTER_MIN_MAG_MIP_LINEAR를 사용하는데, 비싸지만 좋은 결과물을 가져다준다.
	//이것은 Sampler가 축소 및 확대 그리고 밉 레벨 샘플링을 위해 선형보간하도록 한다.
	//Address U와 AddressV는 좌표가 0과 1 사이에 있도록 보증한다. 그 너머에 있다면 0과 1 사이로 놓이게 된다. 다른 세팅은 모두 기본값이다.
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = _device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool ShaderClass::InitializeShaderToyShader(ID3D11Device* _device, HWND _hwnd)
{
	shaderType = ShaderType::ShaderToy;
	hwnd = _hwnd;

	const wchar_t* vsFilenamec = L"VS_ShaderToy.hlsl";
	const wchar_t* psFilenamec = L"PS_FractalPyramid.hlsl";
	WCHAR* vsFilename = const_cast<wchar_t*>(vsFilenamec);
	WCHAR* psFilename = const_cast<wchar_t*>(psFilenamec);
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;

	//텍스처 샘플러를 위한 새로운 변수
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//새로운 텍스처 vertex shader와 pixel shader
	// Compile the vertex shader code.
	UINT compileFlags = 0;
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//D3D10_SHADER_ENABLE_STRICTNESS

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", compileFlags, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", compileFlags, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// Create the vertex shader from the buffer.
	result = _device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Failed Creating VertexShader", MB_OK);
		return false;
	}

	// Create the pixel shader from the buffer.
	result = _device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Failed Creating Pixel Shader", MB_OK);
		return false;
	}

	//Input Layout이 변경된다. 이제 컬러 대신 텍스처 요소를 사용한다. 첫 번째 인자는 바뀌지 않지만 SemanticName과 두 번째 인자의 포맷이 TEXCOORD, DXGI_FORMAT_R32G32_FLOAT
	//으로 바뀐다. 이 두 변화는 새로운 ModelCalss와 셰이더 파일의 VertexType과 맞물린다. 

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = _device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		
		MessageBox(hwnd, L"Failed to Create Input Layout", L"Failed Creating InputLayout", MB_OK);
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	//Sampler State 명세는 여기서 셋업된다. 그리고 셰이더 파일로 넘어간다. Texture Sampler에서 가장 중요한 명세는 Filter다.
	//Filter는 폴리곤의 면에 그려지는 최종 텍스처를 만들기 위해 조합되거나 사용되는 픽셀들을 결정한다.
	//이 예시에서는 D3D11_FILTER_MIN_MAG_MIP_LINEAR를 사용하는데, 비싸지만 좋은 결과물을 가져다준다.
	//이것은 Sampler가 축소 및 확대 그리고 밉 레벨 샘플링을 위해 선형보간하도록 한다.
	//Address U와 AddressV는 좌표가 0과 1 사이에 있도록 보증한다. 그 너머에 있다면 0과 1 사이로 놓이게 된다. 다른 세팅은 모두 기본값이다.
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = _device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	//Constant Buffer 만드는 부분
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(cBufferData);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &cBufferData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	auto hr = _device->CreateBuffer(&cbDesc, &initData,
		cBuffer_ShToyPS.GetAddressOf());
	if (FAILED(hr)) {
		return false;

	}

	return true;
}

bool ShaderClass::InitializeShaderToyTextureShader(ID3D11Device* _device, HWND _hwnd, const wchar_t* _psFilename, const wchar_t** _texFilenames, int _texNum)
{
	textures = new TextureClass[_texNum];
	texNum = _texNum;
	for (int i = 0; i < _texNum; i++)
	{
		textures[i].InitializeWIC(_device, _texFilenames[i]);
	}

	shaderType = ShaderType::ShaderToyTexture;
	hwnd = _hwnd;

	const wchar_t* vsFilenamec = L"VS_ShaderToy.hlsl";
	const wchar_t* psFilenamec = _psFilename;
	WCHAR* vsFilename = const_cast<wchar_t*>(vsFilenamec);
	WCHAR* psFilename = const_cast<wchar_t*>(psFilenamec);
	
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//텍스처 샘플러를 위한 새로운 변수
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	UINT compileFlags = 0;
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//D3D10_SHADER_ENABLE_STRICTNESS

	//새로운 텍스처 vertex shader와 pixel shader
	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", compileFlags, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", compileFlags, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// Create the vertex shader from the buffer.
	result = _device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Failed Creating VertexShader", MB_OK);
		return false;
	}

	// Create the pixel shader from the buffer.
	result = _device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Failed Creating Pixel Shader", MB_OK);
		return false;
	}

	//Input Layout이 변경된다. 이제 컬러 대신 텍스처 요소를 사용한다. 첫 번째 인자는 바뀌지 않지만 SemanticName과 두 번째 인자의 포맷이 TEXCOORD, DXGI_FORMAT_R32G32_FLOAT
	//으로 바뀐다. 이 두 변화는 새로운 ModelCalss와 셰이더 파일의 VertexType과 맞물린다. 

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = _device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{

		MessageBox(hwnd, L"Failed to Create Input Layout", L"Failed Creating InputLayout", MB_OK);
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = _device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	//Sampler State 명세는 여기서 셋업된다. 그리고 셰이더 파일로 넘어간다. Texture Sampler에서 가장 중요한 명세는 Filter다.
	//Filter는 폴리곤의 면에 그려지는 최종 텍스처를 만들기 위해 조합되거나 사용되는 픽셀들을 결정한다.
	//이 예시에서는 D3D11_FILTER_MIN_MAG_MIP_LINEAR를 사용하는데, 비싸지만 좋은 결과물을 가져다준다.
	//이것은 Sampler가 축소 및 확대 그리고 밉 레벨 샘플링을 위해 선형보간하도록 한다.
	//Address U와 AddressV는 좌표가 0과 1 사이에 있도록 보증한다. 그 너머에 있다면 0과 1 사이로 놓이게 된다. 다른 세팅은 모두 기본값이다.
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = _device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	//Constant Buffer 만드는 부분
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(cBufferData);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &cBufferData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	auto hr = _device->CreateBuffer(&cbDesc, &initData,
		cBuffer_ShToyPS.GetAddressOf());
	if (FAILED(hr)) {
		return false;

	}

	return true;
}

bool ShaderClass::TEST_InitSToyOneTextureShader(ID3D11Device* _device, HWND _hwnd, const wchar_t* _psFilename, const wchar_t* _texFilename)
{
	textures = new TextureClass();
	textures->InitializeWIC(_device, _texFilename);

	shaderType = ShaderType::TEST_ShaderToyTexture;
	hwnd = _hwnd;

	const wchar_t* vsFilenamec = L"VS_ShaderToy.hlsl";
	const wchar_t* psFilenamec = _psFilename;
	WCHAR* vsFilename = const_cast<wchar_t*>(vsFilenamec);
	WCHAR* psFilename = const_cast<wchar_t*>(psFilenamec);

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//텍스처 샘플러를 위한 새로운 변수
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	UINT compileFlags = 0;
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	//새로운 텍스처 vertex shader와 pixel shader
	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", compileFlags, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", compileFlags, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// Create the vertex shader from the buffer.
	result = _device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Failed Creating VertexShader", MB_OK);
		return false;
	}

	// Create the pixel shader from the buffer.
	result = _device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Failed Creating Pixel Shader", MB_OK);
		return false;
	}

	//Input Layout이 변경된다. 이제 컬러 대신 텍스처 요소를 사용한다. 첫 번째 인자는 바뀌지 않지만 SemanticName과 두 번째 인자의 포맷이 TEXCOORD, DXGI_FORMAT_R32G32_FLOAT
	//으로 바뀐다. 이 두 변화는 새로운 ModelCalss와 셰이더 파일의 VertexType과 맞물린다. 

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = _device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{

		MessageBox(hwnd, L"Failed to Create Input Layout", L"Failed Creating InputLayout", MB_OK);
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	//Sampler State 명세는 여기서 셋업된다. 그리고 셰이더 파일로 넘어간다. Texture Sampler에서 가장 중요한 명세는 Filter다.
	//Filter는 폴리곤의 면에 그려지는 최종 텍스처를 만들기 위해 조합되거나 사용되는 픽셀들을 결정한다.
	//이 예시에서는 D3D11_FILTER_MIN_MAG_MIP_LINEAR를 사용하는데, 비싸지만 좋은 결과물을 가져다준다.
	//이것은 Sampler가 축소 및 확대 그리고 밉 레벨 샘플링을 위해 선형보간하도록 한다.
	//Address U와 AddressV는 좌표가 0과 1 사이에 있도록 보증한다. 그 너머에 있다면 0과 1 사이로 놓이게 된다. 다른 세팅은 모두 기본값이다.
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = _device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	//Constant Buffer 만드는 부분
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(cBufferData);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &cBufferData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	auto hr = _device->CreateBuffer(&cbDesc, &initData,
		cBuffer_ShToyPS.GetAddressOf());
	if (FAILED(hr)) {
		return false;

	}

	return true;
}

void ShaderClass::Shutdown()
{
	ShutdownShader();
}


void ShaderClass::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

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

bool ShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result = false;
	if (shaderType == ShaderType::Texture)
	{
		ID3D11ShaderResourceView* textureView = textures[0].GetTexture();
		result = SetTextureShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureView);
		if (result == false)
			return false;
	}
	else if (shaderType == ShaderType::ShaderToy)
	{
		result = SetShaderToyParameters(deviceContext, cBuffer_ShToyPS);
		if (result == false)
		{
			MessageBox(hwnd, L"FUckdc", L"Failed Creating VertexShader", MB_OK);
			return false;
		}
	}
	else if (shaderType == ShaderType::ShaderToyTexture)
	{
		ID3D11ShaderResourceView** textureViews = new ID3D11ShaderResourceView * [texNum];
		for (int i = 0; i < texNum; i++)
		{
			textureViews[i] = textures[i].GetTexture();
		}

		result = SetShaderToyTextureParameters(deviceContext, cBuffer_ShToyPS,
			worldMatrix, viewMatrix, projectionMatrix,
			textureViews, texNum);
		if (result == false)
		{
			MessageBox(hwnd, L"FUckdc", L"Failed SetShaderToyPArameterTexture", MB_OK);
			return false;
		}
	}
	else if (shaderType == TEST_ShaderToyTexture)
	{
		ID3D11ShaderResourceView* textureView = textures[0].GetTexture();
		result = TEST_SetShaderToyTextureParameters(deviceContext, textureView);
		if (result == false)
			return false;
	}

	RenderShader(deviceContext, indexCount);
	return true;
}


bool ShaderClass::SetTextureShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

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

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resources in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

bool ShaderClass::SetShaderToyParameters(ID3D11DeviceContext* deviceContext, ComPtr<ID3D11Buffer>& buffer)
{
	//HRESULT result;
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//MatrixBufferType* dataPtr;
	//unsigned int bufferNumber;

	cBufferData.iTime += 0.016f;
	//std::cout << "iTime: " << cBufferData.iTime << std::endl;

	D3D11_MAPPED_SUBRESOURCE ms;
	deviceContext->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, &cBufferData, sizeof(cBufferData));
	deviceContext->Unmap(buffer.Get(), NULL);

	
	return true;
}

bool ShaderClass::SetShaderToyTextureParameters(ID3D11DeviceContext* deviceContext, ComPtr<ID3D11Buffer>& buffer, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textures, int _texNum)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

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

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	cBufferData.iTime += 0.016f;
	//std::cout << "iTime: " << cBufferData.iTime << std::endl;

	D3D11_MAPPED_SUBRESOURCE ms;
	deviceContext->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, &cBufferData, sizeof(cBufferData));
	deviceContext->Unmap(buffer.Get(), NULL);

	for (int i = 0; i < _texNum; i++)
	{
		deviceContext->PSSetShaderResources(i, 1, &textures[i]);
	}

	return true;
}

bool ShaderClass::TEST_SetShaderToyTextureParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture)
{
	cBufferData.iTime += 0.016f;
	//std::cout << "iTime: " << cBufferData.iTime << std::endl;

	D3D11_MAPPED_SUBRESOURCE ms;
	//deviceContext->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	//memcpy(ms.pData, &cBufferData, sizeof(cBufferData));
	//deviceContext->Unmap(buffer.Get(), NULL);

	// Set shader texture resources in the pixel shader.
	//deviceContext->PSSetShaderResources(0, 1, &texture);
	return true;
}

void ShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetConstantBuffers(0, 1, cBuffer_ShToyPS.GetAddressOf());
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);


	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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
