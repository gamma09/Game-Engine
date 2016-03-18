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
#include <DrawInfo.h>
#include <DebuggerSetup.h>
#include <WireframeMaterial.h>

#include "Editor.h"
#include "Scene.h"

#include "ModelObject.h"
#include "ActorObject.h"
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
	updater( new( AssetHeap::Instance(), ALIGN_4 ) AnimatingStrategy() ) //,
	//inModal( false )
{
	// Do nothing
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

	this->moveableCamera = CameraManager::Instance()->Add( perspective, orientation );


	// W - move forward
	KeyBindingManager::Instance()->Add( 0x57, new( TemporaryHeap::Instance(), ALIGN_4 ) MoveAction( this->moveableCamera, 0.0f, 0.0f, -1.0f ) );
	// S - move backward
	KeyBindingManager::Instance()->Add( 0x53, new( TemporaryHeap::Instance(), ALIGN_4 ) MoveAction( this->moveableCamera, 0.0f, 0.0f, 1.0f ) );
	// A - move left
	KeyBindingManager::Instance()->Add( 0x41, new( TemporaryHeap::Instance(), ALIGN_4 ) MoveAction( this->moveableCamera, -1.0f, 0.0f, 0.0f ) );
	// D - move right
	KeyBindingManager::Instance()->Add( 0x44, new( TemporaryHeap::Instance(), ALIGN_4 ) MoveAction( this->moveableCamera, 1.0f, 0.0f, 0.0f ) );

	this->scene = new( AssetHeap::Instance(), ALIGN_4 ) Scene( this->browser, "Test Scene" );
	ModelObject* model1 = this->scene->AddModel( this->device, "../resources/soldier_animated_jump.spu" );
	ModelObject* model2 = this->scene->AddModel( this->device, "../resources/teddy_tga.spu" );

	ActorObject* soldier2 = this->scene->AddActor( this->device, *model1, this->unlitTextureMaterial, this->updater );
	soldier2->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 1 );
	soldier2->GetActorAsset()->GetActor()->position[X] = -125.0f;
	soldier2->GetActorAsset()->GetActor()->Update_Model_Matrix();
	soldier2->GetActorAsset()->GetActor()->Get_Model().Start_Animation( 500, 0 );

	ActorObject* teddy1 = this->scene->AddActor( this->device, *model2, this->unlitTextureMaterial, this->updater );
	teddy1->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 1 );
	teddy1->GetActorAsset()->GetActor()->position[Y] = -175.0f;
	teddy1->GetActorAsset()->GetActor()->Update_Model_Matrix();
	teddy1->GetActorAsset()->GetActor()->Get_Model().Start_Animation( 0, 0 );

	ActorObject* teddy2 = this->scene->AddActor( this->device, *model2, this->unlitTextureMaterial, this->updater );
	teddy2->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 1 );
	teddy2->GetActorAsset()->GetActor()->position[X] = -125.0f;
	teddy2->GetActorAsset()->GetActor()->position[Y] = -175.0f;
	teddy2->GetActorAsset()->GetActor()->Update_Model_Matrix();
	teddy2->GetActorAsset()->GetActor()->Get_Model().Start_Animation( 0, 1 );

	this->light = DirectionLightManager::Instance()->Add( -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f );
	this->light->Add_Reference();
}

const static Time MILLISECOND = Time( TIME_ONE_MILLISECOND );

//-----------------------------------------------------------------------------
// Editor::Update()
//      Called once per frame, update data, tranformations, etc
//      Use this function to control process order
//      Input, AI, Physics, Animation, and Graphics
//-----------------------------------------------------------------------------
void Editor::Update()
{
	Time timeSinceUpdate = this->updateTimer.toc();
	this->updateTimer.tic();
	this->totalTime += timeSinceUpdate;
	uint32_t totalTimeMillis = Time::quotient( this->totalTime, MILLISECOND );
	out( "Time: %d\n", totalTimeMillis );

	KeyBindingManager::Instance()->Check_Input();

	this->moveableCamera->Update_Position( totalTimeMillis );

	this->scene->Update( totalTimeMillis );
}

//-----------------------------------------------------------------------------
// Editor::Draw()
//		This function is called once per frame
//	    Use this for draw graphics to the screen.
//      Only do rendering here
//-----------------------------------------------------------------------------
void Editor::Draw()
{
	DrawInfo info;
	info.camera = CameraManager::Instance()->Get_Active_Camera();
	info.light = this->light;
	info.context = this->deviceContext;

	this->scene->Draw( info );
}

//-----------------------------------------------------------------------------
// Editor::UnLoadContent()
//       unload content (resources loaded above)
//       unload all content that was loaded before the Engine Loop started
//-----------------------------------------------------------------------------
void Editor::UnLoadContent()
{
	delete this->scene;

	this->light->Remove_Reference();
}

void Editor::CloseEngine()
{
	this->browser->Exit();
}


void Editor::OnModelSelected( ContentObject* model )
{
	model;
	// TODO implement model selected
}


void Editor::OnModelDeselected()
{
	// TODO implement model deselected
}

void Editor::OnActorSelected( ContentObject* actor )
{
	actor;
	// TODO implement actor selected
}

void Editor::OnActorDeselected()
{
	// TODO implement actor deselected
}

void Editor::OnActorCreated( ContentObject* modelObj )
{
	ModelObject* model = static_cast<ModelObject*>( modelObj );
	ActorObject* actor = this->scene->AddActor( this->device, *model, this->unlitTextureMaterial, updater );

	actor->GetActorAsset()->GetActor()->Get_Model().Change_Active_Texture( 1 );
	actor->GetActorAsset()->GetActor()->Update_Model_Matrix();
	actor->GetActorAsset()->GetActor()->Get_Model().Start_Animation( Time::quotient( this->totalTime, MILLISECOND ), 0 );
}

void Editor::OnActorDeleted( ContentObject* actorObj )
{
	ActorObject* actor = static_cast<ActorObject*>( actorObj );
	this->scene->RemoveActor( actor );
}

void Editor::OnExit()
{
	PostMessage( this->hWindow, WM_ENGINE_DESTROY_WINDOW, 0, 0 );
}

void Editor::OnNewLevel()
{
	// TODO implement new level
}

void Editor::OnOpenLevel( const char* filepath )
{
	filepath;
	// TODO implement open level
}

void Editor::OnSaveLevel( const char* filepath )
{
	filepath;
	// TODO implement save level
}

void Editor::OnLoadModel( const char* filepath )
{
	filepath;
	// TODO implement load model
}
