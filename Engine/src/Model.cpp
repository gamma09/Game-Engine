#include "Model.h"
#include "ModelBase.h"

#include "DrawInfo.h"
#include "Bone.h"
#include "Animation.h"
#include "BoneManager.h"

void Model::Set(ModelBase* _baseModel)
{
	this->baseModel = _baseModel;
	this->activeTexture = 0;
	this->world.set(IDENTITY);

	this->baseModel->Add_Reference();
	Bone* child = this->baseModel->Create_Skeleton_From_Model();
	this->setChild(child);
	child->setParent(this);

	this->Start_Animation(0, 0);
	this->Update_Animation(0);
	child->Bind_Pose(true);
	this->Stop_Animation();
}

void Model::Reset()
{
	this->baseModel->Remove_Reference();
	Bone* bone = static_cast<Bone*>(this->getChild());
	if (bone != 0)
		BoneManager::Instance()->Remove(bone);

	this->baseModel = 0;
	this->currentAnim = -1;
	this->currentAnimDuration = 0;
	this->currentAnimStartFrame = 0;
}

Model::Model() :
	baseModel(0),
	activeTexture(0),
	world(),
	currentAnim(-1),
	currentAnimDuration(0),
	currentAnimStartFrame(0)
{
	// Do nothing
}

Model::~Model()
{
	// Do nothing
}

const Matrix& Model::Get_Matrix() const
{
	return this->world;
}

void Model::Draw(DrawInfo& info) const
{
	info.worldMatrix = &this->world;
	info.model = this->baseModel;
	info.activeTexture = this->activeTexture;

	Bone* child = static_cast<Bone*>(this->getChild());
	if (child != 0)
		child->Draw(info);
}

void Model::Set_World(const Matrix& world)
{
	this->world = world;
}

void Model::Change_Active_Texture(const unsigned int activeTexture)
{
	this->activeTexture = activeTexture;
}

void Model::Start_Animation(const uint32_t time, const uint32_t animationID)
{
	this->currentAnimStartFrame = time;
	this->currentAnim = animationID;
	this->currentAnimDuration = this->baseModel->Get_Animation(animationID).Get_Animation_Length();
}

void Model::Update_Animation(const uint32_t time)
{
	if (this->currentAnim != -1)
	{
		uint32_t delta = time - this->currentAnimStartFrame;
		while (delta > this->currentAnimDuration)
		{
			delta -= this->currentAnimDuration;
			this->currentAnimStartFrame += this->currentAnimDuration;
		}

		Bone* child = static_cast<Bone*>(this->getChild());
		if (child != 0)
			child->Set_Frame(this->baseModel, this->currentAnim, delta);
	}
}

void Model::Stop_Animation()
{
	this->currentAnim = -1;
}