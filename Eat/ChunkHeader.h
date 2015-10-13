#pragma once

#define CHUNK_NAME_SIZE 20

enum ChunkType
{
	VERTS_TYPE = 0xA0000000,
	NORMS_TYPE,    // 0xA0000001
	ANIM_TYPE,     // 0xA0000002
	TEXTURE_TYPE,  // 0xA0000003
	UV_TYPE,       // 0xA0000004
	MANIFEST_TYPE, // 0xA0000005
	SCENE_TYPE     // 0xA0000006
};

struct ChunkHeader
{
	ChunkType type;
	char chunkName[CHUNK_NAME_SIZE];
	int chunkSize;
	unsigned int hashNum;
};