#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	//#4
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;

	//#6
	m_LightShader = 0;
	m_Light = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& _ApplicationClass)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128];
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

	//#8
	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// Set the file name of the model.
	strcpy_s(modelFilename, "../Assets/cube.txt");

	// Create and initialize the model object.
	m_Model = new ModelClass;

	// Set the name of the texture file that we will be loading.
	strcpy_s(textureFilename, "../Assets/stone01.tga");
	m_Model->hwndTemp = hwnd;
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename, modelFilename);


	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

//#6
	//// Create and initialize the texture shader object.
	//m_TextureShader = new TextureShaderClass;

	//result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
	//	return false;
	//}

	//#6 이제 새로운 라이트 셰이더 오브젝트가 생성되고 초기화된다.
	// Create and initialize the light shader object.
	m_LightShader = new LightShaderClass;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	//라이트 오브젝트는 여기서 생성된다. 라이트는 하얀색이며 z축으로 위에서 아래로 내리쬔다.

		// Create and initialize the light object.
	m_Light = new LightClass;

	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 0.0f);

	return true;
}

void ApplicationClass::Shutdown()
{
	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	//#6
	//// Release the texture shader object.
	//if (m_TextureShader)
	//{
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = 0;
	//}

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
	static float rotation = 0.0f;
	bool result;


	// Update the rotation variable each frame.
	rotation -= 0.0174532925f * 0.1f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
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
bool ApplicationClass::Render(float rotation)
{
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
//#8
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix, scaleMatrix, srMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix); 
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//이것이 첫 번째 트랜스폼이 된다. Y축으로 회전하는 회전행렬을 먼저 만들다. 그 다음 평행이동 행렬을 만들다. 그리고 이것은 2유닛 왼쪽으로 갈 것이다.
	//이 두 행렬이 완성되면, SRT순서로 곱한다. 이것을 월드행렬에 넣고 이 월드 행렬을 셰이더에 보낸다.
	rotateMatrix = XMMatrixRotationY(rotation);  // Build the rotation matrix.
	translateMatrix = XMMatrixTranslation(-2.0f, 0.0f, 0.0f);  // Build the translation matrix.

	// Multiply them together to create the final world transformation matrix.
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the light shader.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	//두 번째 행렬은..?
	//이번에는 스케일링이 추가된다.

	scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);  // Build the scaling matrix.
	rotateMatrix = XMMatrixRotationY(rotation);  // Build the rotation matrix.
	translateMatrix = XMMatrixTranslation(2.0f, 0.0f, 0.0f);  // Build the translation matrix.

	// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
	srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
	worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the light shader.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}
	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}
