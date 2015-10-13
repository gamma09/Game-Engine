#pragma once

struct DrawInfo;

enum TextureDrawType
{
	DRAW_TEXTURE_AND_LIGHTING,
	DRAW_TEXTURE_ONLY,
	DRAW_NO_TEXTURE
};

enum WireframeDrawType
{
	DRAW_WIREFRAME,
	DRAW_NO_WIREFRAME
};

class AbstDrawStrategy;

class GlobalDrawState
{
public:
	static void Draw(const DrawInfo& info);
	static void Change_Draw_Types(const TextureDrawType& textureType, const WireframeDrawType& wireframeType);

private:
	static AbstDrawStrategy* textureStrategy;
	static AbstDrawStrategy* wireframeStrategy;
};
