#pragma once

using namespace std;
#include<iostream>
#include<Global_Constants.h>
#include<Message.h>

class Message_Bus
{
public:
	Message_Bus();

	// INPUT MESSAGES
	void Add_Input_Message(SDL_Event input_event);
	void Clear_Input_Messages();



	// Free 
	void free();



private:
	int count_input_message;
	Input_Message Input_Message_Bus[MAX_INPUT_MESSAGES];
};