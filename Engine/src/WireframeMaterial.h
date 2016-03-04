#pragma once
#include <GL/glew.h>

class Shader;
struct DrawInfo;
class Heap;

class WireframeMaterial
{
public:
	static void Create_Material(Heap* materialHeap);
	static void Destroy_Material();

	static void Draw(const DrawInfo& info);

private:
	WireframeMaterial();
	~WireframeMaterial();

	WireframeMaterial(const WireframeMaterial& material);
	WireframeMaterial& operator=(const WireframeMaterial& material);


private:
	Shader* shader;
	GLint world_location;
	GLint projection_view_location;

	static WireframeMaterial* instance;
};