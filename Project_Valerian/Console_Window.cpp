#include<Console_Window.h>
#include<Cursor.h>
#include<Service_Locator.h>
#include<Object.h>
#include<AI_Stats_Component.h>


UI_Window_Generic::UI_Window_Generic(Global_Service_Locator* sLocator, int window_name, string window_title, SDL_Rect base_rect)
{
	service_locator = sLocator;
	base_window_rect = base_rect;

	window_title_bar = UI_Component_Generic(service_locator,{ 0,0,base_rect.w - window_title_height,window_title_height }, true, { 50,50,155,255 });
	window_title_bar.Assign_Window(window_name);
	window_title_bar.Make_Button(window_title, { 5,3,0,0 }, BUTTON_ACTION_DRAG_WINDOW, { 255,255,255,255 });
	window_title_bar.Set_Font_Type(FONT_LARGE_BOLD);

	window_close_button = UI_Component_Generic(service_locator, { base_rect.w - window_title_height,0,window_title_height, window_title_height }, true, { 200,50,50,255 });
	window_close_button.Assign_Window(window_name);
	window_close_button.Make_Button("X", {7,3,0,0}, BUTTON_ACTION_CLOSE_WINDOW, {255,255,255,255});
	window_close_button.Set_Font_Type(FONT_LARGE_BOLD);
}

void UI_Window_Generic::Change_Window_Name(string new_name)
{
	window_title_bar.Change_Component_Title(new_name, { 5,3,0,0 });
}

SDL_Rect UI_Window_Generic::Return_Rect()
{
	return base_window_rect;
}

void UI_Window_Generic::Respond_To_Mouse(Cursor* cursor)
{
	// Check to see if the mouse click was intended to move windows or change panel
	window_title_bar.Check_For_Click();
	if (window_title_bar.Currently_Clicked() == true) Move_Window(cursor);

	window_close_button.Check_For_Click();
	if (window_close_button.Currently_Clicked() == true) currently_open = false;
	
	SDL_Point new_point = { cursor->current_mouse_x, cursor->current_mouse_y };
	
	if (SDL_PointInRect(&new_point, &base_window_rect) && cursor->Get_Recent_Mouse_Action() == INPUT_LEFT_MOUSE_CLICK)
	{
		for (int i = 0; i < active_panel_headers; i++)
		{
			panel_headers[i].Check_For_Click();
			if (panel_headers[i].Currently_Clicked() == true)
			{
				currently_active_panel = panel_headers[i].Get_Panel();
			}
		}
	}
}

void UI_Window_Generic::Change_Rect(SDL_Rect new_rect)
{
	base_window_rect = new_rect;
}

void UI_Window_Generic::Draw(Draw_System* draw_system)
{
	window_title_bar.Draw(draw_system, base_window_rect);
	window_close_button.Draw(draw_system, base_window_rect);
	
	for (int i = 0; i < active_panel_headers; i++)
	{
		panel_headers[i].Draw(draw_system, base_window_rect);
	}
}

void UI_Window_Generic::Create_Panel_Header(SDL_Rect pos_rect, int panel_name, string panel_title, SDL_Rect text_offset, SDL_Color text_color)
{
	panel_headers[active_panel_headers] = UI_Component_Generic(service_locator, pos_rect,true);
	panel_headers[active_panel_headers].Assign_Window(window_name);
	panel_headers[active_panel_headers].Assign_Panel(panel_name);
	panel_headers[active_panel_headers].Make_Button(panel_title, text_offset, BUTTON_ACTION_SWITCH_PANEL, text_color);
	panel_headers[active_panel_headers].Set_Font_Type(FONT_LARGE_BOLD);
	active_panel_headers++;
}

void UI_Window_Generic::Size_Panel_Headers()
{
	int current_x = 0;
	int total_button_width = 0;

	for (int i = 0; i < active_panel_headers; i++)
	{
		total_button_width += panel_headers[i].Get_Title().size();
	}

	int extra_width_for_buttons = (base_window_rect.w - total_button_width) / active_panel_headers;

	for (int i = 0; i < active_panel_headers; i++)
	{
		int button_width = panel_headers[i].Get_Title().size();
		panel_headers[i].Change_Offset_Rect(1,current_x);
		panel_headers[i].Change_Offset_Rect(3,button_width + extra_width_for_buttons);
		current_x += button_width + extra_width_for_buttons;
	}
}

void UI_Window_Generic::Move_Window(Cursor* cursor)
{
	base_window_rect.x += cursor->current_mouse_x - cursor->last_mouse_x;
	base_window_rect.y += cursor->current_mouse_y - cursor->last_mouse_y;
}

void UI_Window_Generic::Init()
{

}

bool UI_Window_Generic::is_open()
{
	return currently_open;
}

bool UI_Window_Generic::is_currently_clicked()
{
	if (window_title_bar.Currently_Clicked() == true) return true;
	else return false;
}

void UI_Window_Player_Diagnostic::Respond_To_Mouse(Cursor* cursor)
{	
	// Check to see if there's any need to move the window or change panels
	UI_Window_Generic::Respond_To_Mouse(cursor);

	// Check to see if the mouse click was on a component within the window
}

void UI_Window_Player_Diagnostic::Draw(Draw_System* draw_system)
{
	UI_Window_Generic::Draw(draw_system);

	if (currently_active_panel == panel_message_panel.Get_Panel_Name()) panel_message_panel.Draw(draw_system, base_window_rect);
}

void UI_Window_Player_Diagnostic::Push_Message_Into_Stream(string message)
{
	panel_message_panel.Push_Message_Into_Stream(message);
}

