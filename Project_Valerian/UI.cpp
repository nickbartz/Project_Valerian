#include<UI.h>
#include<Service_Locator.h>
#include<algorithm>
#include<Cursor.h>


UI::UI(Service_Locator* srvc_pointer)
{
	service_pointer = srvc_pointer;
}

// External API functions having to do with communicating with the UI

void UI::Collect_Bus_Messages()
{
	Message_Array* bus_pointer = service_pointer->get_MB_Pointer();

	if (bus_pointer == NULL)
	{
		cout << "ERROR: UI::null pointer in Push_Bus_Messages_To_Console()" << endl;
		return;
	}

	for (int i = 0; i < bus_pointer->count_input_messages; i++) Push_Message_To_Console(bus_pointer->Input_Message_Array[i].Get_Message_String());
	for (int i = 0; i < bus_pointer->count_chatter_message; i++) Push_Message_To_Console(bus_pointer->Chatter_Array[i].Get_Message_String());
	for (int i = 0; i < bus_pointer->SG_Tile_Update_MSG_Array.size(); i++) Push_Message_To_Console(bus_pointer->SG_Tile_Update_MSG_Array[i].Get_Message_String());
}

void UI::Push_Message_To_Console(string message)
{
	player_console.Push_Message_Into_Stream(message);
}

void UI::Push_Message_To_Message_Bus()
{

}

// Internal functions having to do with the update of the UI

bool UI::Mouse_Event()
{
	Cursor* cursor = service_pointer->get_Cursor_Pointer();

	if (cursor == NULL) {
		cout << "Null pointer in UI::Mouse_Event()" << endl;
		return false;
	}

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
	// Check Cursor For Clicks
	if (Mouse_Event() == true)
	{
		Update_UI_With_Mouse_Action(service_pointer->get_Cursor_Pointer());
	}

	player_console.Draw(service_pointer->get_Draw_System_Pointer());
}

// UI Mouse_Click Management

void UI::Update_UI_With_Mouse_Action(Cursor* cursor_pointer)
{
	player_console.Respond_To_Mouse(cursor_pointer);
}