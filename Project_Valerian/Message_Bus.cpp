#include<Message_Bus.h>

Message_Bus::Message_Bus()
{
	count_input_message = -1;
}

void Message_Bus::Add_Input_Message(SDL_Event input_event)
{
	count_input_message++;
	if (count_input_message >= MAX_INPUT_MESSAGES) count_input_message = 0;
	
	Input_Message new_message = Input_Message(input_event);
	
	Input_Message_Bus[count_input_message] = new_message;


}

void Message_Bus::Clear_Input_Messages()
{
	for (int i = 0; i < MAX_INPUT_MESSAGES; i++)
	{
		Input_Message_Bus[i] = SDL_Event{};
	}

	count_input_message = -1;
}