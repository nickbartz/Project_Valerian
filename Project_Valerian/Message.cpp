#include <Message.h>
#include <iostream>

Message::Message(string new_message)
{
	message = new_message;
}

string Message::Get_Message_String()
{
	return message;
}

SDL_Event Input_Message::Return_Event()
{
	return sdl_event;
}

string Input_Message::Return_Event_String()
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