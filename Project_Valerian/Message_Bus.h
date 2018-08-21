#pragma once

using namespace std;
#include<iostream>
#include<Global_Constants.h>
#include<Message.h>

class UI;
class Service_Locator;

class Message_Bus
{
public:
	Message_Bus(Service_Locator* service_pointer);

	// Functions to add or substract individual messages
	void Add_Input_Message(SDL_Event input_event);
	void Clear_Input_Messages();
	
	// Push Functions
	void Push_Messages();

	// Free 
	void free();

private:
	Service_Locator* service_locator = NULL;

	int count_input_message;
	Input_Message Input_Message_Bus[MAX_INPUT_MESSAGES];
};