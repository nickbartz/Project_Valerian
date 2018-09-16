#include<Console_Component.h>
#include<Cursor.h>

Console_Component::Console_Component(SDL_Rect placement_rect, bool component_is_filled, SDL_Color bColor, bool is_highlight, SDL_Color hColor)
{
	offset_rect = placement_rect;
	filled = component_is_filled;
	fill_color = bColor;
	highlight = is_highlight;
	highlight_color = hColor;
}

// Functions that determine what type of component this becomes

void Console_Component::Make_Button(string button_title, SDL_Rect tOffset, int bAction, SDL_Color tColor)
{
	title = button_title;
	text_offset = tOffset;
	text_color = tColor;
	draw_title = true;
}

void Console_Component::Set_Font_Type(int fType)
{
	font_type = fType;
}

void Console_Component::Assign_Window(int window_name)
{
	window = window_name;
}

void Console_Component::Assign_Panel(int panel_name) 
{
	panel = panel_name;
}

void Console_Component::Change_Offset_Rect(int argument, int new_value)
{
	switch (argument)
	{
	case 1:
		offset_rect.x = new_value;
		break;
	case 2:
		offset_rect.y = new_value;
		break;
	case 3:
		offset_rect.w = new_value;
		break;
	case 4:
		offset_rect.h = new_value;
		break;
	}
}

// Functions that access certain components

SDL_Rect Console_Component::Return_Rect()
{
	return current_screen_rect;
}

int Console_Component::Get_Font_Type()
{
	return font_type;
}

string Console_Component::Get_Title()
{
	return title;
}

int Console_Component::Get_Window()
{
	return window;
}

int Console_Component::Get_Panel()
{
	return panel;
}

// Functions that check for clicks 

bool Console_Component::Currently_Clicked()
{
	return currently_clicked;
}

bool Console_Component::Currently_Clicked(bool state_override)
{
	currently_clicked = state_override;
	return currently_clicked;
}

void Console_Component::Check_For_Click(Cursor* cursor)
{
	SDL_Point new_point = { cursor->current_mouse_x, cursor->current_mouse_y };

	if (SDL_PointInRect(&new_point, &current_screen_rect))
	{
		if (Currently_Clicked() == false && cursor->left_button == 1 && cursor->left_button_previous == 0)
		{
			Currently_Clicked(true);
			cursor->Set_Currently_Clicked_Component(this);
		}
	}
}

// Draw Functions

void Console_Component::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	SDL_Rect draw_rect = { base_rect.x + offset_rect.x, base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h };
	current_screen_rect = draw_rect;

	if (filled == true)
	{
		SDL_Color new_fill = { fill_color.r, fill_color.g, fill_color.b, fill_color.a };
		if (currently_clicked == true)
		{
			new_fill.r -= 50;
			new_fill.g -= 50;
			new_fill.b -= 50;
		}
		
		draw_system->Add_Primitive_To_Render_Cycle(1, draw_rect, true, new_fill);
	}

	if (highlight == true)
	{
		draw_system->Add_Primitive_To_Render_Cycle(1, draw_rect, false, highlight_color);
	}

	if (draw_title == true)
	{
		SDL_Rect text_rect = { base_rect.x + offset_rect.x + text_offset.x, base_rect.y + offset_rect.y + text_offset.y, offset_rect.w, offset_rect.h };
		Draw_System::Text_Instruction new_text_instruction = { 1,text_rect,font_type, title, text_color };
		draw_system->Add_Text_Job_To_Render_Cycle(new_text_instruction);
	}
}

// Message_Stream Functions

void Message_Stream::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	Console_Component::Draw(draw_system, base_rect);

	int spacer = 0;
	for (int i = 0; i < max_message_lines; i++)
	{
		SDL_Rect draw_rect = { base_rect.x + offset_rect.x + 5, base_rect.y + offset_rect.y + spacer*20 + 5, offset_rect.w, offset_rect.h };
		Draw_System::Text_Instruction new_text_instruction = { 1,draw_rect, font_type,message_array[i] };
		draw_system->Add_Text_Job_To_Render_Cycle(new_text_instruction);
		spacer++;
	}
}

void Message_Stream::Push_Message_Into_Stream(string new_message)
{
	if (new_message.size() == 0) return;

	for (int i = max_message_lines - 1; i > 0; --i)
	{		
		message_array[i] = message_array[i - 1];
	}

	message_array[0] = new_message;
}