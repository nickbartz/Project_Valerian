#include<UI.h>
#include<Service_Locator.h>
#include<Message_Bus.h>
#include<algorithm>
#include<Cursor.h>


UI::UI(Service_Locator* srvc_pointer)
{
	current_num_input_messages = 0;
	service_pointer = srvc_pointer;
}

// External API functions having to do with communicating with the UI

void UI::Push_Message_To_UI(Message message)
{

}

void UI::Push_Message_To_UI(Input_Message message)
{
	input_message_local[current_num_input_messages] = message;

	current_num_input_messages++;
	if (current_num_input_messages > MAX_INPUT_MESSAGES) current_num_input_messages = 0;

}

void UI::Push_Message_To_Message_Bus()
{

}

// Internal functions having to do with the update of the UI

void UI::Push_Input_Messages_To_Console()
{
	for (int i = 0; i < current_num_input_messages; i++)
	{
		string event_string = input_message_local[i].Return_Event_String();
		if (event_string.size() > 0)
		{
			string message = "INPUT: " + event_string;
			Push_Message_To_Console(message);
		}
	}
}

void UI::Push_Message_To_Console(string message)
{
	if (message.size() > 0) player_console.Push_Message_Into_Stream(message);
}

bool UI::Mouse_Event()
{
	Cursor* cursor = service_pointer->get_Cursor_Pointer();

	if (cursor->left_button == 1)
	{
		if( cursor->left_button_previous == 0) return true; // Left Click
		if (cursor->current_mouse_x != cursor->held_mouse_x || cursor->current_mouse_y != cursor->held_mouse_y) return true; // Left Click Drag
	}
	else if (cursor->right_button == 1)
	{
		if (cursor->right_button_previous == 0) return true; // Right Click
		if (cursor->current_mouse_x != cursor->held_mouse_x || cursor->current_mouse_y != cursor->held_mouse_y) return true; // Right Click Drag
	}
	else if (cursor->right_button_previous == 1 && cursor->right_button == 0) return true; // Left Unclick
	else if (cursor->left_button_previous == 1 && cursor->left_button == 0) return true; // Right Unclick
}

void UI::Update()
{		
	// Send relevant input messages to player console window
	Push_Input_Messages_To_Console();

	// Check Cursor For Clicks
	if (Mouse_Event() == true) Update_UI_With_Mouse_Action(service_pointer->get_Cursor_Pointer());

	// not implemented yet
	Push_Message_To_Message_Bus();

	player_console.Draw(service_pointer->get_Draw_System_Pointer());

	Clear_Messages();
}

void UI::Clear_Messages()
{
	// Set the index of input messages back to -1 so it starts empty essentially
	current_num_input_messages = 0;
}

// UI Mouse_Click Management

void UI::Update_UI_With_Mouse_Action(Cursor* cursor_pointer)
{
	player_console.Respond_To_Mouse(cursor_pointer);
}