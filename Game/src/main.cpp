#include "Game.h"

int CALLBACK WinMain(HINSTANCE , HINSTANCE ,  LPSTR , int)                  
{                                                   

	Game *pGame = new Game("Monkey", 800,600);                                 
	pGame->run();                                  

	return 0;                                       
}

