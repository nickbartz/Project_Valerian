#include<Console_Component.h>
#include<Cursor.h>
#include<Service_Locator.h>
#include<Game_Library.h>
#include<UI.h>
#include<AI_Item_Component.h>
#include<Game_Library.h>
#include<Object.h>
#include<AI_Stats_Component.h>
#include<algorithm>

UI_Component_Generic::UI_Component_Generic(Global_Service_Locator* sLocator, SDL_Rect placement_rect, bool component_is_filled, SDL_Color bColor, bool is_highlight, SDL_Color hColor)
{
	service_locator = sLocator;
	
	offset_rect = placement_rect;
	filled = component_is_filled;
	fill_color = bColor;
	highlight = is_highlight;
	highlight_color = hColor;
}

void UI_Component_Button_List::Add_Button_To_Button_List(UI_Component_Generic new_button)
{
	button_list.push_back(new_button);
	Adjust_Button_List_Buttons();
	scroll_bar.Update_Scroll_Bar(button_list.size());
}

void UI_Component_User_Input_Box::Add_Char_To_Box(char* input)
{
	text_input.push_back(*input);
}

void UI_Component_Button_List::Adjust_Button_List_Buttons()
{
	int num_buttons = button_list.size();
	int current_y = offset_rect.y;
	int y_offset = 0;

	for (int i = 0; i < num_buttons ; i++)
	{
		if (i >= current_button_index && i < current_button_index + num_buttons_to_display)
		{
			button_list[i].Change_Offset_Rect(1, offset_rect.x);
			button_list[i].Change_Offset_Rect(2, current_y + y_offset * button_height);
			button_list[i].Change_Offset_Rect(3, offset_rect.w - scroll_bar_width);
			button_list[i].Change_Offset_Rect(4, button_height);
			y_offset++;
		}
		else
		{
			button_list[i].Change_Offset_Rect(4, 0);
		}
	}
}

void UI_Component_Generic::Assign_Window(int window_name)
{
	window = window_name;
}

void UI_Component_Generic::Assign_Panel(int panel_name)
{
	panel = panel_name;
}

void UI_Component_User_Input_Box::Collect_Input_Messages()
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->count_input_messages; i++)
	{
		Message_Input* new_input_message = &service_locator->get_MB_Pointer()->Input_Message_Array[i];
		if (new_input_message->Return_Event().type == SDL_TEXTINPUT)
		{
			Add_Char_To_Box(new_input_message->Return_Event().text.text);
		}

	}
}

bool UI_Component_Generic::Currently_Clicked()
{
	return currently_clicked;
}

bool UI_Component_Generic::Currently_Clicked(bool state_override)
{
	currently_clicked = state_override;
	return currently_clicked;
}

bool UI_Component_Generic::Check_If_Click_In_Component_Rect(SDL_Point cursor_position)
{
	if (SDL_PointInRect(&cursor_position, &current_screen_rect)) return true;
	else return false;
}

void UI_Component_Generic::Change_Component_Title(string new_title, SDL_Rect t_offset, SDL_Color t_color)
{
	title = new_title;
	draw_title = true;
	text_offset = t_offset;
	text_color = t_color;
}

void UI_Component_Message_Stream::Clear_All_Messages_From_Stream()
{
	for (int i = 0; i < max_message_lines; i++)
	{
		message_array[i] = "-";
	}
}

void UI_Component_Generic::Change_Offset_Rect(int argument, int new_value)
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

void UI_Component_Generic::Draw(SDL_Rect base_rect)
{
	SDL_Rect draw_rect = { base_rect.x + offset_rect.x, base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h };
	
	current_screen_rect = draw_rect;

	Draw_System* draw_system = service_locator->get_Draw_System_Pointer();

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

void UI_Component_Message_Stream::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);

	int spacer = 0;
	for (int i = 0; i < max_message_lines; i++)
	{
		SDL_Rect draw_rect = { base_rect.x + offset_rect.x + 5, base_rect.y + offset_rect.y + spacer*20 + 5, offset_rect.w, offset_rect.h };
		//THIS IS DUMB JUST TO GET A SINGLE LINE TO SHOW UP BOLD IN MESSAGE FEED - NEED A MORE THOROUGH IMPLEMENTATION
		int font = font_type;
		if (i == bold_line) font = FONT_SMALL_BOLD;
		Draw_System::Text_Instruction new_text_instruction = { 1,draw_rect, font,message_array[i] };
		service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle(new_text_instruction);
		spacer++;
	}
}

void UI_Component_Graphic_Button::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);

	if (template_id != 0 || sprite_clip_override) service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, { base_rect.x + offset_rect.x , base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h }, sprite_clip);
}

