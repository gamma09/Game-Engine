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
#include <WireframeMaterial.h>

#include "Editor.h"

#include "ModelObject.h"
#include "ActorObject.h"
#include "NewSceneUpdateEvent.h"
#include "StationaryStrategy.h"
#include "AnimatingStrategy.h"

#include "MoveAction.h"



//-----------------------------------------------------------------------------
//  Editor::Editor()
//		Editor Engine Constructor
//-----------------------------------------------------------------------------
Editor::Editor( const char* windowName, const int Width, const int Height )
	: Engine( windowName, Width, Height ),
	browser( nullptr ),
	updater( new( AssetHeap::Instance(), ALIGN_4 ) AnimatingStrategy() )
{
	// In addition to the scene references, add one last reference for the editor itself
	this->updater->Add_Reference();
}

//-----------------------------------------------------------------------------
// Editor::LoadContent()
//		Allows you to load all content needed for your engine,
//	    such as objects, graphics, etc.
//-----------------------------------------------------------------------------
void Editor::LoadContent()
{
	// First and foremost, we have to create the content browser
	this->browser = new CLI::Browser();
	this->browser->AddBrowserListener( this );

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

	this->sceneAsset = new( AssetHeap::Instance(), ALIGN_4 ) SceneAsset( "Test Scene" );
	ModelObject* model1 = new( AssetHeap::Instance(), ALIGN_4 ) ModelObject( this->browser, this->sceneAsset, "../resources/humanoid2.spu", this->device );
	ModelObject* model2 = new( AssetHeap::Instance(), ALIGN_4 ) ModelObject( this->browser, this->sceneAsset, "../resources/teddy_tga.spu", this->device );
	ModelObject* model3 = new( AssetHeap::Instance(), ALIGN_4 ) ModelObject( this->browser, this->sceneAsset, "../resources/2bonepolyskinnedskeleton.spu", this->device );

	ActorObject* humanoid1 = new( AssetHeap::Instance(), ALIGN_4 ) ActorObject( this->device, this->browser, this->sceneAsset, *model1->GetModelAsset(), this->wireframeMaterial, this->updater );
	humanoid1->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 0 );
	humanoid1->GetActorAsset()->GetActor()->position.set(-125.0f, 0.0f, -200.0f );
	humanoid1->GetActorAsset()->GetActor()->rotation.set( -MATH_PI2, 0.0f, 0.0f );
	humanoid1->GetActorAsset()->GetActor()->scale.set( 0.2f, 0.2f, 0.2f );
	humanoid1->GetActorAsset()->GetActor()->Update_Model_Matrix();
	humanoid1->GetActorAsset()->GetActor()->Get_Model().Start_Animation( 500, 0 );

	ActorObject* teddy1 = new( AssetHeap::Instance(), ALIGN_4 ) ActorObject( this->device, this->browser, this->sceneAsset, *model2->GetModelAsset(), this->unlitTextureMaterial, this->updater );
	teddy1->GetActorAsset()->GetActor()->Get_Model().Add_Material( this->wireframeMaterial );
	teddy1->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 0 );
	teddy1->GetActorAsset()->GetActor()->position.set( 0.0f, -175.0f, 0.0f );
	teddy1->GetActorAsset()->GetActor()->Update_Model_Matrix();
	teddy1->GetActorAsset()->GetActor()->Get_Model().Start_Animation( 0, 0 );

	ActorObject* twobone = new( AssetHeap::Instance(), ALIGN_4 ) ActorObject( this->device, this->browser, this->sceneAsset, *model3->GetModelAsset(), this->unlitTextureMaterial, this->updater );
	twobone->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 0 );
	twobone->GetActorAsset()->GetActor()->position.set( 50.0f, 100.0f, -50.0f );
	twobone->GetActorAsset()->GetActor()->Update_Model_Matrix();
	twobone->GetActorAsset()->GetActor()->Get_Model().Start_Animation( 0, 0 );

	ActorObject* teddy2 = new( AssetHeap::Instance(), ALIGN_4 ) ActorObject( this->device, this->browser, this->sceneAsset, *model2->GetModelAsset(), this->unlitTextureMaterial, this->updater );
	teddy2->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 0 );
	teddy2->GetActorAsset()->GetActor()->position.set( -125.0f, -175.0f, 0.0f );
	teddy2->GetActorAsset()->GetActor()->Update_Model_Matrix();
	teddy2->GetActorAsset()->GetActor()->Get_Model().Start_Animation( 0, 1 );

	this->light = DirectionLightManager::Instance()->Add( -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f );
	this->light->Add_Reference();
}

