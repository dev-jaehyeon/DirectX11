#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	//#4
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& _ApplicationClass)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFilename[128];

	bool result;

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass();

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//#4
	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Create and initialize the model object.
	m_Model = new ModelClass;

	// Set the name of the texture file that we will be loading.
	strcpy_s(textureFilename, "../Assets/stone01.tga");
	m_Model->hwndTemp = hwnd;
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);

	//ColorShaderClass는 이제 사용되지 않음

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the texture shader object.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}
	return true;
}

void ApplicationClass::Shutdown()
{
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}

bool ApplicationClass::Frame()
{
	bool result;


	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

/// <summary>
/// Render함수는 튜토리얼4에서 Camera, Model, ColorShader가 추가되면서 많은 부분이 추가된다.
/// 우선 씬을 클리어하는 것으로 시작한다. 그 다음은 Camera Object의 Render함수를 호출한다. 이것은 카메라의 위치와 회전을 기반으로
/// View Matrix를 만들어낸다. View Matrix가 생성되고 나면, 복제한다. D3DClass 오브젝트로부터 projection Matrix와 World Matrix를 얻어낸다.
/// 그 다음에는 ModelClass의 Render 함수를 호출하여 초록색 삼각형 지오매트리를 그래픽스 파이프라인에 놓는다. 
/// vertex들이 준비되고 Model의 정보를 사용하여 color Shader를 호출한다. 위 3개의 매트릭스들은 각 vertex들의 위치를 포지셔닝한다.
/// 그러면 back buffer에 초록색 삼각형이 그려지게 된다. 그러면 이제 씬은 완료되었고, EndScene을 호출하여 스크린에 출력한다.
/// </summary>
/// <returns></returns>
bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the texture shader.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}
