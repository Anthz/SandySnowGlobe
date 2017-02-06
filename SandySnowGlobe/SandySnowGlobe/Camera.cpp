#include "Camera.h"

Camera::Camera()
{
	position = { 0, 0, -5.0f };
	rotation = { 0, 0, 0 };
	up = { 0, 1, 0 };
	target = { 0, 0, 1 };
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	rotateLock = true;
	stepDistance = 20.0f;
	rotateRate = 30.0f;
}


Camera::~Camera()
{
}

void Camera::Update()
{
	DirectX::XMMATRIX mRMatrix, mVMatrix;
	DirectX::XMVECTOR vTarget, vUp, vPos;

	pitch = rotation.x * DirectX::XM_PI / 180.0f;
	yaw = rotation.y * DirectX::XM_PI / 180.0f;
	roll = rotation.z * DirectX::XM_PI / 180.0f;

	mRMatrix =  DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	vTarget = DirectX::XMLoadFloat3(&target);
	vTarget = DirectX::XMVector3TransformCoord(vTarget, mRMatrix);

	vUp = DirectX::XMLoadFloat3(&up);
	vUp = DirectX::XMVector3TransformCoord(vUp, mRMatrix);

	vPos = DirectX::XMLoadFloat3(&position);
	vTarget = DirectX::XMVectorAdd(vPos, vTarget);

	mVMatrix = DirectX::XMMatrixLookAtLH(vPos, vTarget, vUp);
	DirectX::XMStoreFloat4x4(&viewMatrix, mVMatrix);
}

void Camera::Move(Direction d, float dt)
{
	DirectX::XMMATRIX mRMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	DirectX::XMVECTOR v;
	DirectX::XMFLOAT3 tempPos = { 0, 0, 0 };

	switch(d)
	{
		case Camera::FORWARD:
			tempPos.z += stepDistance * dt;
			break;
		case Camera::LEFT:
			tempPos.x -= stepDistance * dt;
			break;
		case Camera::BACKWARD:
			tempPos.z -= stepDistance * dt;
			break;
		case Camera::RIGHT:
			tempPos.x += stepDistance * dt;
			break;
		case UP:
			tempPos.y += stepDistance * dt;
			break;
		case DOWN:
			tempPos.y -= stepDistance * dt;
			break;
	}

	v = DirectX::XMLoadFloat3(&tempPos);
	v = DirectX::XMVector3TransformCoord(v, mRMatrix);
	DirectX::XMStoreFloat3(&tempPos, v);
	position.x += tempPos.x;
	position.y += tempPos.y;
	position.z += tempPos.z;
}

void Camera::Rotate(Direction d, float dt)
{
	switch(d)
	{
		case Camera::FORWARD:
			rotation.x -= rotateRate * dt;
			break;
		case Camera::LEFT:
			rotation.y -= rotateRate * dt;
			break;
		case Camera::BACKWARD:
			rotation.x += rotateRate * dt;
			break;
		case Camera::RIGHT:
			rotation.y += rotateRate * dt;
			break;
	}
}
