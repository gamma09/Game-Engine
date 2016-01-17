#include <cmath>
#include <GameAssert.h>

#include <Timer.h>

#include "LibrarySetup.h"

#include "Game.h"

#include "BoneManager.h"
#include "TextureManager.h"
#include "ActorManager.h"
#include "ShaderManager.h"
#include "ModelBaseManager.h"
#include "CameraManager.h"
#include "DirectionLightManager.h"
#include "KeyBindingManager.h"

#include "WireframeMaterial.h"
#include "TextureMaterial.h"
#include "LightingTextureMaterial.h"

#include "StationaryStrategy.h"
#include "AnimatingStrategy.h"

#include "MoveAction.h"

#include "Camera.h"
#include "DirectionLight.h"
#include "DrawInfo.h"
#include "DrawStrategy.h"


#include "DebuggerSetup.h"
#include "MemorySetup.h"

//-----------------------------------------------------------------------------
//  Game::Game()
//		Game Engine Constructor
//-----------------------------------------------------------------------------
Game::Game( const char* windowName, const int Width,const int Height) :
	Engine( windowName, Width, Height),
	wasCulled(false)
{
	// Do nothing
}

//-----------------------------------------------------------------------------
// Game::Initialize()
//		Allows the engine to perform any initialization it needs to before 
//      starting to run.  This is where it can query for any required services 
//      and load any non-graphic related content. 
//-----------------------------------------------------------------------------
void Game::Initialize()
{
	GameVerify( Mem_OK == Mem::initialize() );
	
	TemporaryHeap::Create();

	GameVerify( Mem_OK == Mem::createHeap( this->managerHeap, 4096, "Manager Heap" ) );
	GameVerify( Mem_OK == Mem::createHeap( this->materialHeap, 4096, "Material Heap" ) );

	ShaderManager::Create(this->managerHeap, 4, 1);
	ModelBaseManager::Create(this->managerHeap, 7, 1);
	CameraManager::Create(this->managerHeap, 1, 1);
	TextureManager::Create(this->managerHeap, 10, 3);
	ActorManager::Create(this->managerHeap, 12, 1);
	DirectionLightManager::Create(this->managerHeap, 1, 1);
	KeyBindingManager::Create(this->managerHeap, 4, 2);
	BoneManager::Create(this->managerHeap, 100, 10);
}

