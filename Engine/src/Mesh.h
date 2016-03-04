#pragma once

#include <cstdint>

#include <GL/glew.h>

struct Mesh
{
	GLuint vao;
	GLuint vboVertices;
	GLuint vboIndices;

	uint32_t triangleVertexCount;
};