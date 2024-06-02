#pragma once
/// <summary>
/// TextureClass�� �ε�, ��ε� �׸��� �ؽ�ó �ڿ��� ���� ������ ĸ��ȭ�Ѵ�. �ݵ�� �ʱ�ȭ�� ������ ���Ǿ�� �Ѵ�.
/// </summary>

#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>


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

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	int GetWidth();
	int GetHeight();

private:
	bool LoadTarga32Bit(char*);

private:
	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
	int m_width, m_height;

};

#endif
