#pragma once

#include <cstdint>

#include <GL/gl3w.h>

struct Mesh
{
	GLuint vao;
	GLuint vboVertices;
	GLuint vboIndices;

	uint32_t triangleVertexCount;
};