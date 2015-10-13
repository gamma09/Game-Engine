#pragma once
#include <GL/gl3w.h>

class Shader;
struct DrawInfo;
class Heap;

class LightingTextureMaterial
{
public:
	static void Create_Material(Heap* materialHeap);
	static void Destroy_Material();

	static void Draw(const DrawInfo& info);

private:
	LightingTextureMaterial();
	~LightingTextureMaterial();

	LightingTextureMaterial(const LightingTextureMaterial& material);
	LightingTextureMaterial& operator=(const LightingTextureMaterial& material);


private:
	Shader* shader;
	GLint world_location;
	GLint view_location;
	GLint projection_location;
	GLint texture_unit_location;
	GLint light_position_location;
	GLint light_color_location;

	static LightingTextureMaterial* instance;
};