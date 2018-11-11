#include<UI.h>
#include<Service_Locator.h>
#include<algorithm>
#include<Cursor.h>
#include<SDL.h>
#include<Game_Library.h>
#include<Scene_Graph.h>
#include<AI_Stats_Component.h>


UI::UI(Global_Service_Locator* srvc_pointer)
{
	service_pointer = srvc_pointer;
	player_console = UI_Window_Player_Diagnostic(srvc_pointer);
	create_window = UI_Window_Structure_Create(srvc_pointer);
	static_buttons = UI_Window_Screen_Buttons(srvc_pointer);
	entity_diagnostic_array[0] = UI_Window_Object_Diagnostic(srvc_pointer);
	entity_diagnostic_array[1] = UI_Window_Object_Diagnostic(srvc_pointer);
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
}

void UI::Push_Message_To_Console(string message)
{
	player_console.Push_Message_Into_Stream(message);
}

void UI::Push_Message_To_Message_Bus()
{

}

void UI::Load_Action_To_UI(int message_size, int mArray[])
{
	if (mArray[0] == UI_ACTION_CATEGORY_SUPPORTING)
	{
		for (int i = 0; i < message_size; i++)
		{
			action_support[i] = mArray[i];
		}

		length_action_support = message_size;
		action_support_loaded = true;
	}
	else if (mArray[0] == UI_ACTION_CATEGORY_MAIN)
	{
		for (int i = 0; i < message_size; i++)
		{
			current_action[i] = mArray[i];
		}

		length_current_action = message_size;
		current_action_loaded = true;
	}
}

void UI::Clear_Action_Support()
{
	for (int i = 0; i < MAX_LENGTH_CUSTOM_MESSAGE; i++)
	{
		action_support[i] = 0;
	}

	length_action_support = 0;
	action_support_loaded = false;
}

void UI::Clear_Action()
{
	for (int i = 0; i < MAX_LENGTH_CUSTOM_MESSAGE; i++)
	{
		current_action[i] = 0;
	}

	length_current_action = 0;
	current_action_loaded = false;
}

// Internal functions having to do with the update of the UI

void UI::Init()
{
	player_console.Init();
	create_window.Init();
	static_buttons.Init();
}

bool UI::Mouse_Event()
{
	Cursor* cursor = service_pointer->get_Cursor_Pointer();

	if (cursor == NULL) {
		cout << "Null pointer in UI::Mouse_Event()" << endl;
		return false;
	}
	else
	{
		int mouse_event = cursor->Get_Recent_Mouse_Action();
		if (mouse_event > 0)
		{
			return true;
		}
	}
}

void UI::Update()
{		
	// Check Cursor For Clicks
	if (Mouse_Event() == true)
	{
		Update_UI_With_Mouse_Action(service_pointer->get_Cursor_Pointer());
	}

	if (player_console.is_open()) player_console.Draw(service_pointer->get_Draw_System_Pointer());
	if (create_window.is_open()) create_window.Draw(service_pointer->get_Draw_System_Pointer());
	static_buttons.Draw();
	if (entity_diagnostic_array[0].is_open()) entity_diagnostic_array[0].Draw(service_pointer->get_Draw_System_Pointer());
	if (entity_diagnostic_array[1].is_open()) entity_diagnostic_array[1].Draw(service_pointer->get_Draw_System_Pointer());
}

void UI::Parse_Loaded_Actions()
{
	int icon_clip_x = 0;
	int icon_clip_y = 0;
	Coordinate click_coord = service_pointer->get_Cursor_Pointer()->Get_Mouse_Grid_Coord();
	
	// See if there's a main action loaded 
	if (current_action_loaded)
	{
		// See if there is a supporting action to go along with the main action
		if (action_support_loaded)
		{
			switch (current_action[1])
			{
			case UI_ACTION_MAIN_TYPE_CLICK_IN_WORLD:
				switch (action_support[1])
				{
				case UI_ACTION_SUPPORTING_TYPE_SET_STRUCTURE_BUILD:
					//service_pointer->get_Scene_Graph()->Create_New_Structure(service_pointer->get_Cursor_Pointer()->Get_Mouse_Grid_Coord(), service_pointer->get_Game_Library()->Fetch_Tile_Object_Config(action_support[2]),2);
					service_pointer->get_Scene_Graph()->Create_New_Scaffold(service_pointer->get_Cursor_Pointer()->Get_Mouse_Grid_Coord(), action_support[2], 2);
					break;
				case UI_ACTION_SUPPORTING_TYPE_DELETE_STRUCTURE:
					service_pointer->get_Scene_Graph()->Delete_Structure_Highest_Layer(service_pointer->get_Cursor_Pointer()->Get_Mouse_Grid_Coord());
					break;
				case UI_ACTION_SUPPORTING_TYPE_SET_RALLY_POINT:
					{int custom_message[6] = { MESSAGE_TYPE_SET_ENTITY_RALLY_POINT, OBJECT_TYPE_ENTITY, FOCUS_RANGE, 0,service_pointer->get_Cursor_Pointer()->Get_Mouse_Grid_Coord().x,service_pointer->get_Cursor_Pointer()->Get_Mouse_Grid_Coord().y };
					service_pointer->get_MB_Pointer()->Add_Custom_Message(6, custom_message); }
					break;
				case UI_ACTION_SUPPORTING_TYPE_SET_ASTEROID_MINE:
					{Object* asteroid = service_pointer->get_Scene_Graph()->Return_Structure_At_Coord_By_Layer(click_coord.x, click_coord.y, TILE_LAYER_MID);
					if (asteroid != NULL) service_pointer->get_Scene_Graph()->Job_Create_Mine_Asteroid(asteroid);}
					break;
				}
				break;
			}
		}
		// If not just process the main action
		else
		{

		}
		Clear_Action();
	}
	// If there's no main action loaded, process the loaded supporting action
	else if (action_support_loaded)
	{
		switch (action_support[1])
		{
		case UI_ACTION_SUPPORTING_TYPE_SET_STRUCTURE_BUILD:
			icon_clip_x = service_pointer->get_Game_Library()->Fetch_Tile_Object_Config(action_support[2])->icon_clip_x;
			icon_clip_y = service_pointer->get_Game_Library()->Fetch_Tile_Object_Config(action_support[2])->icon_clip_y;
			service_pointer->get_Cursor_Pointer()->Change_Cursor_Icon(icon_clip_x, icon_clip_y);
			break;
		case UI_ACTION_SUPPORTING_TYPE_DELETE_STRUCTURE:
			icon_clip_x = 0;
			icon_clip_y = SPRITE_SIZE;
			service_pointer->get_Cursor_Pointer()->Change_Cursor_Icon(icon_clip_x, icon_clip_y);
			break;
		case UI_ACTION_SUPPORTING_TYPE_SET_RALLY_POINT:
			icon_clip_x = 2*SPRITE_SIZE;
			icon_clip_y = 0;
			service_pointer->get_Cursor_Pointer()->Change_Cursor_Icon(icon_clip_x, icon_clip_y);
			break;
		case UI_ACTION_SUPPORTING_TYPE_SET_ASTEROID_MINE:
			break;
		}
	}
}

