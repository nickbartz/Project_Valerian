#include<Message_Array.h>
#include<iostream>

Message_Array::Message_Array(Service_Locator* sLocator)
{
	service_locator = sLocator;
}

void Message_Array::Add_Chatter_Message(Message message)
{
	Chatter_Array[count_chatter_message] = message;

	count_chatter_message++;
	if (count_chatter_message >= max_chatter_messages)
	{
		std::cout << "chatter message overflow" << endl;
		count_chatter_message = 0;
	}
}

void Message_Array::Clear_Chatter_Messages()
{
	count_chatter_message = 0;
}

void Message_Array::Add_Input_Message(Input_Message input_event)
{
	Input_Message_Array[count_input_messages] = input_event;

	count_input_messages++;
	if (count_input_messages >= max_input_messages)
	{
		std::cout << "input message overflow" << endl;
		count_input_messages = 0;
	}

}

void Message_Array::Clear_Input_Messages()
{
	count_input_messages = 0;
}

void Message_Array::Clear_All()
{
	Clear_Chatter_Messages();
	Clear_Input_Messages();
}

void Message_Array :: free()
{
}