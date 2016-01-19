#pragma once

#include "Component.h"
#include "ButtonListener.h"

class ButtonListenerNode;

class Button : public Component
{
public:
	Button();
	Button( const Rectangle& rect );
	Button( const char* buttonText );
	Button( const Rectangle& rect, const char* buttonText );
	virtual ~Button();
	
	void Add_Action_Listener( ButtonListener* listener );
	void Remove_Action_Listener( ButtonListener* listener );

private:
	void Process_Click();

	static void Button_Click_Callback( Fl_Widget*, void* button );


private:
	ButtonListenerNode* listenerHead;
};