#include "InputHandler.h"

InputHandler::InputHandler()
{
	//initialise arrays for keypress bools
	for(int i = 0; i < 256; ++i)
	{
		keys[i] = false;
		prevState[i] = false;
	}
}

InputHandler::~InputHandler()
{

}

void InputHandler::KeyDown(unsigned int key)
{
	prevState[key] = keys[key];
	keys[key] = true;
}

void InputHandler::KeyUp(unsigned int key)
{
	prevState[key] = keys[key];
	keys[key] = false;
}

bool InputHandler::IsKeyDown(unsigned int key) const
{
	return keys[key];
}

bool InputHandler::IsKeyPressed(unsigned int key)
{
	if(prevState[key] != keys[key])
	{
		prevState[key] = keys[key];
		return true;
	}

	prevState[key] = keys[key];
	return false;
}

bool InputHandler::IsKeyUp(unsigned int key) const
{
	return !keys[key];
}
