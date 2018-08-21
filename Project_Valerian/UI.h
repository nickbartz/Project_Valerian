#pragma once

#include<Global_Constants.h>
#include<Message.h>
#include<Console_Component.h>
#include<Console_Panel.h>
#include<Console_Window.h>

class Service_Locator;
class Message_Bus;

class UI
{
public:
	UI(Service_Locator* service_locator);
	void Push_Message_To_UI(Message message);
	void Push_Message_To_UI(Input_Message message);

	void Update();

	struct UI_Message {
		int ui_message_init = 0;
	};

private:
	int last_clicked_window = WINDOW_NULL;
	int last_clicked_panel = PANEL_NULL;
	int last_clicked_button_action = BUTTON_ACTION_DO_NOTHING;

	int current_num_input_messages;
	Input_Message input_message_local[UI_MAX_INPUT_MESSAGES];
	
	// Input message functions
	void Push_Input_Messages_To_Console();
	bool Mouse_Event();

	void Push_Message_To_Console(string message);
	void Push_Message_To_Message_Bus();
	void Clear_Messages();

	// Internal Click Management Functions
	void Update_UI_With_Mouse_Action(Cursor* cursor);
	
	Service_Locator* service_pointer;

	Player_Diagnostic_Window player_console;
};




