#include "CalculateSkeletonShader.h"
#include "FileLoader.h"
#include "DirectXAssert.h"
#include "ModelBaseManager.h"
#include "ModelBase.h"

const static char* shaderFile = "CalculateSkeleton.cs.cso";

CalculateSkeletonShader::CalculateSkeletonShader( ID3D11Device* device )
{
	unsigned long csSize;
	char* computeShaderBytes = FileLoader::Load_File( shaderFile, csSize );

	GameCheckFatalDx( device->CreateComputeShader( computeShaderBytes, csSize, nullptr, &this->computeShader ), "Could not create the compute shader to calculate each model's skeleton." );
}

CalculateSkeletonShader::~CalculateSkeletonShader()
{
	this->computeShader->Release();
}

void CalculateSkeletonShader::CalculateSkeletons( ID3D11DeviceContext* context, ID3DUserDefinedAnnotation* annotation ) const
{
	annotation->BeginEvent( L"CalculateSkeletonShader::CalculateSkeletons()" );
	{
		context->CSSetShader( this->computeShader, nullptr, 0 );
		
		for( auto modelBase = ModelBaseManager::Instance()->Active_Iterator(); !modelBase.Is_At_End(); modelBase++ )
		{
			static_cast<ModelBase&>( *modelBase ).Update_Model_Skeletons( context, annotation );
		}
	}
	annotation->EndEvent();
}