#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	//m_MultiTextureShader = 0;
	m_Model = 0;
	m_TextureShader = 0;
	m_Texture = 0;
	m_Shader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128], textureFilename1[128], textureFilename2[128];
	bool result;
	m_hwnd = hwnd;

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the camera object.
	m_Camera = new CameraClass;

	m_Camera->SetPosition(0.0f, 0.0f, -3.0f);
	m_Camera->Render();

	//// Create and initialize the multitexture shader object.
	//m_MultiTextureShader = new MultiTextureShaderClass;

	//result = m_MultiTextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the multitexture shader object.", L"Error", MB_OK);
	//	return false;
	//}

	//m_TextureShader = new TextureShaderClass();
	//result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	// Set the file name of the model.
	strcpy_s(modelFilename, "../Assets/square.txt");

	// Set the file name of the textures.
	strcpy_s(textureFilename1, "../Assets/stone01.tga");
	strcpy_s(textureFilename2, "../Assets/dirt01.tga");

	const wchar_t* textureFilename3 = L"../Assets/Magnus.png";
	const wchar_t* textureFilename4 = L"../Assets/noise01.png";
	const wchar_t* textureFilename5 = L"../Assets/shadertoytexture0.jpg";
	const wchar_t* textureFilenames[] = { L"../Assets/shadertoytexture0.jpg" };

	m_Shader = new ShaderClass();
	//result = m_Shader->InitializeShaderToyShader(m_Direct3D->GetDevice(), hwnd);
	result = m_Shader->InitializeShaderToyTextureShader(m_Direct3D->GetDevice(), hwnd, L"PS_SequenceStar.hlsl", textureFilenames, 1);
	//result = m_Shader->InitializeTextureShader(m_Direct3D->GetDevice(), hwnd, textureFilename4);
	//result = m_Shader->TEST_InitSToyOneTextureShader(m_Direct3D->GetDevice(), hwnd, L"PS_SequenceStar.hlsl", textureFilename5);

	if (!result)
	{
		MessageBox(hwnd, L"Initialize ShaderToy FAiled", L"Error", MB_OK);
		return false;
	}
	// Create and initialize the model object.

	m_Model = new ModelClass();
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename);
	//m_Texture = new TextureClass();
	//m_Texture->InitializeWIC(m_Direct3D->GetDevice(), textureFilename3);
	if (!result)
	{
		MessageBox(hwnd, L"Model FAiled", L"Error", MB_OK);
		return false;
	}
	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	//// Release the multitexture shader object.
	//if (m_MultiTextureShader)
	//{
	//	m_MultiTextureShader->Shutdown();
	//	delete m_MultiTextureShader;
	//	m_MultiTextureShader = 0;
	//}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	if (m_Shader)
	{
		m_Shader->Shutdown();
		delete m_Shader;
		m_Shader = 0;
	}

	return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
	bool result;


	// Check if the user pressed escape and wants to exit the application.
	if (Input->IsEscapePressed())
	{
		return false;
	}

	// Render the graphics scene.
	result = Render(m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(HWND hwnd)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result = true;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Render the mouse text strings using the font shader.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Texture->GetTexture());
	/*result = m_MultiTextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(0), m_Model->GetTexture(1));*/
	//result = m_MultiTextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0), m_Model->GetTexture(1));
	result = m_Shader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Render Failed", L"Error", MB_OK);
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}