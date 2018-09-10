#pragma once

#include<Message.h>
#include<Global_Constants.h>
#include<vector>

class Service_Locator;

class Message_Array {
public:
	Message_Array(Service_Locator* sLocator);

	void Add_Message(Message_Core message);
	void Add_Message(Message_Input message);
	void Add_SG_Tile_Update_Message(int grid_point_x, int grid_point_y, int tile_layer, int structure_id, int structure_type);

	void Clear_All();

	int count_chatter_message = 0;
	Message_Core Chatter_Array[MAX_CHATTER_MESSAGES];

	int count_input_messages = 0;
	Message_Input Input_Message_Array[MAX_INPUT_MESSAGES];

	vector<Message_SG_Tile_Update> SG_Tile_Update_MSG_Array;

	void free();

private:
	Service_Locator * service_locator = NULL;

};