void UI_Window_Structure_Create::Respond_To_Mouse(Cursor* cursor)
{
	// Check to see if there's any need to move the window or change panels
	UI_Window_Generic::Respond_To_Mouse(cursor);

	// Check to see if the mouse click was on a component within the window
	if (currently_active_panel == structure_create_wall.Get_Panel_Name()) structure_create_wall.Check_For_Clicks();
	else if (currently_active_panel == structure_create_floor.Get_Panel_Name()) structure_create_floor.Check_For_Clicks();
	else if (currently_active_panel == structure_create_misc.Get_Panel_Name()) structure_create_misc.Check_For_Clicks();
}

void UI_Window_Structure_Create::Draw(Draw_System* draw_system)
{
	UI_Window_Generic::Draw(draw_system);

	if (currently_active_panel == structure_create_wall.Get_Panel_Name())
	{
		structure_create_wall.Draw(draw_system, base_window_rect);
	}
	else if (currently_active_panel == structure_create_floor.Get_Panel_Name())
	{
		structure_create_floor.Draw(draw_system, base_window_rect);
	}
	else if (currently_active_panel == structure_create_misc.Get_Panel_Name())
	{
		structure_create_misc.Draw(draw_system, base_window_rect);
	}

}

void UI_Window_Structure_Create::Init()
{
	structure_create_wall.Init(1);
	structure_create_floor.Init(2);
	structure_create_misc.Init(3);
}

void UI_Window_Object_Diagnostic::Respond_To_Mouse(Cursor* cursor)
{
	// Check to see if there's any need to move the window or change panels
	UI_Window_Generic::Respond_To_Mouse(cursor);

	// Check to see if the mouse click was on a component within the window

}

void UI_Window_Object_Diagnostic::Draw(Draw_System* draw_system)
{
	UI_Window_Generic::Draw(draw_system);

	switch (currently_active_panel)
	{
	case PANEL_OBJECT_INVENTORY:
		object_inventory.Draw(draw_system, base_window_rect);
		break;
	case PANEL_OBJECT_STATS:
		object_stats.Draw(draw_system, base_window_rect);
		break;
	case PANEL_OBJECT_JOBS:
		object_jobs.Draw(draw_system, base_window_rect);
		break;
	}
}

void UI_Window_Object_Diagnostic::Init(int base_x, int base_y, Object* diagnostic_object)
{
	Change_Rect({ base_x,base_y, base_window_rect.w, base_window_rect.h });
	currently_open = true;
	AI_Stats_Component* object_stats_pointer = diagnostic_object->Return_Stats_Component();

	switch (object_stats_pointer->Return_Object_Type())
	{
	case OBJECT_TYPE_ENTITY:
		Change_Window_Name(object_stats_pointer->Get_Entity_Name());
		break;
	case OBJECT_TYPE_CONTAINER:
		Change_Window_Name("Container");
		break;
	case OBJECT_TYPE_STRUCTURE:
		Change_Window_Name(object_stats_pointer->Get_Structure_Common_Name());
		break;
	case OBJECT_TYPE_SCAFFOLD:
		Change_Window_Name("Scaffold: " + object_stats_pointer->Get_Structure_Common_Name());
		break;
	}

	object_inventory.Init(diagnostic_object);
	object_stats.Init(diagnostic_object);
	object_jobs.Init(diagnostic_object);
}

void UI_Window_Screen_Buttons::Init()
{
	UI_Static_Button_Delete_Structure.Init(OBJECT_TYPE_NULL, 0, {7*SPRITE_SIZE,0,SPRITE_SIZE,SPRITE_SIZE});
	int message_length = 2;
	int message_array[2] = { UI_ACTION_CATEGORY_SUPPORTING,UI_ACTION_SUPPORTING_TYPE_DELETE_STRUCTURE };
	UI_Static_Button_Delete_Structure.Set_Click_Message(message_length, message_array);

	UI_Static_Button_Set_Rally_Point.Init(OBJECT_TYPE_NULL, 0, { 1 * SPRITE_SIZE,1*SPRITE_SIZE,SPRITE_SIZE,SPRITE_SIZE });
	int rally_length = 2;
	int rally_array[2] = { UI_ACTION_CATEGORY_SUPPORTING,UI_ACTION_SUPPORTING_TYPE_SET_RALLY_POINT };
	UI_Static_Button_Set_Rally_Point.Set_Click_Message(rally_length, rally_array);

	UI_Static_Button_Mark_Asteroid_For_Mining.Init(OBJECT_TYPE_NULL, 0, { 7 * SPRITE_SIZE,1 * SPRITE_SIZE,SPRITE_SIZE,SPRITE_SIZE });
	int mining_message_length = 2;
	int mining_message[2] = { UI_ACTION_CATEGORY_SUPPORTING,UI_ACTION_SUPPORTING_TYPE_SET_ASTEROID_MINE };
	UI_Static_Button_Mark_Asteroid_For_Mining.Set_Click_Message(mining_message_length, mining_message);
}

void UI_Window_Screen_Buttons::Draw()
{
	UI_Static_Button_Delete_Structure.Draw(service_locator->get_Draw_System_Pointer(),base_window_rect);
	UI_Static_Button_Set_Rally_Point.Draw(service_locator->get_Draw_System_Pointer(), base_window_rect);
	UI_Static_Button_Mark_Asteroid_For_Mining.Draw(service_locator->get_Draw_System_Pointer(), base_window_rect);
}

void UI_Window_Screen_Buttons::Respond_To_Mouse()
{
	// Check to see if there's any need to move the window or change panels
	//UI_Window_Generic::Respond_To_Mouse(service_locator->get_Cursor_Pointer());

	// Check to see if anyof the buttons are clicked
	UI_Static_Button_Delete_Structure.Check_For_Click();
	UI_Static_Button_Set_Rally_Point.Check_For_Click();
	UI_Static_Button_Mark_Asteroid_For_Mining.Check_For_Click();
}