//-----------------------------------------------------------------------------
// Game::LoadContent()
//		Allows you to load all content needed for your engine,
//	    such as objects, graphics, etc.
//-----------------------------------------------------------------------------
void Game::LoadContent()
{
	TextureManager::Instance()->Create_Default_Texture();
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	ViewportData viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = info.windowWidth;
	viewport.height = info.windowHeight;

	PerspectiveData perspective;
	perspective.fov_Y = 50.0f * MATH_PI_180;
	perspective.aspect_ratio = (float) info.windowWidth / (float) info.windowHeight;
	perspective.near_distance = 0.1f;
	perspective.far_distance = 1000.0f;

	OrientationData orientation;
	orientation.pos = Vect(0.0f, 0.0f, 400.0f);
	orientation.lookAt = Vect(0.0f, 0.0f, 0.0f);
	orientation.up = Vect(0.0f, 1.0f, 0.0f);

	this->moveableCamera = CameraManager::Instance()->Add(viewport, perspective, orientation);


	// W - move forward
	KeyBindingManager::Instance()->Add(0x57, new(TemporaryHeap::Instance(), ALIGN_4) MoveAction(this->moveableCamera, 0.0f, 0.0f, -1.0f));
	// S - move backward
	KeyBindingManager::Instance()->Add(0x53, new(TemporaryHeap::Instance(), ALIGN_4) MoveAction(this->moveableCamera, 0.0f, 0.0f, 1.0f));
	// A - move left
	KeyBindingManager::Instance()->Add(0x41, new(TemporaryHeap::Instance(), ALIGN_4) MoveAction(this->moveableCamera, -1.0f, 0.0f, 0.0f));
	// D - move right
	KeyBindingManager::Instance()->Add(0x44, new(TemporaryHeap::Instance(), ALIGN_4) MoveAction(this->moveableCamera, 1.0f, 0.0f, 0.0f));

	WireframeMaterial::Create_Material(this->materialHeap);
	TextureMaterial::Create_Material(this->materialHeap);
	LightingTextureMaterial::Create_Material(this->materialHeap);

	GlobalDrawState::Change_Draw_Types(DRAW_TEXTURE_ONLY, DRAW_WIREFRAME);

	UpdateStrategy* updater = new(TemporaryHeap::Instance(), ALIGN_4) AnimatingStrategy();

	ModelBase* model = ModelBaseManager::Instance()->Add("../resources/soldier_animated_jump.spu");
	this->actor[0] = ActorManager::Instance()->Add(model, updater);
	this->actor[0]->Add_Reference();
	this->actor[0]->Get_Model().Change_Active_Texture(1);
	this->actor[0]->Update_Model_Matrix();
	this->actor[0]->Get_Model().Start_Animation(0, 0);

	this->actor[1] = ActorManager::Instance()->Add(model, updater);
	this->actor[1]->Add_Reference();
	this->actor[1]->position[X] = -125.0f;
	this->actor[1]->Get_Model().Change_Active_Texture(1);
	this->actor[1]->Update_Model_Matrix();
	this->actor[1]->Get_Model().Start_Animation(500, 0);

	ModelBase* model2 = ModelBaseManager::Instance()->Add("../resources/teddy_tga.spu");
	this->actor[2] = ActorManager::Instance()->Add(model2, updater);
	this->actor[2]->Add_Reference();
	this->actor[2]->position[Y] = -175.0f;
	this->actor[2]->Get_Model().Change_Active_Texture(1);
	this->actor[2]->Update_Model_Matrix();
	this->actor[2]->Get_Model().Start_Animation(0, 0);

	this->actor[3] = ActorManager::Instance()->Add(model2, updater);
	this->actor[3]->Add_Reference();
	this->actor[3]->position[Y] = -175.0f;
	this->actor[3]->position[X] = -125.0f;
	this->actor[3]->Get_Model().Change_Active_Texture(1);
	this->actor[3]->Update_Model_Matrix();
	this->actor[3]->Get_Model().Start_Animation(0, 1);

	this->light = DirectionLightManager::Instance()->Add(-1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	this->light->Add_Reference();

	
}

const static Time MILLISECOND = Time(TIME_ONE_MILLISECOND);

//-----------------------------------------------------------------------------
// Game::Update()
//      Called once per frame, update data, tranformations, etc
//      Use this function to control process order
//      Input, AI, Physics, Animation, and Graphics
//-----------------------------------------------------------------------------
void Game::Update()
{
	Time timeSinceUpdate = this->updateTimer.toc();
	this->updateTimer.tic();
	this->totalTime += timeSinceUpdate;
	uint32_t totalTimeMillis = Time::quotient(this->totalTime, MILLISECOND);
	out("Time: %d\n", totalTimeMillis);

	KeyBindingManager::Instance()->Check_Input();

	this->moveableCamera->Update_Position(totalTimeMillis);

	this->actor[0]->Update(totalTimeMillis);
	this->actor[1]->Update(totalTimeMillis);
	this->actor[2]->Update(totalTimeMillis);
	this->actor[3]->Update(totalTimeMillis);
}

//-----------------------------------------------------------------------------
// Game::Draw()
//		This function is called once per frame
//	    Use this for draw graphics to the screen.
//      Only do rendering here
//-----------------------------------------------------------------------------
void Game::Draw()
{
	static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
	static const GLfloat red[] = {0.25f, 0.0f, 0.0f, 1.0f};

	DrawInfo info;
	info.camera = CameraManager::Instance()->Get_Active_Camera();
	info.light = this->light;

	this->actor[0]->Draw(info);
	this->actor[1]->Draw(info);
	this->actor[2]->Draw(info);
	this->actor[3]->Draw(info);
}

//-----------------------------------------------------------------------------
// Game::UnLoadContent()
//       unload content (resources loaded above)
//       unload all content that was loaded before the Engine Loop started
//-----------------------------------------------------------------------------
void Game::UnLoadContent()
{
	this->actor[0]->Remove_Reference();
	this->actor[1]->Remove_Reference();
	this->actor[2]->Remove_Reference();
	this->actor[3]->Remove_Reference();


	this->light->Remove_Reference();

	TextureMaterial::Destroy_Material();
	WireframeMaterial::Destroy_Material();
	LightingTextureMaterial::Destroy_Material();

	KeyBindingManager::Destroy();
	DirectionLightManager::Destroy();
	ActorManager::Destroy();
	TextureManager::Destroy();
	CameraManager::Destroy();
	ModelBaseManager::Destroy();
	BoneManager::Destroy();
	ShaderManager::Destroy();

	GameVerify( Mem_OK == Mem::destroyHeap( this->managerHeap ) );
	GameVerify( Mem_OK == Mem::destroyHeap( this->materialHeap ) );

	TemporaryHeap::Destroy();

	GameVerify( Mem_OK == Mem::destroy() );
}


//------------------------------------------------------------------
// Game::ClearBufferFunc()
// Allows user to change the way the clear buffer function works
//------------------------------------------------------------------
void Game::ClearBufferFunc()
{
	const GLfloat grey[] = { 0.4f, 0.4f, 0.8f, 1.0f };
	const GLfloat one = 1.0f;

	glClearBufferfv(GL_COLOR, 0, grey);
	glClearBufferfv(GL_DEPTH, 0, &one);
}
