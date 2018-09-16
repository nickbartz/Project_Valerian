#pragma once

#include<Global_Constants.h>
#include<Message.h>
#include<Console_Component.h>
#include<Console_Panel.h>
#include<Console_Window.h>
#include<Message_Array.h>

class Global_Service_Locator;
class Message_Bus;

class UI
{
public:
	UI(Global_Service_Locator* service_locator);

	void Update();

	void Push_Message_To_Console(string message);

	void Collect_Bus_Messages();

private:
	int last_clicked_window = WINDOW_NULL;
	int last_clicked_panel = PANEL_NULL;
	int last_clicked_button_action = BUTTON_ACTION_DO_NOTHING;

	// Input message functions
	bool Mouse_Event();

	void Push_Message_To_Message_Bus();

	// Internal Click Management Functions
	void Update_UI_With_Mouse_Action(Cursor* cursor);
	
	Global_Service_Locator* service_pointer;

	Player_Diagnostic_Window player_console;
};




