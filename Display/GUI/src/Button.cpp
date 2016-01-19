#pragma warning( push )
#pragma warning( disable : 4244 )
#include <FL/Fl_Button.H>
#pragma warning( pop )

#include <GameAssert.h>

#include "Button.h"

namespace GUI
{


	class ButtonListenerNode
	{
	public:
		ButtonListenerNode( ButtonListener* inListener, ButtonListenerNode* inNext ) :
			listener( inListener ),
			next( inNext )
		{
			// Do nothing
		}

		~ButtonListenerNode() { }

		ButtonListenerNode( const ButtonListenerNode& ) = delete;
		ButtonListenerNode& operator=( const ButtonListenerNode& ) = delete;

		ButtonListener* const listener;
		ButtonListenerNode* next;
	};





	Button::Button() :
		Component( new Fl_Button( 0, 0, 0, 0 ) ),
		listenerHead( nullptr )
	{
		this->Get_FLTK_Component()->callback( Button::Button_Click_Callback, this );
	}

	Button::Button( const Rectangle& rect ) :
		Component( new Fl_Button( rect.x, rect.y, rect.width, rect.height ) ),
		listenerHead( nullptr )
	{
		this->Get_FLTK_Component()->callback( Button::Button_Click_Callback, this );
	}

	Button::Button( const char* buttonText ) :
		Component( new Fl_Button( 0, 0, 0, 0 ) ),
		listenerHead( nullptr )
	{
		GameAssert( buttonText );

		this->Get_FLTK_Component()->callback( Button::Button_Click_Callback, this );
		this->Set_Text( buttonText );
	}

	Button::Button( const Rectangle& rect, const char* buttonText ) :
		Component( new Fl_Button( rect.x, rect.y, rect.width, rect.height ) ),
		listenerHead( nullptr )
	{
		GameAssert( buttonText );

		this->Get_FLTK_Component()->callback( Button::Button_Click_Callback, this );
		this->Set_Text( buttonText );
	}

	Button::~Button()
	{
		while( this->listenerHead )
		{
			ButtonListenerNode* curr = this->listenerHead;
			this->listenerHead = this->listenerHead->next;
			delete curr;
		}
	}

	void Button::Add_Action_Listener( ButtonListener* listener )
	{
		GameAssert( listener );
		this->listenerHead = new ButtonListenerNode( listener, this->listenerHead );
	}

	void Button::Remove_Action_Listener( ButtonListener* listener )
	{
		GameAssert( listener );

		ButtonListenerNode* prev = nullptr;
		ButtonListenerNode* curr = this->listenerHead;

		while( curr != nullptr && curr->listener != listener )
		{
			prev = curr;
			curr = curr->next;
		}

		GameAssert( curr );

		if( nullptr == prev )
		{
			this->listenerHead = curr->next;
		}
		else
		{
			prev->next = curr->next;
		}

		delete curr;
	}

	void Button::Process_Click()
	{
		ButtonListenerNode* curr = this->listenerHead;
		while( curr != nullptr )
		{
			curr->listener->Button_Clicked( this );
			curr = curr->next;
		}
	}

	void Button::Button_Click_Callback( Fl_Widget*, void* buttonPtr )
	{
		Button* button = static_cast<Button*>( buttonPtr );
		button->Process_Click();
	}

}
