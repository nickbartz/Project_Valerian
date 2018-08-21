#include<Console_Window.h>
#include<Cursor.h>


Console_Window::Console_Window(int window_name, string window_title, SDL_Rect base_rect)
{
	base_window_rect = base_rect;

	window_title_bar = Console_Component({ 0,0,base_rect.w,window_title_height }, true, { 200,50,50,255 });
	window_title_bar.Assign_Window(window_name);
	window_title_bar.Make_Button(window_title, { 5,3,0,0 }, BUTTON_ACTION_DRAG_WINDOW, { 255,255,255,255 });
	window_title_bar.Set_Font_Type(FONT_LARGE_BOLD);
}

SDL_Rect Console_Window::Return_Rect()
{
	return base_window_rect;
}

void Console_Window::Respond_To_Mouse(Cursor* cursor)
{
	window_title_bar.Check_For_Click(cursor);
	if (window_title_bar.Currently_Clicked() == true) Move_Window(cursor);
	
	SDL_Point new_point = { cursor->current_mouse_x, cursor->current_mouse_y };
	
	if (SDL_PointInRect(&new_point, &base_window_rect))
	{
		for (int i = 0; i < active_panel_headers; i++)
		{
			panel_headers[i].Check_For_Click(cursor);
			if (panel_headers[i].Currently_Clicked() == true)
			{
				currently_active_panel = panel_headers[i].Get_Panel();
			}
		}
	}
}

void Console_Window::Change_Rect(SDL_Rect new_rect)
{
	base_window_rect = new_rect;
}

void Console_Window::Draw(Draw_System* draw_system)
{
	window_title_bar.Draw(draw_system, base_window_rect);
	
	for (int i = 0; i < active_panel_headers; i++)
	{
		panel_headers[i].Draw(draw_system, base_window_rect);
	}
}

void Console_Window::Create_Panel_Header(SDL_Rect pos_rect, int panel_name, string panel_title, SDL_Rect text_offset, SDL_Color text_color)
{
	panel_headers[active_panel_headers] = Console_Component(pos_rect,true);
	panel_headers[active_panel_headers].Assign_Window(window_name);
	panel_headers[active_panel_headers].Assign_Panel(panel_name);
	panel_headers[active_panel_headers].Make_Button(panel_title, text_offset, BUTTON_ACTION_SWITCH_PANEL, text_color);
	active_panel_headers++;
}

void Console_Window::Size_Panel_Headers()
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

void Console_Window::Move_Window(Cursor* cursor)
{
	base_window_rect.x += cursor->current_mouse_x - cursor->last_mouse_x;
	base_window_rect.y += cursor->current_mouse_y - cursor->last_mouse_y;
}

void Player_Diagnostic_Window::Respond_To_Mouse(Cursor* cursor)
{	
	Console_Window::Respond_To_Mouse(cursor);
}

void Player_Diagnostic_Window::Draw(Draw_System* draw_system)
{
	Console_Window::Draw(draw_system);

	if (currently_active_panel == message_panel.Get_Panel_Name()) message_panel.Draw(draw_system, base_window_rect);
}

void Player_Diagnostic_Window::Push_Message_Into_Stream(string message)
{
	message_panel.Push_Message_Into_Stream(message);
}