void UI_Component_Item_Slot_Button::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);

	if (slot_pointer != NULL && slot_pointer->item_quantity > 0)
	{
		sprite_clip = service_locator->get_Game_Library()->Fetch_Item_Template(slot_pointer->slot_item.item_template_id)->sprite_specs;
		service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, { base_rect.x + offset_rect.x , base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h }, sprite_clip);

		Set_Font_Type(FONT_SMALL_BOLD);
		string quantity = to_string(slot_pointer->item_quantity);

		int text_width = service_locator->get_Draw_System_Pointer()->Return_Text_Width(font_type, quantity);
		int text_height = service_locator->get_Draw_System_Pointer()->Return_Text_Height(font_type, quantity);
		SDL_Rect text_rect = { base_rect.x + offset_rect.x + SPRITE_SIZE - text_width - 2, base_rect.y + offset_rect.y + SPRITE_SIZE - text_height, text_width, text_height };
		SDL_Rect background_rect = { base_rect.x + offset_rect.x + SPRITE_SIZE - text_width - 5, base_rect.y + offset_rect.y + SPRITE_SIZE - text_height + 1, text_width + 5, text_height - 1 };
		service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(2, background_rect, true, { 50,50,25,255 });
		service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(2, background_rect, false, { 255,255,255,255 });
		service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle({ 2,text_rect,font_type,quantity, text_color });
	}
}

void UI_Component_Stat_Button::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);
	AI_Stats_Component* ai_stats = reference_object->Return_Stats_Component();
	int stat_value = ai_stats->Return_Stat_Value(stat_name);
	SDL_Rect text_rect = { base_rect.x + offset_rect.x + 3, base_rect.y + offset_rect.y + 5, offset_rect.w, offset_rect.h };
	SDL_Rect stat_rect = { base_rect.x + offset_rect.x + 3 + string_width_offset, base_rect.y + offset_rect.y + 5, offset_rect.w, offset_rect.h };
	service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle({ 1,text_rect,FONT_DEFAULT, stat_string_name,{ 255,255,255,255 } });
	service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle({ 1,stat_rect,FONT_DEFAULT, to_string(stat_value) ,{ 255,255,255,255 } });
}

void UI_Component_Button_List::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);
	
	scroll_bar.Draw(base_rect);

	int num_elements_in_array = button_list.size();

	for (int i = current_button_index; i < (current_button_index + min(num_elements_in_array, num_buttons_to_display)); i++)
	{
		button_list[i].Draw(base_rect);
	}
}

void UI_Component_Scroll_Bar::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);
	up_arrow.Draw(base_rect);
	down_arrow.Draw(base_rect);
	current_location_range.Draw(base_rect);
	current_location_indicator.Draw(base_rect);
}

void UI_Component_Details_Display::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);
}

void UI_Component_User_Input_Box::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);

	text_offset.x = 5;
	text_offset.y = 3;

	SDL_Rect text_rect = { base_rect.x + offset_rect.x + text_offset.x, base_rect.y + offset_rect.y + text_offset.y, offset_rect.w, offset_rect.h };
	Draw_System::Text_Instruction new_text_instruction = { 1,text_rect,font_type, text_input, text_color };
	service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle(new_text_instruction);
}

void UI_Component_Graphic_Button::Fetch_Sprite_Details_From_Object_ID()
{
	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
	{
		Structure_Template* new_structure = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(template_id);
		sprite_clip = { new_structure->icon_clip_x,new_structure->icon_clip_y, SPRITE_SIZE, SPRITE_SIZE };
	}
	break;
	case OBJECT_TYPE_ENTITY:
		break;
	case OBJECT_TYPE_ITEM:
	{
		Item_Template* new_item = service_locator->get_Game_Library()->Fetch_Item_Template(template_id);
		sprite_clip = { new_item->sprite_specs.x, new_item->sprite_specs.y, SPRITE_SIZE, SPRITE_SIZE };
	}
	break;
	}
}

int UI_Component_Generic::Get_Font_Type()
{
	return font_type;
}

string UI_Component_Generic::Get_Title()
{
	return title;
}

int UI_Component_Generic::Get_Window()
{
	return window;
}

int UI_Component_Generic::Get_Panel()
{
	return panel;
}

void UI_Component_Generic::Handle_Button_Click()
{
	Cursor* cursor = service_locator->get_Cursor_Pointer();

	SDL_Point new_point = cursor->Get_Mouse_Position();

	if (Currently_Clicked() == false && cursor->left_button == true && cursor->left_button_previous == false)
	{
		Currently_Clicked(true);
		cursor->Set_Currently_Clicked_Component(this);
		service_locator->get_UI_pointer()->Load_Action_To_UI(message_length, message_array);
	}
}

int UI_Component_Scroll_Bar::Handle_Click_On_Component()
{
	SDL_Point cursor_position = service_locator->get_Cursor_Pointer()->Get_Mouse_Position();
	int increment = 0;

	if (service_locator->get_Cursor_Pointer()->Get_Recent_Mouse_Action() == INPUT_LEFT_MOUSE_CLICK)
	{
		if (up_arrow.Check_If_Click_In_Component_Rect(cursor_position))
		{
			if (current_object_index > 0)
			{
				increment = -1;
				current_object_index--;
				current_location_indicator.Increment_Offset_Rect(2, -increment_distance);
			}
		}
		else if (down_arrow.Check_If_Click_In_Component_Rect(cursor_position))
		{
			if ((current_object_index + num_objects_displayed_at_once) < (num_scroll_elements))
			{
				increment = 1;
				current_object_index++;
				current_location_indicator.Increment_Offset_Rect(2, increment_distance);
			}
		}
	}

	return increment;
}

