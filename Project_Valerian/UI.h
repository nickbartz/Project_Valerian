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

	void Init();

	// Load and Clear Action Messages
	void Load_Action_To_UI(int message_size, int mArray[]);
	void Clear_Action_Support();
	void Clear_Action();

private:
	// Old Action Functionality // TO BE DEPRECATED
	int last_clicked_window = WINDOW_NULL;
	int last_clicked_panel = PANEL_NULL;
	int last_clicked_button_action = BUTTON_ACTION_DO_NOTHING;

	// New Action Functionality
	int action_support[MAX_LENGTH_CUSTOM_MESSAGE];
	bool action_support_loaded = false;
	int length_action_support = 0;

	int current_action[MAX_LENGTH_CUSTOM_MESSAGE];
	bool current_action_loaded = false;
	int length_current_action = 0;

	// Input message functions
	bool Mouse_Event();
	void Push_Message_To_Message_Bus();

	// Internal Click Management Functions
	
	void Handle_Click_In_World();
	void Update_UI_With_Mouse_Action(Cursor* cursor);
	void Parse_Loaded_Actions();

	// Window Open and Close Functions
	int num_entity_diagnostics_open = 0;
	void Open_Entity_Diagnostic(int x_pos, int y_pos);
	
	Global_Service_Locator* service_pointer;

	UI_Window_Player_Diagnostic player_console;
	UI_Window_Structure_Create create_window;
	UI_Window_Screen_Buttons static_buttons;
	UI_Window_Entity_Diagnostic entity_diagnostic_array[2];
};




