#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& _ApplicationClass)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass();

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}


	return true;
}

void ApplicationClass::Shutdown()
{
}

bool ApplicationClass::Frame()
{
	return true;
}

bool ApplicationClass::Render()
{
	return true;
}
