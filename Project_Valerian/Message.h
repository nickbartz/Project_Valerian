#pragma once

#include<SDL.h>

class Message
{
public:
	Message();
private:
};

class Input_Message: public Message
{
public:
	Input_Message(SDL_Event event = {}) :Message()
	{
		sdl_event = event;
	}

	SDL_Event Return_Event();

private:

	SDL_Event sdl_event;

};