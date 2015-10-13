#include <assert.h>

#include "ShaderFactory.h"
#include "ShaderManager.h"


static Shader* Create_Shader(const Shader::Name& name);

Shader* ShaderFactory::Make_Shader(const Shader::Name& name)
{
	Shader* shader = static_cast<Shader*>(ShaderManager::Instance()->Find(name));
	if (shader == 0)
		shader = Create_Shader(name);

	return shader;
}



//---------------------------------------------------------------------------------------------------------
// Internal methods
//---------------------------------------------------------------------------------------------------------
static Shader* Create_Shader(const Shader::Name& name)
{
	char* shaderFileBase = 0;

	switch (name)
	{
	case Shader::Name::TEXTURE_LIGHT:
		shaderFileBase = "LightingTexture";
		break;

	case Shader::Name::TEXTURE:
		shaderFileBase = "Texture";
		break;

	case Shader::Name::WIREFRAME:
		shaderFileBase = "Wireframe";
		break;

	default:
		// Shader not recognized!
		assert(0);
		break;
	}

	Shader* shader = ShaderManager::Instance()->Add(name, shaderFileBase);
	assert(shader != 0);

	return shader;
}