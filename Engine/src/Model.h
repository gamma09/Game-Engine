#pragma once

#include <MathEngine.h>

#include "HierarchyElement.h"

class ModelBase;
class Material;
struct DrawInfo;


class Model : public HierarchyElement
{
public:

	void Set( const Material* material, ModelBase* baseModel );
	virtual void Reset();

	virtual const Matrix& Get_Matrix() const override;

	void Draw( DrawInfo& info ) const;
	void Set_World( const Matrix& world );
	void Change_Active_Texture( uint32_t activeTexture );

	void Start_Animation( uint32_t time, uint32_t animationID );
	void Update_Animation( uint32_t time );
	void Stop_Animation();

private:
	// Only Actor is allowed to create, copy, or destroy models
	Model();
	Model( const Model& model );
	Model& operator=( const Model& model );
	~Model();

	friend class Actor;


private:
	Matrix world;
	ModelBase* baseModel;
	const Material* material;
	uint32_t activeTexture;

	uint32_t currentAnimStartFrame;
	uint32_t currentAnimDuration;
	int32_t currentAnim;
};

