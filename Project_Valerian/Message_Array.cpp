#include<Message_Array.h>
#include<iostream>

Message_Array::Message_Array(Global_Service_Locator* sLocator)
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

void Message_Array::Add_Custom_Message(int message_length, int message_array[])
{
	bool message_valid = false;
	switch (message_array[0])
	{
	case MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION:
		switch (message_array[1])
		{
		case OBJECT_TYPE_ANY:
			switch (message_array[2])
			{
			case FOCUS_ALL:
				Custom_Message_Array[count_custom_messages] = (Custom_Message(message_length, message_array));
				message_valid = true;
				break;
			}
			break;
		}
		break;
	case MESSAGE_TYPE_SG_ENTITY_CREATE:
		switch (message_array[1])
		{
		case OBJECT_TYPE_ANY:
			switch (message_array[2])
			{
			case FOCUS_ALL:
				Custom_Message_Array[count_custom_messages] = (Custom_Message(message_length, message_array));
				message_valid = true;
				break;
			}
			break;
		}
		break;
	case MESSAGE_TYPE_STAT_UPDATE_REQUEST:
		switch (message_array[1])
		{
		case OBJECT_TYPE_STRUCTURE:
			switch (message_array[2])
			{
			case FOCUS_SPECIFC_OBJECT:
				Custom_Message_Array[count_custom_messages] = (Custom_Message(message_length, message_array));
				message_valid = true;
				break;
			}
			break;
		}
		break;
	case MESSAGE_TYPE_SET_ENTITY_RALLY_POINT:
		switch (message_array[1])
		{
		case OBJECT_TYPE_ENTITY:
			switch (message_array[2])
			{
			case FOCUS_RANGE:
				Custom_Message_Array[count_custom_messages] = (Custom_Message(message_length, message_array));
				message_valid = true;
				break;
			}
			break;
		}
		break;
	case MESSAGE_TYPE_SG_ENTITY_MOVEMENT:
		switch (message_array[1])
		{
		case OBJECT_TYPE_ANY:
			switch (message_array[2])
			{
			case FOCUS_ALL:
				Custom_Message_Array[count_custom_messages] = (Custom_Message(message_length, message_array));
				message_valid = true;
				break;
			}
			break;
		}
		break;
	}

	if (message_valid == false)
	{
		cout << "Invalid Custom Message Submitted to Bus" << endl;
	}
	else
	{
		count_custom_messages++;
		if (count_custom_messages >= MAX_NUM_CUSTOM_MESSAGES)
		{
			cout << "Custom message overflow" << endl;
			count_custom_messages = 0;
		}
	}
}

void Message_Array::Clear_All()
{
	count_chatter_message = 0;
	count_input_messages = 0;
	count_custom_messages = 0;
}

void Message_Array :: free()
{
}