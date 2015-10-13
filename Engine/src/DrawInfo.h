#pragma once

#include <cstdint>

class Matrix;
class Camera;
class ModelBase;
class DirectionLight;

struct DrawInfo
{
	const Matrix* worldMatrix;
	Camera* camera;
	ModelBase* model;
	uint32_t activeTexture;
	DirectionLight* light;
	uint32_t boneIndex;

	DrawInfo() { }
	~DrawInfo() { }
};