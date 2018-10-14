#include <Message.h>
#include <iostream>

Message_Core::Message_Core(string new_message)
{
	string_message = new_message;
}

string Message_Core::Get_Message_String()
{
	return string_message;
}

void Message_Core::Update_Message(string new_message)
{
	string_message = new_message;
}

SDL_Event Message_Input::Return_Event()
{
	return sdl_event;
}

string Message_Input::Return_Event_String()
{
	string new_string;
	
	switch (sdl_event.type)
	{
	case SDL_KEYDOWN:
		new_string = "Key push: " + string(SDL_GetKeyName(sdl_event.key.keysym.sym));
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (sdl_event.button.button == SDL_BUTTON_LEFT) new_string = "Left Mouse Button Down";
		else if (sdl_event.button.button == SDL_BUTTON_RIGHT) new_string = "Right Mouse Button Down";
		else if (sdl_event.button.button == SDL_BUTTON_MIDDLE) new_string = "Middle Mouse Button Down";
		break;
	case SDL_MOUSEBUTTONUP:
		if (sdl_event.button.button == SDL_BUTTON_LEFT) new_string = "Left Mouse Button Up";
		else if (sdl_event.button.button == SDL_BUTTON_RIGHT) new_string = "Right Mouse Button Up";
		else if (sdl_event.button.button == SDL_BUTTON_MIDDLE) new_string = "Middle Mouse Button Up";
		break;
	case SDL_MOUSEWHEEL:
		if (sdl_event.wheel.y > 0 ) new_string = "Mouse Wheel Scroll Up";
		else if (sdl_event.wheel.y > 0) new_string = "Mouse Wheel Scroll Down";
		break;
	}
	
	return new_string;
}

int Message_SG_Tile_Update::Get_Grid_X()
{
	return grid_x;
}
int Message_SG_Tile_Update::Get_Grid_Y()
{
	return grid_y;
}
int Message_SG_Tile_Update::Get_Tile_Layer()
{
	return tile_layer;
}
int Message_SG_Tile_Update::Get_Structure_Name()
{
	return new_structure_name;
}
int Message_SG_Tile_Update::Get_Structure_Type()
{
	return new_structure_type;
}

int Custom_Message::Read_Message(int message_column)
{
	return message[message_column];
}

int Custom_Message::Get_Message_Length()
{
	return message_length;
}