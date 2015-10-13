#include <MathEngine.h>
#include <ManagedObject.h>

#include "ReferencedObject.h"

// Direction light will always point at the origin
class DirectionLight : public ManagedObject, public ReferencedObject
{
public:
	void Set(float x, float y, float z, float r, float g, float b, float a);
	virtual void Reset();

	const Vect& Get_Position() const;
	const Vect& Get_Color() const;

protected:
	virtual void Free_Me();

private:
	// Only DirectionLightManager allowed to create or destroy this object
	DirectionLight();
	virtual ~DirectionLight();

	friend class DirectionLightManager;

	// Copying not allowed!
	DirectionLight(const DirectionLight& light);
	DirectionLight& operator=(const DirectionLight& light);


private:
	Vect position;
	Vect color;
};