//-----------------------------------------------------------------------------
// Editor::Update()
//      Called once per frame, update data, tranformations, etc
//      Use this function to control process order
//      Input, AI, Physics, Animation, and Graphics
//-----------------------------------------------------------------------------
void Editor::Update( uint32_t totalTimeMillis )
{
	Engine::Update( totalTimeMillis );

	this->moveableCamera->Update_Position( totalTimeMillis );
}

//-----------------------------------------------------------------------------
// Editor::UnLoadContent()
//       unload content (resources loaded above)
//       unload all content that was loaded before the Engine Loop started
//-----------------------------------------------------------------------------
void Editor::UnLoadContent()
{
	this->sceneAsset->DeleteAsset();

	this->light->Remove_Reference();
}

void Editor::CloseEngine()
{
	this->browser->Exit();
}


void Editor::OnModelSelected( ContentObject* modelObj )
{
	GameAssert( modelObj != nullptr );
	modelObj;
	// TODO implement model selected
}


void Editor::OnModelDeselected()
{
	// TODO implement model deselected
}

void Editor::OnActorSelected( ContentObject* actorObj )
{
	GameAssert( actorObj != nullptr );
	actorObj;
	// TODO implement actor selected
}

void Editor::OnActorDeselected()
{
	// TODO implement actor deselected
}

const static Time MILLISECOND = Time( TIME_ONE_MILLISECOND );

void Editor::OnActorCreated( ContentObject* modelObj )
{
	GameAssert( modelObj != nullptr );

	ModelObject* model = static_cast<ModelObject*>( modelObj );
	ActorObject* actor = new( AssetHeap::Instance(), ALIGN_4 ) ActorObject( this->device, this->browser, this->sceneAsset, *model->GetModelAsset(), this->unlitTextureMaterial, this->updater );

	actor->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 1 );
	actor->GetActorAsset()->GetActor()->Update_Model_Matrix();
	actor->GetActorAsset()->GetActor()->Get_Model().Start_Animation( Time::quotient( this->totalTime, MILLISECOND ), 0 );
}

void Editor::OnActorDeleted( ContentObject* actorObj )
{
	GameAssert( actorObj != nullptr );

	ActorObject* actor = static_cast<ActorObject*>( actorObj );
	this->sceneAsset->RemoveActor( *actor->GetActorAsset() );
}

void Editor::OnExit()
{
	PostMessage( this->hWindow, WM_ENGINE_DESTROY_WINDOW, 0, 0 );
}

void Editor::OnNewLevel()
{
	this->updateEventManager.AddPreUpdateEvent( new( EventHeap::Instance(), ALIGN_4 ) NewSceneUpdateEvent( *this ) );
}

void Editor::OnOpenLevel( const char* filepath )
{
	GameAssert( filepath != nullptr );
	filepath;
	// TODO implement open level
}

void Editor::OnSaveLevel( const char* filepath )
{
	GameAssert( filepath != nullptr );
	filepath;
	// TODO implement save level
}

void Editor::OnLoadModel( const char* filepath )
{
	GameAssert( filepath != nullptr );

	new( AssetHeap::Instance(), ALIGN_4 ) ModelObject( this->browser, this->sceneAsset, filepath, this->device );
}
