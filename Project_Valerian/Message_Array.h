#pragma once

#include<Message.h>
#include<Global_Constants.h>

class Service_Locator;

class Message_Array {
public:
	Message_Array(Service_Locator* sLocator);

	void Add_Chatter_Message(Message message);
	void Clear_Chatter_Messages();

	void Add_Input_Message(Input_Message input_event);
	void Clear_Input_Messages();

	void Clear_All();

	int count_chatter_message = 0;
	int max_chatter_messages = MAX_CHATTER_MESSAGES;
	Message Chatter_Array[MAX_CHATTER_MESSAGES];

	int count_input_messages = 0;
	int max_input_messages = MAX_INPUT_MESSAGES;
	Input_Message Input_Message_Array[MAX_INPUT_MESSAGES];

	void free();

private:
	Service_Locator * service_locator = NULL;

};