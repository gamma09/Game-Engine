#include <math.h>
#include <assert.h>

#include <GL/gl3w.h>
#include <Quat.h>

#include "Camera.h"
#include "CameraManager.h"

Camera::Camera() :
	ManagedObject(),
	ReferencedObject(),
	projection(),
	view(),
	projection_view_matrix(),
	pos(),
	facing(),
	right(),
	up(),
	nearTopLeft(),
	nearTopRight(),
	nearBottomLeft(),
	nearBottomRight(),
	farTopLeft(),
	farTopRight(),
	farBottomLeft(),
	farBottomRight(),
	frontNorm(),
	backNorm(),
	rightNorm(),
	leftNorm(),
	topNorm(),
	bottomNorm(),
	near_height(0.0f),
	near_width(0.0f),
	far_height(0.0f),
	far_width(0.0f),
	near_distance(0.0f),
	far_distance(0.0f)
{
	// Do nothing
}

Camera::~Camera()
{
	// Do nothing
}

void Camera::Set(const ViewportData& viewport, const PerspectiveData& perspective, const OrientationData& orientation)
{
	this->_set_viewport(viewport);
	this->_set_perspective(perspective);
	this->_set_orientation(orientation);

	this->_calculate_frustum();
	this->_calculate_projection_view_matrix();
}

void Camera::Reset()
{
	ManagedObject::Reset();

	assert(this->Get_Reference_Count() == 0);
}

void Camera::Free_Me()
{
	CameraManager::Instance()->Remove(this);
}

const bool Camera::Should_Be_Drawn(const Vect& position, const float boundingRadius) const
{
	Vect posFromTopLeftNear = position - this->nearTopLeft;
	if (posFromTopLeftNear.dot(this->topNorm) > boundingRadius)
		return false;
	if (posFromTopLeftNear.dot(this->leftNorm) > boundingRadius)
		return false;
	if (posFromTopLeftNear.dot(this->frontNorm) > boundingRadius)
		return false;

	Vect posFromBotRightFar = position - this->farBottomRight;
	if (posFromBotRightFar.dot(this->rightNorm) > boundingRadius)
		return false;
	if (posFromBotRightFar.dot(this->bottomNorm) > boundingRadius)
		return false;
	if (posFromBotRightFar.dot(this->backNorm) > boundingRadius)
		return false;

	return true;
}

const Vect Camera::Get_Position() const
{
	return Vect(this->pos);
}

void Camera::Set_Position(const Vect& pos)
{
	this->pos = pos;

	this->_calculate_view_matrix();
	this->_calculate_frustum();
	this->_calculate_projection_view_matrix();
}


const Matrix& Camera::Get_Projection_View() const
{
	return this->projection_view_matrix;
}

const Matrix& Camera::Get_Projection() const
{
	return this->projection;
}

const Matrix& Camera::Get_View() const
{
	return this->view;
}

void Camera::Update_Position(const uint32_t& time)
{
	uint32_t timeSinceLast = time - this->lastMovementTime;
	this->lastMovementTime = time;
	const Vect& vel = this->Get_Velocity();
	if (!vel.isZero())
		this->pos += vel * (static_cast<float>(timeSinceLast) / 1000.0f);

	this->_calculate_view_matrix();
	this->_calculate_frustum();
	this->_calculate_projection_view_matrix();
}





