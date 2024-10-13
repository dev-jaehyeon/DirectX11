#pragma once
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include "lightshaderclass.h"
#include "lightclass.h"

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
#include "CameraClass.h"
#include "ModelClass.h"
#include "MultiTextureShaderClass.h"
#include "InputClass.h"

#include "ModelClass2.h"
#include "TextureShaderClass.h"

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*);

private:
	bool Render(HWND hwnd);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	MultiTextureShaderClass* m_MultiTextureShader;
	ModelClass* m_Model;
	ModelClass2* m_Model2;
	TextureShaderClass* m_TextureShader;
	TextureClass* m_Texture;
	HWND m_hwnd;
};

#endif

//Tutorial4:
//ApplicationClass�� ���� 3���� Ŭ������ �߰��ȴ�. ApplicationClass�� �ٸ� �ʿ��� Ŭ�������� ����Ͽ� ���� �������ϴ� ���� Ŭ�����̴�.