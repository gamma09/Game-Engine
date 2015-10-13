#pragma once

#include "Shader.h"

class ShaderFactory
{
public:
	static Shader* Make_Shader(const Shader::Name& name);





private:
	ShaderFactory();
	ShaderFactory(const ShaderFactory& factory);
	ShaderFactory& operator=(const ShaderFactory& factory);
	~ShaderFactory();
};