#pragma once

#include <GL/glew.h>
#include <ManagedObject.h>
#include <ManagedObjectCondition.h>

#include "ReferencedObject.h"

class ShaderManager;
class ShaderNameMatcher;

class Shader : public ManagedObject, public ReferencedObject
{
public:

	enum Name
	{
		TEXTURE_LIGHT,
		TEXTURE,
		WIREFRAME,
		Not_Initialized
	};


public:

	virtual void Set(const Name& name, const char* const shaderFileAsset);
	virtual void Reset();

	void Activate() const;
	GLuint GetUniform(const char* name) const;

protected:
	virtual void Free_Me();
		
private:
	Shader();
	Shader(const Shader& shader);
	Shader& operator=(const Shader& shader);
	virtual ~Shader();

private:
	Name name;
	GLuint program;
	

	friend class ShaderNameMatcher;
	friend class ShaderManager;
};