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
public:
    HWND hwndTemp;
private:
    //We define the Targa file header structure here to make reading in the data easier.
    //tga ������ ���� ���� �� �ְ� ����ü�� �����.
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
    //Here we have our Targa reading function.If you wanted to support more formats you would add reading functions here.
    //Tga ������ �д� �Լ���.
    bool LoadTarga32Bit(char*);

private:
    //This class has five member variables.The first one holds the raw Targa data read straight in from the file.The second variable called m_texture will hold the structured texture data that DirectX will use for rendering.And the third variable is the resource view that the shader uses to access the texture data when drawing.The width and height are the dimensions of the texture.
    //�� Ŭ������ �ټ� ��� ������ ������.
    //ù ��°�� ���Ͽ��� ���� �ε�� raw Targa data�̴�.
    //�� ��°�� m_Texture��, ����ȭ�� �ؽ�ó�̸� Dx�� �������� �� ����Ѵ�.
    //�� ��°�� shader�� ����ϱ����� �����ϴ� resource view�̴�.
    //������ ���̿� ���̴�.
    unsigned char* m_targaData;
    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;
    int m_width, m_height;

};

#endif
