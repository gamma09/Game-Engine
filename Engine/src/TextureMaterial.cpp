#include <assert.h>

#include <MathEngine.h>
#include "DebuggerSetup.h"

#include "MemorySetup.h"
#include "TextureMaterial.h"
#include "ModelBase.h"
#include "ShaderFactory.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DrawInfo.h"


TextureMaterial* TextureMaterial::instance;

void TextureMaterial::Create_Material(Heap* materialHeap)
{
	materialHeap;

	assert(instance == 0);

	instance = new(materialHeap, ALIGN_4) TextureMaterial();
}

void TextureMaterial::Destroy_Material()
{
	assert(instance != 0);

	delete instance;
}

void TextureMaterial::Draw(const DrawInfo& info)
{
	assert(instance != 0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	instance->shader->Activate();
	glUniformMatrix4fv(instance->projection_view_location, 1, GL_FALSE, (float *) &info.camera->Get_Projection_View());
	glUniformMatrix4fv(instance->world_location, 1, GL_FALSE, (float *)info.worldMatrix);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, info.model->Get_Texture(info.activeTexture)->Get_Texture_ID());
	glUniform1i(instance->texture_unit_location, 0);
	
	info.model->Draw(info.boneIndex);

	glBindTexture(GL_TEXTURE_2D, 0);
}

TextureMaterial::TextureMaterial() :
	shader(ShaderFactory::Make_Shader(Shader::Name::TEXTURE))
{
	this->shader->Add_Reference();
	this->world_location = this->shader->GetUniform("world_matrix");
	this->projection_view_location = this->shader->GetUniform("projection_view_matrix");
	this->texture_unit_location = this->shader->GetUniform("tex_object");
}

TextureMaterial::~TextureMaterial()
{
	this->shader->Remove_Reference();
}