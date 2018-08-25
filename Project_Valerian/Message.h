#pragma once

using namespace std;
#include<SDL.h>
#include<string>

class Message
{
public:
	Message(string message = "");
	string Get_Message_String();

private:
	string message;
};

class Input_Message: public Message
{
public:
	Input_Message(SDL_Event event = {}) :Message("")
	{
		sdl_event = event;
	}

	SDL_Event Return_Event();
	string Return_Event_String();

private:

	SDL_Event sdl_event;

};

