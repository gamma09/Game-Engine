#pragma once
#include <GL/glew.h>

class Shader;
struct DrawInfo;
class Heap;

class TextureMaterial
{
public:
	static void Create_Material(Heap* materialHeap);
	static void Destroy_Material();

	static void Draw(const DrawInfo& info);

private:
	TextureMaterial();
	~TextureMaterial();

	TextureMaterial(const TextureMaterial& material);
	TextureMaterial& operator=(const TextureMaterial& material);


private:
	Shader* shader;
	GLint world_location;
	GLint projection_view_location;
	GLint texture_unit_location;

	static TextureMaterial* instance;
};