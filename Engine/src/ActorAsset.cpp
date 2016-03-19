#include "ActorAsset.h"
#include "Actor.h"
#include "ActorManager.h"
#include "MemorySetup.h"

ActorAsset::ActorAsset()
	: Asset(),
	name( nullptr ),
	model(),
	actor( nullptr ),
	x( 0.0f ),
	y( 0.0f ),
	z( 0.0f ),
	rx( 0.0f ),
	ry( 0.0f ),
	rz( 0.0f ),
	sx( 1.0f ),
	sy( 1.0f ),
	sz( 1.0f ),
	next( nullptr )
{
	// Do nothing
}

ActorAsset::~ActorAsset()
{
	if( this->actor != nullptr )
	{
		this->actor->Remove_Reference();
	}

	if( this->name != nullptr )
	{
		delete this->name;
	}
}

ActorAsset::ActorAsset( const char* name, const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy )
	: model( model ),
	actor( nullptr ),
	x( 0.0f ),
	y( 0.0f ),
	z( 0.0f ),
	rx( 0.0f ),
	ry( 0.0f ),
	rz( 0.0f ),
	sx( 1.0f ),
	sy( 1.0f ),
	sz( 1.0f ),
	next( nullptr )
{
	GameAssert( name != nullptr );

	size_t nameSize = strlen( name ) + 1;
	this->name = newArray( char, nameSize, AssetHeap::Instance(), ALIGN_4 );
	strcpy_s( this->name, nameSize, name );

	this->actor = ActorManager::Instance()->Add( material, this->model.GetModelBase(), updateStrategy );
	this->actor->Add_Reference();

	this->actor->position.set( this->x, this->y, this->z );
	this->actor->sx = this->sx;
	this->actor->sy = this->sy;
	this->actor->sz = this->sz;
	this->actor->rx = this->rx;
	this->actor->ry = this->ry;
	this->actor->rz = this->rz;
}

Actor* ActorAsset::GetActor() const
{
	return this->actor;
}

bool ActorAsset::operator==( const ActorAsset& actor ) const
{
	return strcmp( this->name, actor.name ) == 0;
}

void ActorAsset::Update( uint32_t totalTimeMillis ) const
{
	this->actor->Update( totalTimeMillis );
}

void ActorAsset::Draw( DrawInfo& info ) const
{
	this->actor->Draw( info );
}