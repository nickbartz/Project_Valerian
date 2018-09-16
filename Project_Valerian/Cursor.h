#pragma once
#include<SDL.h>
#include<Message.h>
#include<Global_Constants.h>
#include<Message_Array.h>

class Console_Component;
class Global_Service_Locator;

class Cursor
{
public:
	Cursor(Global_Service_Locator* sPointer);
	
	SDL_Rect Get_Camera();
	void Draw();

	bool left_button = false;
	bool right_button = false;
	bool left_button_previous = false;
	bool right_button_previous = false;
	int current_mouse_x = 0;
	int current_mouse_y = 0;
	int held_mouse_x = 0;
	int held_mouse_y = 0;
	int last_mouse_x = 0;
	int last_mouse_y = 0;

	void Set_Currently_Clicked_Component(Console_Component* component);

	void Collect_Bus_Messages();
	void Update();

private:
	void Parse_Input_Message(SDL_Event e);
	void Age_Mouse();

	// Camera is instantiated as a simple rect where the "w" variables is equal to the zoom;
	SDL_Rect camera = { 0,0,0,0 };

	Global_Service_Locator * service_pointer;
	Console_Component* currently_clicked_component = NULL;
	SDL_Rect drag_rect = { 0,0,0,0 };
};