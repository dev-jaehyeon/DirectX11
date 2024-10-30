#pragma once
/// <summary>
/// TextureClass는 로딩, 언로딩 그리고 텍스처 자원에 대한 접근을 캡슐화한다. 반드시 초기화한 다음에 사용되어야 한다.
/// </summary>

#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>

#include "WICTextureLoader.h"

using namespace DirectX;

class TextureClass
{
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool InitializeTarga(ID3D11Device*, ID3D11DeviceContext*, char*);
	bool InitializeWIC(ID3D11Device* _device, const wchar_t* _filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	int GetWidth();
	int GetHeight();

	void InitHWND(HWND hwnd);

private:
	bool LoadTarga32Bit(char*);

private:
	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
	int m_width, m_height;
	HWND m_hwnd;
};

#endif
