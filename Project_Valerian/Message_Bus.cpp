#include<Message_Bus.h>
#include<UI.h>
#include<Cursor.h>
#include<Service_Locator.h>

Message_Bus::Message_Bus(Service_Locator* service_pointer)
{
	service_locator = service_pointer;
	
	count_input_message = 0;
}

// Functions for adding or subtracting individual messages
void Message_Bus::Add_Input_Message(SDL_Event input_event)
{
	Input_Message new_message = Input_Message(input_event);
	
	Input_Message_Bus[count_input_message] = new_message;

	count_input_message++;
	if (count_input_message >= MAX_INPUT_MESSAGES) count_input_message = 0;
}

void Message_Bus::Clear_Input_Messages()
{
	count_input_message = 0;
}

// Functions for pushing messages to subsystems

void Message_Bus::Push_Messages()
{
	for (int i = 0; i < count_input_message; i++)
	{
		service_locator->get_UI_pointer()->Push_Message_To_UI(Input_Message_Bus[i]);

		service_locator->get_Cursor_Pointer()->Push_Message_To_Cursor(Input_Message_Bus[i]);
	}
}

void Message_Bus::free()
{

}