//-------------------------------------------------------------------------------------------------
// Internal methods
//-------------------------------------------------------------------------------------------------
void Camera::_set_viewport(const ViewportData& viewport)
{
	glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

void Camera::_set_perspective(const PerspectiveData& perspective)
{
	this->near_distance = perspective.near_distance;
	this->far_distance = perspective.far_distance;

	this->near_height = 2.0f * tanf(0.5f * perspective.fov_Y) * perspective.near_distance;
	this->near_width = this->near_height * perspective.aspect_ratio;

	this->far_height = 2.0f * tanf(perspective.fov_Y * 0.5f) * perspective.far_distance;
	this->far_width = this->far_height * perspective.aspect_ratio;

	// Projection matrix
	// [[ T  0  0  0 ] 
	//  [ 0  U  0  0 ]
	//  [ 0  0  V -1 ]
	//  [ 0  0  W  0 ]
	//
	// where:
	// T = 2 * near_distance / near_width
	// U = 2 * near_distance / near_height
	// V = (near_distance + far_distance) / (near_distance - far_distance)
	// W = 2 * near_distance * far_distance / (near_distance - far_distance)

	this->projection.set(ZERO);

	float A = 2.0f * perspective.near_distance;
	float B =  perspective.near_distance - perspective.far_distance;

	this->projection[m0] = A / this->near_width;
	this->projection[m5] = A / this->near_height;
	this->projection[m10] = (perspective.near_distance + perspective.far_distance) / B;
	this->projection[m11] = -1.0f;

	this->projection[m14] = (A * perspective.far_distance) / B;
}

void Camera::_set_orientation(const OrientationData& orientation)
{
	this->pos = orientation.pos;

	this->facing = orientation.pos - orientation.lookAt;
	this->facing.norm();

	this->right = orientation.up.cross(this->facing);
	this->right.norm();
	
	this->up = this->facing.cross(this->right);
	this->up.norm();

	this->_calculate_view_matrix();
}

void Camera::_calculate_view_matrix()
{
	this->view[m0] =  this->right[x];
	this->view[m1] =     this->up[x];
	this->view[m2] = this->facing[x];
	this->view[m3] = 0.0f;

	this->view[m4] =  this->right[y];
	this->view[m5] =     this->up[y];
	this->view[m6] = this->facing[y];
	this->view[m7] = 0.0f;

	this->view[m8]  =  this->right[z];
	this->view[m9]  =     this->up[z];
	this->view[m10] = this->facing[z];
	this->view[m11] = 0.0f;

	this->view[m12] = -1.0f * this->pos.dot(this->right);
	this->view[m13] = -1.0f * this->pos.dot(this->up);
	this->view[m14] = -1.0f * this->pos.dot(this->facing);
	this->view[m15] = 1.0f;
}

// requires:
// orientation and perspective
void Camera::_calculate_frustum()
{
	this->nearTopLeft		= this->pos - this->facing * this->near_distance + this->up * this->near_height * 0.5f - this->right * this->near_width * 0.5f;
	this->nearTopRight		= this->pos - this->facing * this->near_distance + this->up * this->near_height * 0.5f + this->right * this->near_width * 0.5f;
	this->nearBottomLeft	= this->pos - this->facing * this->near_distance - this->up * this->near_height * 0.5f - this->right * this->near_width * 0.5f;
	this->nearBottomRight	= this->pos - this->facing * this->near_distance - this->up * this->near_height * 0.5f + this->right * this->near_width * 0.5f;
	this->farTopLeft		= this->pos - this->facing * this->far_distance  + this->up * this->far_height * 0.5f  - this->right * this->far_width * 0.5f;
	this->farTopRight		= this->pos - this->facing * this->far_distance  + this->up * this->far_height * 0.5f  + this->right * this->far_width * 0.5f;
	this->farBottomLeft		= this->pos - this->facing * this->far_distance  - this->up * this->far_height * 0.5f  - this->right * this->far_width * 0.5f;
	this->farBottomRight   	= this->pos - this->facing * this->far_distance  - this->up * this->far_height * 0.5f  + this->right * this->far_width * 0.5f;

	// Normals of the frustum around nearTopLeft
	Vect A = this->nearBottomLeft - this->nearTopLeft;
	Vect B = this->nearTopRight - this->nearTopLeft;
	Vect C = this->farTopLeft - this->nearTopLeft;
	
	this->frontNorm = A.cross(B);
	this->frontNorm.norm();

	this->leftNorm = C.cross(A);
	this->leftNorm.norm();
	
	this->topNorm = B.cross(C);
	this->topNorm.norm();

	// Normals of the frustum around farBottomRight
	A = this->farBottomLeft - this->farBottomRight;
	B = this->farTopRight - this->farBottomRight;
	C = this->nearBottomRight - this->farBottomRight;
	
	this->backNorm = A.cross(B);
	this->backNorm.norm();
	
	this->rightNorm = B.cross(C);
	this->rightNorm.norm();

	this->bottomNorm = C.cross(A);
	this->bottomNorm.norm();
}

void Camera::_calculate_projection_view_matrix()
{
	//this->projection_view_matrix = this->projection * this->view;
	this->projection_view_matrix = this->view * this->projection;
}