// UI Mouse_Click Management

void UI::Update_UI_With_Mouse_Action(Cursor* cursor_pointer)
{
	SDL_Point new_point = cursor_pointer->Get_Mouse_Position();

	// Check to see if the mouse action affected any of the open windows
	if (player_console.is_open() && (player_console.is_currently_clicked() || SDL_PointInRect(&new_point, &player_console.Return_Rect()))) player_console.Respond_To_Mouse(cursor_pointer);
	else if (create_window.is_open() && (create_window.is_currently_clicked() || SDL_PointInRect(&new_point, &create_window.Return_Rect()))) create_window.Respond_To_Mouse(cursor_pointer);
	else if (SDL_PointInRect(&new_point, &static_buttons.Return_Rect())) static_buttons.Respond_To_Mouse();
	else if (entity_diagnostic_array[0].is_open() && (entity_diagnostic_array[0].is_currently_clicked() || SDL_PointInRect(&new_point, &entity_diagnostic_array[0].Return_Rect()))) entity_diagnostic_array[0].Respond_To_Mouse(cursor_pointer);
	else if (entity_diagnostic_array[1].is_open() && (entity_diagnostic_array[1].is_currently_clicked() || SDL_PointInRect(&new_point, &entity_diagnostic_array[1].Return_Rect()))) entity_diagnostic_array[1].Respond_To_Mouse(cursor_pointer);

	// If the mouse click didn't, then check to see if the mouse interacted with the world 
	else Handle_Click_In_World(); 

	// Basic idea is we check the console windows and the world to see if there the mouseclick generated an action 
	// If it did, we load the action from that button into either the supporting action array or the main action array
	// Then we parse the actions currently loaded into either array

	Parse_Loaded_Actions();
}

void UI::Handle_Click_In_World()
{
	SDL_Point mouse_location = service_pointer->get_Cursor_Pointer()->Get_Mouse_Position();

	switch (service_pointer->get_Cursor_Pointer()->Get_Recent_Mouse_Action())
	{
	case INPUT_LEFT_MOUSE_CLICK:
	{
		int message[4] = { UI_ACTION_CATEGORY_MAIN,UI_ACTION_MAIN_TYPE_CLICK_IN_WORLD,mouse_location.x, mouse_location.y };
		Load_Action_To_UI(4, message);
	}
		break;
	case INPUT_RIGHT_MOUSE_CLICK:
		Open_Entity_Diagnostic(mouse_location.x, mouse_location.y);
		break;
	}
}

// Window Open and Close Management 

void UI::Open_Entity_Diagnostic(int x_pos, int y_pos)
{
	Coordinate world_coord = service_pointer->get_Cursor_Pointer()->Get_Mouse_Grid_Coord();
	Object* diagnostic_object = service_pointer->get_Scene_Graph()->Return_Object_At_Coord(world_coord.x, world_coord.y);

	if (diagnostic_object != NULL)
	{
		switch (num_entity_diagnostics_open)
		{
		case 0:
			entity_diagnostic_array[0].Init(x_pos, y_pos, diagnostic_object);
			num_entity_diagnostics_open++;
			break;
		case 1:
			entity_diagnostic_array[1].Init(x_pos, y_pos, diagnostic_object);
			num_entity_diagnostics_open++;
			break;
		case 2:
			entity_diagnostic_array[0] = entity_diagnostic_array[1];
			entity_diagnostic_array[1] = UI_Window_Object_Diagnostic(service_pointer);
			entity_diagnostic_array[1].Init(x_pos, y_pos, diagnostic_object);
			break;
		}
	}
}