#include<Message_Array.h>
#include<iostream>

Message_Array::Message_Array(Service_Locator* sLocator)
{
	service_locator = sLocator;
}

void Message_Array::Add_Message(Message_Core message)
{
	Chatter_Array[count_chatter_message] = message;

	count_chatter_message++;
	if (count_chatter_message >= MAX_CHATTER_MESSAGES)
	{
		std::cout << "chatter message overflow" << endl;
		count_chatter_message = 0;
	}
}

void Message_Array::Add_Message(Message_Input input_event)
{
	Input_Message_Array[count_input_messages] = input_event;

	count_input_messages++;
	if (count_input_messages >= MAX_INPUT_MESSAGES)
	{
		std::cout << "input message overflow" << endl;
		count_input_messages = 0;
	}

}

void Message_Array::Add_SG_Tile_Update_Message(int grid_point_x, int grid_point_y, int tile_layer, int structure_id, int structure_type)
{
	SG_Tile_Update_MSG_Array.push_back(Message_SG_Tile_Update(grid_point_x, grid_point_y, tile_layer, structure_id, structure_type));
}

void Message_Array::Clear_All()
{
	count_chatter_message = 0;
	count_input_messages = 0;
	SG_Tile_Update_MSG_Array.clear();
}

void Message_Array :: free()
{
}