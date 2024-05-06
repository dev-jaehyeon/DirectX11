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

//#5 ModelClass는 이제 textureShaderClass를 가진다.
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
	//#5 ColorShaderClass 는 이제 없다
	TextureShaderClass* m_TextureShader;
};

#endif

//Tutorial4:
//ApplicationClass는 이제 3가지 클래스가 추가된다. ApplicationClass는 다른 필요한 클래스들을 사용하여 씬을 렌더링하는 메인 클래스이다.