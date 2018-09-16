#pragma once

#include<Message.h>
#include<Global_Constants.h>
#include<vector>

class Global_Service_Locator;

class Message_Array {
public:
	Message_Array(Global_Service_Locator* sLocator);

	void Add_Message(Message_Core message);
	void Add_Message(Message_Input message);
	void Add_Custom_Message(int message_lenth, int custom_message[]);

	void Clear_All();

	int count_chatter_message = 0;
	Message_Core Chatter_Array[MAX_CHATTER_MESSAGES];

	int count_input_messages = 0;
	Message_Input Input_Message_Array[MAX_INPUT_MESSAGES];
	
	int count_custom_messages = 0;
	Custom_Message Custom_Message_Array[MAX_NUM_CUSTOM_MESSAGES];

	void free();

private:
	Global_Service_Locator * service_locator = NULL;

};