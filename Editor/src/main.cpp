#include "Editor.h"

int CALLBACK WinMain(HINSTANCE , HINSTANCE ,  LPSTR , int)                  
{                                                   

	Editor *pEditor = new Editor("Editor", 800,600);                                 
	pEditor->run();                                  

	return 0;                                       
}

