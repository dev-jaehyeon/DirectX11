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


class TextureClass
{
public:
    HWND hwndTemp;
private:
    //We define the Targa file header structure here to make reading in the data easier.
    //tga 파일을 쉽게 읽을 수 있게 구조체로 만든다.
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
    //Tga 파일을 읽는 함수다.
    bool LoadTarga32Bit(char*);

private:
    //This class has five member variables.The first one holds the raw Targa data read straight in from the file.The second variable called m_texture will hold the structured texture data that DirectX will use for rendering.And the third variable is the resource view that the shader uses to access the texture data when drawing.The width and height are the dimensions of the texture.
    //이 클래스는 다섯 멤버 변수를 가진다.
    //첫 번째는 파일에서 직접 로드된 raw Targa data이다.
    //두 번째는 m_Texture로, 구조화된 텍스처이며 Dx가 렌더링할 때 사용한다.
    //세 번째는 shader가 사용하기위해 접근하는 resource view이다.
    //나머진 넓이와 높이다.
    unsigned char* m_targaData;
    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;
    int m_width, m_height;

};

#endif
