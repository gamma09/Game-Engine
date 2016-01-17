#include <GameAssert.h>

#include <MathEngine.h>

#include "MemorySetup.h"
#include "LightingTextureMaterial.h"
#include "ModelBase.h"
#include "ShaderFactory.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionLight.h"
#include "DrawInfo.h"


LightingTextureMaterial* LightingTextureMaterial::instance;

void LightingTextureMaterial::Create_Material(Heap* materialHeap)
{
	materialHeap;

	GameAssert(instance == 0);

	instance = new(materialHeap, ALIGN_4) LightingTextureMaterial();
}

void LightingTextureMaterial::Destroy_Material()
{
	GameAssert(instance != 0);

	delete instance;
}

void LightingTextureMaterial::Draw(const DrawInfo& info)
{
	GameAssert(instance != 0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	instance->shader->Activate();
	glUniformMatrix4fv(instance->projection_location, 1, GL_FALSE, (float *) &info.camera->Get_Projection());
	glUniformMatrix4fv(instance->view_location, 1, GL_FALSE, (float *) &info.camera->Get_View());
	glUniformMatrix4fv(instance->world_location, 1, GL_FALSE, (float *)info.worldMatrix);
	glUniform4fv(instance->light_color_location, 1, (float *) &info.light->Get_Color());
	glUniform3fv(instance->light_position_location, 1, (float *) &info.light->Get_Position());
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, info.model->Get_Texture(info.activeTexture)->Get_Texture_ID());
	glUniform1i(instance->texture_unit_location, 0);
	
	info.model->Draw(info.boneIndex);

	glBindTexture(GL_TEXTURE_2D, 0);
}

LightingTextureMaterial::LightingTextureMaterial() :
	shader(ShaderFactory::Make_Shader(Shader::Name::TEXTURE_LIGHT))
{
	this->shader->Add_Reference();
	this->projection_location = this->shader->GetUniform("projection_matrix");
	this->view_location = this->shader->GetUniform("view_matrix");
	this->world_location = this->shader->GetUniform("world_matrix");
	this->light_position_location = this->shader->GetUniform("light_position");
	this->light_color_location = this->shader->GetUniform("light_color");
	this->texture_unit_location = this->shader->GetUniform("texture_unit");
}

LightingTextureMaterial::~LightingTextureMaterial()
{
	this->shader->Remove_Reference();
}