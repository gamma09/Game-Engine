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



#include "StationaryStrategy.h"
#include "AnimatingStrategy.h"

#include "MoveAction.h"



//-----------------------------------------------------------------------------
//  Editor::Editor()
//		Editor Engine Constructor
//-----------------------------------------------------------------------------
Editor::Editor( const char* windowName, const int Width, const int Height ) :
Engine( windowName, Width, Height ),
wasCulled( false )
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

	UpdateStrategy* updater = new( TemporaryHeap::Instance(), ALIGN_4 ) AnimatingStrategy();

	ModelBase* model = ModelBaseManager::Instance()->Add( this->device, "../resources/soldier_animated_jump.spu" );
	this->actor[0] = ActorManager::Instance()->Add( this->unlitTextureMaterial, model, updater );
	this->actor[0]->Add_Reference();
	this->actor[0]->Get_Model().Change_Active_Texture( 1 );
	this->actor[0]->Update_Model_Matrix();
	this->actor[0]->Get_Model().Start_Animation( 0, 0 );

	this->actor[1] = ActorManager::Instance()->Add( this->unlitTextureMaterial, model, updater );
	this->actor[1]->Add_Reference();
	this->actor[1]->position[X] = -125.0f;
	this->actor[1]->Get_Model().Change_Active_Texture( 1 );
	this->actor[1]->Update_Model_Matrix();
	this->actor[1]->Get_Model().Start_Animation( 500, 0 );

	ModelBase* model2 = ModelBaseManager::Instance()->Add( this->device, "../resources/teddy_tga.spu" );
	this->actor[2] = ActorManager::Instance()->Add( this->unlitTextureMaterial, model2, updater );
	this->actor[2]->Add_Reference();
	this->actor[2]->position[Y] = -175.0f;
	this->actor[2]->Get_Model().Change_Active_Texture( 1 );
	this->actor[2]->Update_Model_Matrix();
	this->actor[2]->Get_Model().Start_Animation( 0, 0 );

	this->actor[3] = ActorManager::Instance()->Add( this->unlitTextureMaterial, model2, updater );
	this->actor[3]->Add_Reference();
	this->actor[3]->position[Y] = -175.0f;
	this->actor[3]->position[X] = -125.0f;
	this->actor[3]->Get_Model().Change_Active_Texture( 1 );
	this->actor[3]->Update_Model_Matrix();
	this->actor[3]->Get_Model().Start_Animation( 0, 1 );

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

	this->actor[0]->Update( totalTimeMillis );
	this->actor[1]->Update( totalTimeMillis );
	this->actor[2]->Update( totalTimeMillis );
	this->actor[3]->Update( totalTimeMillis );
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

	this->actor[0]->Draw( info );
	this->actor[1]->Draw( info );
	this->actor[2]->Draw( info );
	this->actor[3]->Draw( info );
}

//-----------------------------------------------------------------------------
// Editor::UnLoadContent()
//       unload content (resources loaded above)
//       unload all content that was loaded before the Engine Loop started
//-----------------------------------------------------------------------------
void Editor::UnLoadContent()
{
	this->actor[0]->Remove_Reference();
	this->actor[1]->Remove_Reference();
	this->actor[2]->Remove_Reference();
	this->actor[3]->Remove_Reference();

	this->light->Remove_Reference();
}
