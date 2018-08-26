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
	if (count_chatter_message >= max_chatter_messages)
	{
		std::cout << "chatter message overflow" << endl;
		count_chatter_message = 0;
	}
}

void Message_Array::Add_Message(Message_Input input_event)
{
	Input_Message_Array[count_input_messages] = input_event;

	count_input_messages++;
	if (count_input_messages >= max_input_messages)
	{
		std::cout << "input message overflow" << endl;
		count_input_messages = 0;
	}

}

void Message_Array::Add_Message(Message_SG_Tile_Update tile_update)
{
	SG_Tile_Update_MSG_Array[count_SG_tile_map_update_messages] = tile_update;

	count_SG_tile_map_update_messages++;
	if (count_SG_tile_map_update_messages >= max_SG_tile_map_update_messages)
	{
		std::cout << "tile update message overflow" << endl;
		count_SG_tile_map_update_messages = 0;
	}
}

void Message_Array::Clear_All()
{
	count_chatter_message = 0;
	count_input_messages = 0;
	count_SG_tile_map_update_messages = 0;
}

void Message_Array :: free()
{
}