#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
using namespace DirectX;

/// <summary>
/// 여기까지 HLSL Shader를 작성하고 vertex와 index buffer를 세팅하고 ColorShaderClass를 통해 셰이더를 이용하여
/// 버퍼를 그려낼 수 있다. 이제 카메라를 정의하여 뷰 포인트를 설정해야 한다. 카메라 클래스를 정의하여 DX11이 어디에서 어떻게 씬이
/// 보이게 되는지를 알려줘야 한다. CameraClass는 현재 카메라의 위치와 회전을 추적한다. 이 위치와 회전 정보를 이용해 
/// HLSL Shader에 view matrix를 생성하여 넘길 것이다.
/// </summary>
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass& other );
	~CameraClass();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

#endif

//카메라 클래스의 SetPositon, SetRotation은 카메라 오브젝트의 위치와 회전을 설정한다. Render는 그 positon과 rotation에 기반한
//view matrix를 만들어 낸다. 최종적으로 GetViewMatrix()는 view matrix를 camera object로부터 셰이더로 보내는 것이다.