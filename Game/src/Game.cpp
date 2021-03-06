#include <cmath>

#include <MemorySetup.h>
#include <GameAssert.h>
#include <Timer.h>
#include <ActorManager.h>
#include <CameraManager.h>
#include <KeyBindingManager.h>
#include <ModelBaseManager.h>
#include <AppInfo.h>
#include <DirectionLightManager.h>
#include <Camera.h>
#include <DirectionLight.h>
#include <DebuggerSetup.h>
#include <ActorAsset.h>

#include <WireframeMaterial.h>

#include "Game.h"



#include "StationaryStrategy.h"
#include "AnimatingStrategy.h"

#include "MoveAction.h"



//-----------------------------------------------------------------------------
//  Game::Game()
//		Game Engine Constructor
//-----------------------------------------------------------------------------
Game::Game( const char* windowName, const int Width, const int Height )
	: Engine( windowName, Width, Height )
{
	// Do nothing
}

//-----------------------------------------------------------------------------
// Game::LoadContent()
//		Allows you to load all content needed for your engine,
//	    such as objects, graphics, etc.
//-----------------------------------------------------------------------------
void Game::LoadContent()
{

	PerspectiveData perspective;
	perspective.fov_Y = 50.0f * MATH_PI_180;
	perspective.aspect_ratio = (float) info[Window_Width] / (float) info[Window_Height];
	perspective.near_distance = 0.1f;
	perspective.far_distance = 1000.0f;

	OrientationData orientation;
	orientation.pos = Vect( 0.0f, 0.0f, 400.0f );
	orientation.lookAt = Vect( 0.0f, 0.0f, 0.0f );
	orientation.up = Vect( 0.0f, 1.0f, 0.0f );

	this->moveableCamera = CameraManager::Instance()->Add( this->device, perspective, orientation );


	// W - move forward
	KeyBindingManager::Instance()->Add( 0x57, new( TemporaryHeap::Instance(), ALIGN_4 ) MoveAction( this->moveableCamera, 0.0f, 0.0f, -1.0f ) );
	// S - move backward
	KeyBindingManager::Instance()->Add( 0x53, new( TemporaryHeap::Instance(), ALIGN_4 ) MoveAction( this->moveableCamera, 0.0f, 0.0f, 1.0f ) );
	// A - move left
	KeyBindingManager::Instance()->Add( 0x41, new( TemporaryHeap::Instance(), ALIGN_4 ) MoveAction( this->moveableCamera, -1.0f, 0.0f, 0.0f ) );
	// D - move right
	KeyBindingManager::Instance()->Add( 0x44, new( TemporaryHeap::Instance(), ALIGN_4 ) MoveAction( this->moveableCamera, 1.0f, 0.0f, 0.0f ) );

	UpdateStrategy* updater = new( TemporaryHeap::Instance(), ALIGN_4 ) AnimatingStrategy();

	this->sceneAsset = new( AssetHeap::Instance(), ALIGN_4 ) SceneAsset( "Test Scene" );

	ModelAsset* model1 = this->sceneAsset->AddModel( this->device, "../resources/humanoid2.spu" );
	ModelAsset* model2 = this->sceneAsset->AddModel( this->device, "../resources/teddy_tga.spu" );
	ModelAsset* model3 = this->sceneAsset->AddModel( this->device, "../resources/2bonepolyskinnedskeleton.spu" );

	ActorAsset* humanoid1 = this->sceneAsset->AddActor( this->device, *model1, this->wireframeMaterial, updater );
	humanoid1->GetActor()->Get_Model().Change_Active_Texture( 0 );
	humanoid1->GetActor()->position.set( -125.0f, 0.0f, -200.0f );
	humanoid1->GetActor()->rotation.set( -MATH_PI2, 0.0f, 0.0f );
	humanoid1->GetActor()->scale.set( 0.2f, 0.2f, 0.2f );
	humanoid1->GetActor()->Update_Model_Matrix();
	humanoid1->GetActor()->Get_Model().Start_Animation( 0, 0 );

	ActorAsset* teddy1 = this->sceneAsset->AddActor( this->device, *model2, this->unlitTextureMaterial, updater );
	teddy1->GetActor()->position.set( 0.0f, -175.0f, 0.0f );
	teddy1->GetActor()->Get_Model().Add_Material( this->wireframeMaterial );
	teddy1->GetActor()->Get_Model().Change_Active_Texture( 0 );
	teddy1->GetActor()->Update_Model_Matrix();
	teddy1->GetActor()->Get_Model().Start_Animation( 0, 0 );

	ActorAsset* teddy2 = this->sceneAsset->AddActor( this->device, *model2, this->unlitTextureMaterial, updater );
	teddy2->GetActor()->Get_Model().Change_Active_Texture( 0 );
	teddy2->GetActor()->position.set( -125.0f, -175.0f, 0.0f );
	teddy2->GetActor()->Update_Model_Matrix();
	teddy2->GetActor()->Get_Model().Start_Animation( 0, 1 );

	ActorAsset* twobone = this->sceneAsset->AddActor( this->device, *model3, this->unlitTextureMaterial, updater );
	twobone->GetActor()->Get_Model().Change_Active_Texture( 0 );
	twobone->GetActor()->position.set( 50.0f, 100.0f, -50.0f );
	twobone->GetActor()->Update_Model_Matrix();
	twobone->GetActor()->Get_Model().Start_Animation( 0, 0 );

	this->light = DirectionLightManager::Instance()->Add( -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f );
	this->light->Add_Reference();
}

const static Time MILLISECOND = Time( TIME_ONE_MILLISECOND );

//-----------------------------------------------------------------------------
// Game::Update()
//      Called once per frame, update data, tranformations, etc
//      Use this function to control process order
//      Input, AI, Physics, Animation, and Graphics
//-----------------------------------------------------------------------------
void Game::Update( uint32_t totalTimeMillis )
{
	Engine::Update( totalTimeMillis );

	this->moveableCamera->Update_Position( totalTimeMillis );
}

//-----------------------------------------------------------------------------
// Game::UnLoadContent()
//       unload content (resources loaded above)
//       unload all content that was loaded before the Engine Loop started
//-----------------------------------------------------------------------------
void Game::UnLoadContent()
{
	this->sceneAsset->DeleteAsset();

	this->light->Remove_Reference();
}
