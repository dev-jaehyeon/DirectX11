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
/// �ؽ�ó Ŭ������ �ʱ�ȭ�� Direct3D device�� �ʿ���ϰ�, Targa �̹��� ������ �̸� ���� �ʿ���Ѵ�. 
/// �켱 Targa data�� �迭�� �ε��Ѵ�.
/// �׷� ���� texture�� ����� tga data�� �ùٸ� �������� �ε��Ͽ� �迭�� ���� ���̴�. �ؽ�ó�� �ε尡 �Ǹ� ���̴��� �� resource view�� �����.
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

	//���ʷ� TextureClass::LoadTarga32Bit�� ȣ���Ͽ� targa ������ �ε��Ѵ�.
		// Load the targa image data into memory.
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}

	//��������, DirectX Texture�� ���� �¾��Ѵ�. Targa image data�κ��� ���̿� �ʺ� �̿��Ͽ� 32-bit RGBA Texture�� �����Ѵ�. SampleDesc�� �⺻������ �д�.
	//D3D11_USAGE_DEFAULT�� ����Ͽ� �޸𸮸� ����ȭ�Ͽ� ����Ѵ�.
	//���������� MipLevels, BindFlags, MiscFlags�� �����Ͽ� �Ӹʵ� �ؽ�ó�� �����Ѵ�. ���� �Ϸ�Ǹ� CreateTexture2D�� ȣ���ϰ� �� �ؽ�ó�� �����Ѵ�.
	//�� ���� ���ܿ����� Targa data�� �� ����ִ� �ؽ�ó�� �ִ´�.
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

	//���⼭ UpdateSubresource�� ����ϴµ�, �̰��� ���������� DirectX Texture�� Targa Data Array�� �����Ͽ� �ִ´�.
	//���� Ʃ�丮�󿡼� Map�� Unmap�� ����Ͽ� ModelClass���� ��ĵ��� Matrix Constant Buffer�� �����߾���. ���⼭�� �Ȱ��� ����� ���̴�.
	//��� Map�� Unmap�� UpdateSubresource���� �� �������� �� �ε� ������� Ư���� �������� ������ �ְ� ��Ȳ�� ���� �°� ȣ���ؾ� �Ѵ�.
	//��õ�� Unmap�� Map�� ��� �����Ͱ� �����Ӹ��� �ٽ� �ε�Ǵ� ��쿡 ��õ�ȴ�. UpdateSubresource�� ��� �� ���� �ε�Ǵ� ���� ��õ�ȴ�.
	//������ UpdateSubresource�� �����͸� �ӵ��� ���� �޸𸮿� ���´�. �׸��� �̰��� ĳ���� ���������� ���� ���ε�ǰų� ������� ���� ���� �ǹ��Ѵ�.
	//DirectX�� ���� D3D11_USAGE_DEFAULT�� ��������μ� �츮�� UpdateSubresource�� ����� ���� �ȴ�.
	//Map�� Unmap�� �����͸� ĳ���̵��� �ʴ� �޸𸮿� �Ҵ��ϰ� DirectX�� �� �����Ͱ� �� ������� ���� �ȴ�.
	//D3D_USAGE_DYNAMIC�� DirectX�� �� Ÿ���� �����Ͱ� �Ͻ����̶�� ���� �˰� ���ش�.

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	//�ؽ�ó�� �ε�� �������� Shader Resource View�� �����. �̰��� Shader���� texture�� �����ϱ� ���� �����͸� ���� �� �ְ� ���ش�.
	//������ �� ���� �߿��� ������ �����Ѵ�.
	//Mipmap variables�� ��� �Ÿ������� ���� ����Ƽ�� �ؽ�ó �������� ���� ��� ������ Mipmap level���� �ش�.
	//Shader resource view�� �����Ǹ� GenerateMipmaps�� ȣ���ϰ� �̰��� Mipmap���� ������ش�.

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
/// Helper �Լ�, ���̴��� �������� ���� �ʿ���ϴ� ���� ���
/// </summary>
ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

/// <summary>
/// Target �̹����� �ε��ϴ� �Լ���. Targa �����̴� Upside down �������� ����Ǳ� ������ �̰��� ����ϱ� ���� �ѹ� ������ �ʿ䰡 �ִ�.
/// �ε��� ���� �迭�� ���� �� m_targaData�� �ùٸ� �迭�� �ٽ� ���� ���̴�. 32��Ʈ targa���ϸ� �ٷ�� 24��Ʈ�� ����� targa�� �źεȴ�.
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

