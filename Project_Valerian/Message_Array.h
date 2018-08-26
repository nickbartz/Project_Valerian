#pragma once

#include<Message.h>
#include<Global_Constants.h>

class Service_Locator;

class Message_Array {
public:
	Message_Array(Service_Locator* sLocator);

	void Add_Message(Message_Core message);
	void Add_Message(Message_Input message);
	void Add_Message(Message_SG_Tile_Update message);

	void Clear_All();

	int count_chatter_message = 0;
	int max_chatter_messages = MAX_CHATTER_MESSAGES;
	Message_Core Chatter_Array[MAX_CHATTER_MESSAGES];

	int count_input_messages = 0;
	int max_input_messages = MAX_INPUT_MESSAGES;
	Message_Input Input_Message_Array[MAX_INPUT_MESSAGES];

	int count_SG_tile_map_update_messages = 0;
	int max_SG_tile_map_update_messages = MAX_SG_TILE_UPDATE_MESSAGES;
	Message_SG_Tile_Update SG_Tile_Update_MSG_Array[MAX_SG_TILE_UPDATE_MESSAGES];

	void free();

private:
	Service_Locator * service_locator = NULL;

};