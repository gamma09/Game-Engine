#include <assert.h>
#include <Windows.h>

#include <File.h>

#include "MemorySetup.h"
#include "DebuggerSetup.h"

#include "Shader.h"
#include "ShaderManager.h"



static GLuint Load_Shader(const char* const fileName, GLenum shaderType);



Shader::Shader() :
	ManagedObject(),
	ReferencedObject(),
	name(Not_Initialized),
	program(0)
{
	// Do nothing
}

Shader::~Shader()
{
	// Do nothing
}

void Shader::Set(const Name& name, const char* const shaderFileAsset)
{
	assert(shaderFileAsset != 0);
	assert(name != Name::Not_Initialized);

	this->name = name;
	char file[256];

	strcpy_s(file, shaderFileAsset);
	strcat_s(file, ".fs.glsl");

		
	GLuint fragmentShader = Load_Shader(file, GL_FRAGMENT_SHADER);

	strcpy_s(file, shaderFileAsset);
	strcat_s(file, ".vs.glsl");

	GLuint vertexShader = Load_Shader(file, GL_VERTEX_SHADER);

	this->program = glCreateProgram();
	glAttachShader(this->program, vertexShader);
	glAttachShader(this->program, fragmentShader);

	glLinkProgram(this->program);
}

void Shader::Reset()
{
	ManagedObject::Reset();

	assert(this->Get_Reference_Count() == 0);

	glDeleteProgram(this->program);
	this->program = 0;
	this->name = Name::Not_Initialized;
}

void Shader::Free_Me()
{
	ShaderManager::Instance()->Remove(this);
}

void Shader::Activate() const
{
	assert(this->name != Name::Not_Initialized);

	glUseProgram(this->program);
}

GLuint Shader::GetUniform(const char* name) const
{
	assert(name != 0);
	GLuint loc = glGetUniformLocation(this->program, name);
	assert(loc != -1);

	return loc;
}





//---------------------------------------------------------------------------------------------------------
// Internal methods
//---------------------------------------------------------------------------------------------------------

#define SHADER_COMPILE_ERROR_BUFFER_SIZE 4096

static GLuint Load_Shader(const char* const fileName, GLenum shaderType)
{
	assert(fileName != 0);
		
	FileHandle file;
	FileError status;

	status = File::open(file, fileName, FILE_READ);
	assert(status == FILE_SUCCESS);
	
	// Read file size
	int size;
	status = File::size(file, size);
	assert(status == FILE_SUCCESS);

	char* data = new(TemporaryHeap::Instance(), ALIGN_4) char[size+1];


	// Read from file
	status = File::read(file, data, size);
	assert(status == FILE_SUCCESS);

	data[size] = 0;
	
	// Close file
	status = File::close(file);
	assert(status == FILE_SUCCESS);

	GLuint shader = glCreateShader(shaderType);
	assert(shader != 0);

	glShaderSource(shader, 1, &data, 0);
	delete[] data;

	GLint compileStatus = 0;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (!compileStatus)
	{
		char buf[SHADER_COMPILE_ERROR_BUFFER_SIZE];
		glGetShaderInfoLog(shader, SHADER_COMPILE_ERROR_BUFFER_SIZE, 0, buf);
		out("Error %s: %s\n", fileName, buf);

		glDeleteShader(shader);

		assert(false);
	}

	return shader;
}