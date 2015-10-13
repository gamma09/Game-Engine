#include <assert.h>

#include <MathEngine.h>

#include "WireframeMaterial.h"
#include "ModelBase.h"
#include "ShaderFactory.h"
#include "Shader.h"
#include "Camera.h"
#include "MemorySetup.h"
#include "DrawInfo.h"

WireframeMaterial* WireframeMaterial::instance;

void WireframeMaterial::Create_Material(Heap* materialHeap)
{
	materialHeap;

	assert(instance == 0);

	instance = new(materialHeap, ALIGN_4) WireframeMaterial();
}

void WireframeMaterial::Destroy_Material()
{
	assert(instance != 0);

	delete instance;
}

void WireframeMaterial::Draw(const DrawInfo& info)
{
	assert(instance != 0);

	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	instance->shader->Activate();
	glUniformMatrix4fv(instance->projection_view_location, 1, GL_FALSE, (float *) &info.camera->Get_Projection_View());
	glUniformMatrix4fv(instance->world_location, 1, GL_FALSE, (float *)info.worldMatrix);

	info.model->Draw(info.boneIndex);
}

WireframeMaterial::WireframeMaterial() :
	shader(ShaderFactory::Make_Shader(Shader::Name::WIREFRAME))
{
	this->shader->Add_Reference();
	this->world_location = this->shader->GetUniform("world_matrix");
	this->projection_view_location = this->shader->GetUniform("projection_view_matrix");
}

WireframeMaterial::~WireframeMaterial()
{
	this->shader->Remove_Reference();
}