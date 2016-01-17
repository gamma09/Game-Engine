#include "DrawStrategy.h"

#include <Matrix.h>
#include <GameAssert.h>

#include "ModelBaseManager.h"
#include "WireframeMaterial.h"
#include "TextureMaterial.h"
#include "LightingTextureMaterial.h"
#include "ModelBase.h"
#include "DrawInfo.h"


class AbstDrawStrategy abstract
{
public:
	virtual void Draw(const DrawInfo& info) const abstract;
};

class LightingTextureStrategy : public AbstDrawStrategy
{
public:
	virtual void Draw(const DrawInfo& info) const override
	{
		LightingTextureMaterial::Draw(info);
	}
	
	static AbstDrawStrategy* Instance()
	{
		static LightingTextureStrategy instance;

		return &instance;
	}

private:
	// Only I can create or destroy
	LightingTextureStrategy() { }
	virtual ~LightingTextureStrategy() { }

	// No one may copy!
	LightingTextureStrategy(const LightingTextureStrategy& strat);
	LightingTextureStrategy& operator=(const LightingTextureStrategy& strat);
};




class UnlitTextureStrategy : public AbstDrawStrategy
{
public:
	virtual void Draw(const DrawInfo& info) const override
	{
		TextureMaterial::Draw(info);
	}
	
	static AbstDrawStrategy* Instance()
	{
		static UnlitTextureStrategy instance;

		return &instance;
	}

private:
	// Only I can create or destroy
	UnlitTextureStrategy() { }
	virtual ~UnlitTextureStrategy() { }

	// No one may copy!
	UnlitTextureStrategy(const UnlitTextureStrategy& strat);
	UnlitTextureStrategy& operator=(const UnlitTextureStrategy& strat);
};



class WireframeStrategy : public AbstDrawStrategy
{
public:
	virtual void Draw(const DrawInfo& info) const override
	{
		WireframeMaterial::Draw(info);
	}
	
	static AbstDrawStrategy* Instance()
	{
		static WireframeStrategy instance;

		return &instance;
	}

private:
	// Only I can create or destroy
	WireframeStrategy() { }
	virtual ~WireframeStrategy() { }

	// No one may copy!
	WireframeStrategy(const WireframeStrategy& strat);
	WireframeStrategy& operator=(const WireframeStrategy& strat);
};




class DisabledStrategy : public AbstDrawStrategy
{
public:
	virtual void Draw(const DrawInfo&) const override
	{
		// Do nothing
	}
	
	static AbstDrawStrategy* Instance()
	{
		static DisabledStrategy instance;

		return &instance;
	}

private:
	// Only I can create or destroy
	DisabledStrategy() { }
	virtual ~DisabledStrategy() { }

	// No one may copy!
	DisabledStrategy(const DisabledStrategy& strat);
	DisabledStrategy& operator=(const DisabledStrategy& strat);
};



AbstDrawStrategy* GlobalDrawState::textureStrategy;
AbstDrawStrategy* GlobalDrawState::wireframeStrategy;


void GlobalDrawState::Draw(const DrawInfo& info)
{
	GameAssert(textureStrategy);
	GameAssert(wireframeStrategy);

	textureStrategy->Draw(info);
	wireframeStrategy->Draw(info);
}

void GlobalDrawState::Change_Draw_Types(const TextureDrawType& textureType, const WireframeDrawType& wireframeType)
{
	switch (textureType)
	{
	case DRAW_TEXTURE_AND_LIGHTING:
		textureStrategy = LightingTextureStrategy::Instance();
		break;

	case DRAW_TEXTURE_ONLY:
		textureStrategy = UnlitTextureStrategy::Instance();
		break;

	case DRAW_NO_TEXTURE:
		textureStrategy = DisabledStrategy::Instance();
		break;

	default:
		// Forget to add something to the switch statement?
		GameAssert(0);
		break;
	}

	switch (wireframeType)
	{
	case DRAW_WIREFRAME:
		wireframeStrategy = WireframeStrategy::Instance();
		break;

	case DRAW_NO_WIREFRAME:
		wireframeStrategy = DisabledStrategy::Instance();
		break;

	default:
		// Forget to add something to the switch statement?
		GameAssert(0);
		break;
	}
}

