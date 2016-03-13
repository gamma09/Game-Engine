#pragma once

#include <cstdint>

struct ID3D11Buffer;

struct Mesh
{
	ID3D11Buffer* verticesBuffer;
	ID3D11Buffer* indicesBuffer;

	uint32_t triangleVertexCount;
};