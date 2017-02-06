#pragma once

#include "DirectXMath.h"

class Camera
{
public:
	enum Direction
	{
		FORWARD,
		LEFT,
		BACKWARD,
		RIGHT,
		UP,
		DOWN
	};

	Camera();
	~Camera();

	void Update();

	DirectX::XMFLOAT3 Position() const { return position; }
	void Position(const DirectX::XMFLOAT3 &val) { position = val; }

	DirectX::XMFLOAT3 Rotation() const { return rotation; }
	void Rotation(const DirectX::XMFLOAT3 &val) { rotation = val; }

	DirectX::XMFLOAT3 Up() const { return up; }
	void Up(const DirectX::XMFLOAT3 &val) { up = val; }

	DirectX::XMFLOAT3 Target() const { return target; }
	void Target(const DirectX::XMFLOAT3 &val) { target = val; }

	DirectX::XMFLOAT4X4 ViewMatrix() const { return viewMatrix; }

	void Move(Direction, float dt);
	void Rotate(Direction, float dt);

	bool RotateLock() const { return rotateLock; }
	void RotateLock(bool val) { rotateLock = val; }

private:
	DirectX::XMFLOAT3 position, rotation, up, target;
	DirectX::XMFLOAT4X4 viewMatrix;

	float yaw, pitch, roll, stepDistance, rotateRate;
	bool rotateLock;
};

