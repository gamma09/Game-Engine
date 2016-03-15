#pragma once

#include <MathEngine.h>
#include <ManagedObject.h>
#include <Reflection.h>

#include "ReferencedObject.h"
#include "Moveable.h"
#include "ReflectedVect.h"

struct PerspectiveData
{
	// field of view in radians for the Y-component
	float fov_Y;
	float aspect_ratio;

	// distance from camera to near clipping plane
	float near_distance;

	// distance from camera to far clipping plane
	float far_distance;
};

struct OrientationData
{
	REFLECTED_CLASS( OrientationData );

	REFLECTED( ReflVect, pos );
	REFLECTED( ReflVect, lookAt );
	REFLECTED( ReflVect, up );
};

class Camera : public ManagedObject, public ReferencedObject, public Moveable
{
public:

	void Set( const PerspectiveData& perspective, const OrientationData& orientation );
	virtual void Reset();

	bool Should_Be_Drawn( const Vect& position, float boundingRadius ) const;

	Vect Get_Position() const;
	void Set_Position( const Vect& pos );

	const Matrix& Get_Projection_View() const;
	const Matrix& Get_Projection() const;
	const Matrix& Get_View() const;

	virtual void Update_Position( uint32_t time ) override;

protected:
	virtual void Free_Me() override;

private:
	friend class CameraManager;

	// only CameraManager can create or destroy a camera
	Camera();
	virtual ~Camera();

	// Copying of a camera is not allowed
	Camera( const Camera& cam ) = delete;
	Camera& operator=( const Camera& cam ) = delete;


private:
	// Internal helper methods
	void _set_perspective( const PerspectiveData& perspective );
	void _set_orientation( const OrientationData& orientation );

	void _calculate_frustum();
	void _calculate_view_matrix();
	void _calculate_projection_view_matrix();


private:

	Matrix projection;
	Matrix view;
	Matrix projection_view_matrix;


	/********************************************************************************
	 * facing is actually the reverse of where the camera is pointing
	 * in the below picture, the camera is pointing down, so facing is pointing up
	 *    ^ Facing
	 *    |
	 *    C
	 *   / \
	 *  /   \
	 * /     \
	 ********************************************************************************/
	Vect pos;
	Vect facing;
	Vect right;
	Vect up;

	// world space coords for viewing frustum
	Vect nearTopLeft;
	Vect nearTopRight;
	Vect nearBottomLeft;
	Vect nearBottomRight;
	Vect farTopLeft;
	Vect farTopRight;
	Vect farBottomLeft;
	Vect farBottomRight;

	// Normals of the frustum
	Vect frontNorm;
	Vect backNorm;
	Vect rightNorm;
	Vect leftNorm;
	Vect topNorm;
	Vect bottomNorm;

	// Screen size in world space
	float near_height;
	float near_width;
	float far_height;
	float far_width;

	float near_distance;
	float far_distance;
};