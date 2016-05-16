#pragma once

struct ID3D11Device;
struct ID3D11ComputeShader;
struct ID3D11DeviceContext;
struct ID3DUserDefinedAnnotation;

class CalculateSkeletonShader
{
public:
	CalculateSkeletonShader( ID3D11Device* device );
	~CalculateSkeletonShader();

	void CalculateSkeletons( ID3D11DeviceContext* context, ID3DUserDefinedAnnotation* annotation ) const;

private:
	ID3D11ComputeShader* computeShader;


private:
	

	CalculateSkeletonShader( const CalculateSkeletonShader& ) = delete;
	CalculateSkeletonShader& operator=( const CalculateSkeletonShader& ) = delete;
};