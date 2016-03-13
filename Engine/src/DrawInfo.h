#pragma once

#include <cstdint>

class Matrix;
class Camera;
class ModelBase;
class DirectionLight;
class Texture;
struct ID3D11DeviceContext;

struct DrawInfo
{
	ID3D11DeviceContext* context;
	const Matrix* worldMatrix;
	Camera* camera;
	ModelBase* model;
	const Texture* texture;
	DirectionLight* light;
	uint32_t boneIndex;

	DrawInfo() { }
	~DrawInfo() { }
};