void UI_Component_Button_List::Handle_Click_On_Component()
{
	if (scroll_bar.Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position()))
	{
		int increment = scroll_bar.Handle_Click_On_Component();

		current_button_index += increment;
		Adjust_Button_List_Buttons();
	}
}

void UI_Component_Generic::Init()
{

}

void UI_Component_Graphic_Button::Init(int oType, int temp_id, SDL_Rect icon_clip)
{
	object_type = oType;
	template_id = temp_id;
	
	if (object_type != OBJECT_TYPE_NULL)
	{
		Fetch_Sprite_Details_From_Object_ID();
	}
	else
	{
		sprite_clip = icon_clip;
		sprite_clip_override = true;
	}

}

void UI_Component_Item_Slot_Button::Init(Item_Slot* sPointer)
{
	slot_pointer = sPointer;
}

void UI_Component_Stat_Button::Init(Object* ref_object, int s_name, string stat_s_name)
{
	reference_object = ref_object;
	stat_name = s_name;
	stat_string_name = stat_s_name;
}

void UI_Component_Scroll_Bar::Init(int length, int bHeight, int num_displayed)
{
	scroll_bar_length = length;
	current_object_index = 0;
	num_objects_displayed_at_once = num_displayed;
	button_height = bHeight;

	up_arrow = UI_Component_Graphic_Button(service_locator, { offset_rect.x, offset_rect.y, button_height, button_height });
	down_arrow = UI_Component_Graphic_Button(service_locator, { offset_rect.x , offset_rect.y + offset_rect.h - button_height, button_height, button_height });
	current_location_indicator = UI_Component_Generic(service_locator, { offset_rect.x, offset_rect.y + button_height, button_height, button_height }, true, {177,177,177,200});
	current_location_range = UI_Component_Generic(service_locator, { offset_rect.x, offset_rect.y + button_height, button_height, scroll_bar_length }, true, { 50,50,90,200 });

	up_arrow.Init(OBJECT_TYPE_NULL, 0, { 0 * SPRITE_SIZE,2 * SPRITE_SIZE,1 * SPRITE_SIZE,1 * SPRITE_SIZE });
	down_arrow.Init(OBJECT_TYPE_NULL, 0, { 1 * SPRITE_SIZE,2 * SPRITE_SIZE,1 * SPRITE_SIZE,1 * SPRITE_SIZE });
}

void UI_Component_Button_List::Init()
{
	scroll_bar.Init(offset_rect.h - 2*button_height, button_height, num_buttons_to_display);
}

void UI_Component_Details_Display::Init()
{
}

void UI_Component_User_Input_Box::Init()
{
}

void UI_Component_Generic::Increment_Offset_Rect(int argument, int increment)
{
	switch (argument)
	{
	case 1:
		offset_rect.x += increment;
		break;
	case 2:
		offset_rect.y += increment;
		break;
	case 3:
		offset_rect.w += increment;
		break;
	case 4:
		offset_rect.h += increment;
		break;
	}
}

void UI_Component_Generic::Make_Button(string button_title, SDL_Rect tOffset, int bAction, SDL_Color tColor)
{
	// THIS FUNCTION IS DEPRECATED DO NOT USE
	title = button_title;
	text_offset = tOffset;
	text_color = tColor;
	draw_title = true;
}

void UI_Component_Message_Stream::Push_Message_Into_Stream(string new_message)
{
	if (new_message.size() == 0) return;

	for (int i = max_message_lines - 1; i > 0; --i)
	{
		message_array[i] = message_array[i - 1];
	}

	message_array[0] = new_message;
}

SDL_Rect UI_Component_Generic::Return_Rect()
{
	return current_screen_rect;
}

void UI_Component_Message_Stream::Set_Bold_Line(int bLine)
{
	bold_line = bLine;
}

void UI_Component_Message_Stream::Set_Message_At_Array_Num(string new_message, int array_num)
{
	message_array[array_num] = new_message;
}

void UI_Component_Generic::Set_Font_Type(int fType)
{
	font_type = fType;
}

void UI_Component_Generic::Set_Background_Color(SDL_Color new_background_color)
{
	fill_color = new_background_color;
}

void UI_Component_Generic::Set_Click_Message(int mLength, int mArray[])
{
	for (int i = 0; i < mLength; i++)
	{
		message_array[i] = mArray[i];
	}

	is_button = true;
	message_length = mLength;
}

void UI_Component_Scroll_Bar::Update_Scroll_Bar(int num_elements)
{
	num_scroll_elements = num_elements;
	increment_distance = scroll_bar_length / max(1,(num_scroll_elements - num_objects_displayed_at_once + 1));
}