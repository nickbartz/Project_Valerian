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
	UI_Component_Generic*  Return_Currently_Clicked_Component();
	UI_Component_User_Input_Box* Return_Current_Focus_Component();
	void Set_Currently_Clicked_Component(UI_Component_Generic* component);
	void Set_Current_Focus_Component(UI_Component_User_Input_Box* component);
	void Set_Currently_Clicked_To_Null();
	void Set_Current_Focus_To_Null();

	void Load_Action_To_UI(int message_size, int mArray[]);
	void Clear_Action_Support();
	void Clear_Action();

private:
	UI_Component_Generic * currently_clicked_component;
	UI_Component_User_Input_Box * current_focus_component;

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
	UI_Window_Object_Diagnostic entity_diagnostic_array[2];
};




