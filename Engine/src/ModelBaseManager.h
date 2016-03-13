#pragma once

#include <Manager.h>


class ModelBase;
class Heap;
struct ID3D11Device;

class ModelBaseManager : public Manager
{
public:
	static ModelBaseManager* Instance();

	static void Create( Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize );
	static void Destroy();

	ModelBase* Add( ID3D11Device* device, const char* archiveFilename );




protected:
	virtual ManagedObject* Make_Object() const override;
	virtual void Delete_Object( ManagedObject* obj ) const override;


private:
	ModelBaseManager( uint32_t initialReserve, uint32_t refillSize );
	virtual ~ModelBaseManager();


	ModelBaseManager();
	ModelBaseManager( const ModelBaseManager& manager );
	ModelBaseManager& operator=( const ModelBaseManager& manager );

private:
	Heap* heap;

	static ModelBaseManager* instance;
};