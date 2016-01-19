#pragma once

class Button;

class ButtonListener
{
protected:
	ButtonListener();
	ButtonListener( const ButtonListener& listener );
	ButtonListener& operator=( const ButtonListener& listener );
	virtual ~ButtonListener();

public:
	virtual void Button_Clicked( const Button* buttonPressed );
};