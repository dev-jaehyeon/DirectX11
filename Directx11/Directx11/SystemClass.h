#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

//WIN32_LEAN_AND_MEAN ������ �� ������ �ʴ� Win32 ������ϵ��� ����� �ٿ��� ���� �ӵ��� �÷��ش�.
#define WIN32_LEAN_AND_MEAN
#pragma once

#include <windows.h>

#include "inputclass.h"
#include "applicationclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass& _SystemClass);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;

#endif