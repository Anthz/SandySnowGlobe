#pragma once
class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);
	bool IsKeyDown(unsigned int key) const;
	bool IsKeyPressed(unsigned int key);
	bool IsKeyUp(unsigned int key) const;

private:
	bool keys[256], prevState[256];
};

