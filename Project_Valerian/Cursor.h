#pragma once
#include<SDL.h>
#include<Message.h>
#include<Global_Constants.h>

class Console_Component;
class Service_Locator;

class Cursor
{
public:
	Cursor(Service_Locator* sPointer);
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

	void Push_Message_To_Cursor(Input_Message message);
	void Set_Currently_Clicked_Component(Console_Component* component);

	void Update();

private:

	int current_num_input_messages = 0;
	Input_Message input_message_local[UI_MAX_INPUT_MESSAGES];
	void Age_Mouse();
	void Parse_Input_Message(SDL_Event event);
	void Clear_Messages();

	Service_Locator * service_pointer;
	Console_Component* currently_clicked_component = NULL;
	SDL_Rect drag_rect = { 0,0,0,0 };
};