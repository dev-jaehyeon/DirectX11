#pragma once
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "D3DClass.h"
#include <windows.h>

//#4
#include "CameraClass.h"
#include "ModelClass.h"

//#5 ModelClass�� ���� textureShaderClass�� ������.
#include "TextureShaderClass.h"

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass& _ApplicationClass);
	~ApplicationClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_Direct3D;
	//#4
	CameraClass* m_Camera;
	ModelClass* m_Model;
	//#5 ColorShaderClass �� ���� ����
	TextureShaderClass* m_TextureShader;
};

#endif

//Tutorial4:
//ApplicationClass�� ���� 3���� Ŭ������ �߰��ȴ�. ApplicationClass�� �ٸ� �ʿ��� Ŭ�������� ����Ͽ� ���� �������ϴ� ���� Ŭ�����̴�.