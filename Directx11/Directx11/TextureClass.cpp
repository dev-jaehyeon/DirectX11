#include "TextureClass.h"

TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

/// <summary>
/// 텍스처 클래스의 초기화는 Direct3D device를 필요로하고, Targa 이미지 파일의 이름 역시 필요로한다. 
/// 우선 Targa data를 배열에 로드한다.
/// 그런 다음 texture를 만들고 tga data를 올바른 포맷으로 로드하여 배열에 넣을 것이다. 텍스처가 로드가 되면 셰이더가 쓸 resource view를 만든다.
/// </summary>
/// <returns></returns>
bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//최초로 TextureClass::LoadTarga32Bit를 호출하여 targa 파일을 로드한다.
		// Load the targa image data into memory.
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}

	//다음으로, DirectX Texture의 명세를 셋업한다. Targa image data로부터 높이와 너비를 이용하여 32-bit RGBA Texture로 세팅한다. SampleDesc를 기본값으로 둔다.
	//D3D11_USAGE_DEFAULT를 사용하여 메모리를 최적화하여 사용한다.
	//마지막으로 MipLevels, BindFlags, MiscFlags를 세팅하여 밉맵된 텍스처를 구성한다. 명세가 완료되면 CreateTexture2D를 호출하고 빈 텍스처를 생선한다.
	//그 다음 스텝에서는 Targa data를 그 비어있는 텍스처에 넣는다.
	// Setup the description of the texture.
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	//여기서 UpdateSubresource를 사용하는데, 이것이 실질적으로 DirectX Texture에 Targa Data Array를 복사하여 넣는다.
	//이전 튜토리얼에서 Map과 Unmap을 사용하여 ModelClass에서 행렬들을 Matrix Constant Buffer로 복사했었다. 여기서도 똑같이 사용할 것이다.
	//사실 Map과 Unmap이 UpdateSubresource보다 더 빠르지만 두 로딩 방법들을 특정한 목적들을 가지고 있고 상황에 따라 맞게 호출해야 한다.
	//추천은 Unmap과 Map의 경우 데이터가 프레임마다 다시 로드되는 경우에 추천된다. UpdateSubresource의 경우 한 번만 로드되는 때에 추천된다.
	//이유는 UpdateSubresource는 데이터를 속도가 빠른 메모리에 놓는다. 그리고 이것은 캐싱이 가능해지고 당장 리로드되거나 사라지지 않을 것을 의미한다.
	//DirectX는 또한 D3D11_USAGE_DEFAULT를 사용함으로서 우리가 UpdateSubresource를 사용할 것을 안다.
	//Map과 Unmap은 데이터를 캐싱이되지 않는 메모리에 할당하고 DirectX는 이 데이터가 곧 덮어씌워질 것을 안다.
	//D3D_USAGE_DYNAMIC은 DirectX가 이 타입의 데이터가 일시적이라는 것을 알게 해준다.

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	//텍스처가 로드된 다음에는 Shader Resource View를 만든다. 이것은 Shader에서 texture를 세팅하기 위한 포인터를 가질 수 있게 해준다.
	//명세에서 두 가지 중요한 변수를 설정한다.
	//Mipmap variables는 어느 거리에서나 높은 퀄리티의 텍스처 렌더링을 위한 모든 범위의 Mipmap level들을 준다.
	//Shader resource view가 생성되면 GenerateMipmaps를 호출하고 이것은 Mipmap들을 만들어준다.

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(m_textureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

/// <summary>
/// Helper 함수, 셰이더가 렌더링을 위해 필요로하는 접근 방법
/// </summary>
ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

/// <summary>
/// Target 이미지를 로딩하는 함수다. Targa 이지미는 Upside down 형식으로 저장되기 때문에 이것을 사용하기 전에 한번 뒤집을 필요가 있다.
/// 로드한 다음 배열에 넣은 후 m_targaData에 올바른 배열로 다시 넣을 것이다. 32비트 targa파일만 다루고 24비트로 저장된 targa는 거부된다.
/// </summary>
bool TextureClass::LoadTarga32Bit(char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		MessageBox(hwndTemp, L"Stopped Here: fopen_s", L"Error", MB_OK);
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		MessageBox(hwndTemp, L"Stopped Here: fread", L"Error", MB_OK);
		return false;
	}

	// Get the important information from the header.
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		MessageBox(hwndTemp, L"Stopped Here: is not 32bit", L"Error", MB_OK);
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = m_width * m_height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (m_width * m_height * 4) - (m_width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
	for (j = 0; j < m_height; j++)
	{
		for (i = 0; i < m_width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (m_width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return true;
}

int TextureClass::GetWidth()
{
	return m_width;
}

int TextureClass::GetHeight()
{
	return m_height;
}

