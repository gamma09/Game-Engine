#include "Game.h"

int CALLBACK WinMain(HINSTANCE , HINSTANCE ,  LPSTR , int)                  
{                                                   

	Game *pGame = new Game("Game", 800,600);                                 
	pGame->run();                                  

	return 0;                                       
}
