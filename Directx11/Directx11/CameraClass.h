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
/// ������� HLSL Shader�� �ۼ��ϰ� vertex�� index buffer�� �����ϰ� ColorShaderClass�� ���� ���̴��� �̿��Ͽ�
/// ���۸� �׷��� �� �ִ�. ���� ī�޶� �����Ͽ� �� ����Ʈ�� �����ؾ� �Ѵ�. ī�޶� Ŭ������ �����Ͽ� DX11�� ��𿡼� ��� ����
/// ���̰� �Ǵ����� �˷���� �Ѵ�. CameraClass�� ���� ī�޶��� ��ġ�� ȸ���� �����Ѵ�. �� ��ġ�� ȸ�� ������ �̿��� 
/// HLSL Shader�� view matrix�� �����Ͽ� �ѱ� ���̴�.
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

//ī�޶� Ŭ������ SetPositon, SetRotation�� ī�޶� ������Ʈ�� ��ġ�� ȸ���� �����Ѵ�. Render�� �� positon�� rotation�� �����
//view matrix�� ����� ����. ���������� GetViewMatrix()�� view matrix�� camera object�κ��� ���̴��� ������ ���̴�.