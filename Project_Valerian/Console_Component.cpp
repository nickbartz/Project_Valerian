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
void UI_Component_Generic::Assign_Window(int window_name)
{
	window = window_name;
}
void UI_Component_Generic::Assign_Panel(int panel_name)
{
	panel = panel_name;
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
bool UI_Component_Generic::Currently_Focus()
{
	return currently_is_focus;
}
bool UI_Component_Generic::Currently_Focus(bool state_override)
{
	currently_is_focus = state_override;
	return currently_is_focus;
}
bool UI_Component_Generic::Check_If_Click_In_Component_Rect(SDL_Point cursor_position)
{
	if (SDL_PointInRect(&cursor_position, &current_screen_rect) && service_locator->get_Cursor_Pointer()->Get_Recent_Mouse_Action() == INPUT_LEFT_MOUSE_CLICK) return true;
	else return false;
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
		if (text_is_multirow == false)
		{
			SDL_Rect text_rect = { base_rect.x + offset_rect.x + text_offset.x, base_rect.y + offset_rect.y + text_offset.y, offset_rect.w, offset_rect.h };
			draw_system->Add_Text_Job_To_Render_Cycle(1, text_rect, title, font_type, text_color);
		}
		else if (text_is_multirow == true)
		{
			for (int i = 0; i < multi_row_text.size(); i++)
			{
				SDL_Rect text_rect = { base_rect.x + offset_rect.x + multi_row_text[i].row_rect.x, base_rect.y + offset_rect.y + multi_row_text[i].row_rect.y, offset_rect.w, multi_row_text[i].row_rect.h };
				draw_system->Add_Text_Job_To_Render_Cycle(1, text_rect, multi_row_text[i].row_text, font_type, text_color);
			}
		}

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
int UI_Component_Generic::Get_Message_Length()
{
	return message_length;
}
SDL_Rect UI_Component_Generic::Get_Offset_Rect()
{
	return offset_rect;
}
vector<int> UI_Component_Generic::Get_Message_In_Vector()
{
	vector<int> message;

	for (int i = 0; i < message_length; i++)
	{
		message.push_back(message_array[i]);
	}

	return message;
}
void UI_Component_Generic::Handle_Button_Click()
{
	Cursor* cursor = service_locator->get_Cursor_Pointer();

	SDL_Point new_point = cursor->Get_Mouse_Position();

	if (Currently_Clicked() == false && cursor->left_button == true && cursor->left_button_previous == false)
	{
		service_locator->get_UI_pointer()->Set_Currently_Clicked_Component(this);
		service_locator->get_UI_pointer()->Load_Action_To_UI(message_length, message_array);
	}
}
void UI_Component_Generic::Init()
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
SDL_Rect UI_Component_Generic::Return_Rect()
{
	return current_screen_rect;
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
void UI_Component_Generic::Set_Highlight(bool highlight_bool)
{
	highlight = highlight_bool;
}

void UI_Component_Generic::Update_Component_Title(string new_title, SDL_Color t_color, int alignment)
{
	draw_title = true;
	title = new_title;

	// Split the line up into newlines if it is too long for the box it's in
	if (service_locator->get_Draw_System_Pointer()->Return_Text_Width(font_type, title) >= offset_rect.w)
	{
		int start = 0;
		int last_available_break = 0;

		for (int i = 0; i < title.size(); i++)
		{
			if (title[i] == ' ')
			{
				string::const_iterator first = title.begin() + start;
				string::const_iterator last = title.begin() + i;
				string new_line(first, last);

				if (service_locator->get_Draw_System_Pointer()->Return_Text_Width(font_type, new_line) >= offset_rect.w)
				{
					title.insert(title.begin() + last_available_break, '\n');
					start = last_available_break;
				}
				else
				{
					last_available_break = i;
				}
			}
		}
	}

	text_offset = service_locator->get_Draw_System_Pointer()->Get_Offset_Rect_For_Text(offset_rect, font_type, title, alignment);

	text_color = t_color;
}

void UI_Component_Button_List::Add_Button_To_Button_List(UI_Component_Generic new_button)
{
	button_list.push_back(new_button);
	Adjust_Button_List_Buttons();
	scroll_bar.Update_Scroll_Bar(button_list.size());
}
void UI_Component_Button_List::Adjust_Button_List_Buttons()
{
	int num_buttons = button_list.size();
	int current_y = offset_rect.y;
	int y_offset = 0;

	for (int i = 0; i < num_buttons; i++)
	{
		if (i >= current_button_index && i < current_button_index + num_buttons_to_display)
		{
			button_list[i].Change_Offset_Rect(1, offset_rect.x);
			button_list[i].Change_Offset_Rect(2, current_y + y_offset * button_height);
			button_list[i].Change_Offset_Rect(3, offset_rect.w - scroll_bar_width);
			button_list[i].Change_Offset_Rect(4, button_height);
			y_offset++;
			button_list[i].Update_Component_Title(button_list[i].Get_Title(), { 255,255,255,255 }, 0);
		}
		else
		{
			button_list[i].Change_Offset_Rect(4, 0);
		}
	}
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
void UI_Component_Button_List::Init()
{
	scroll_bar.Init(offset_rect.h - 2 * button_height, button_height, num_buttons_to_display);
}
UI_Component_Generic* UI_Component_Button_List::Handle_Click_On_Component()
{
	UI_Component_Generic::Handle_Button_Click();

	if (scroll_bar.Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position()))
	{
		int increment = scroll_bar.Handle_Click_On_Component();

		current_button_index += increment;
		Adjust_Button_List_Buttons();
	}
	else
	{
		for (int i = 0; i < button_list.size(); i++)
		{
			if (button_list[i].Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position()))
			{
				return &button_list[i];
			}
		}
	}

	return NULL;
}

void UI_Component_User_Input_Box::Add_Char_To_Box(char* input)
{
	if (is_num == true && text_input.size() < 3)
	{
		if (input[0] >= '0' && input[0] <= '9')
		{
			text_input.push_back(*input);
			Update_Component_Title(text_input, { 255,255,255,255 });
			number = stoi(text_input);
		}
	}
	else if (is_num == false)
	{
		text_input.push_back(*input);
		Update_Component_Title(text_input, { 255,255,255,255 });
	}
}
void UI_Component_User_Input_Box::Remove_Char_From_Box()
{
	if (text_input.size() > 0)
	{
		text_input.pop_back();
		Update_Component_Title(text_input, { 255,255,255,255 });
		
	}

	if (text_input.size() > 0 && is_num == true) number = stoi(text_input);
}
void UI_Component_User_Input_Box::Check_For_User_Input()
{
	for (int i = 0; i < service_locator->get_MB_Pointer()->count_input_messages; i++)
	{
		Message_Input* new_input_message = &service_locator->get_MB_Pointer()->Input_Message_Array[i];
		if (new_input_message->Return_Event().type == SDL_KEYDOWN && new_input_message->Return_Event().key.keysym.sym == SDLK_BACKSPACE)
		{
			Remove_Char_From_Box();
		}
		else if (new_input_message->Return_Event().type == SDL_TEXTINPUT)
		{
			Add_Char_To_Box(new_input_message->Return_Event().text.text);
		}
	}
}
void UI_Component_User_Input_Box::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);

	SDL_Rect focus_rect = { base_rect.x + offset_rect.x, base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h };

	if (currently_is_focus) service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(1, focus_rect, true, { 150,100,100,100 }, PRIMITIVE_TYPE_RECT);
}
void UI_Component_User_Input_Box::Handle_Click_On_Component()
{
	UI_Component_Generic::Handle_Button_Click();

	service_locator->get_UI_pointer()->Set_Current_Focus_Component(this);
}
int UI_Component_User_Input_Box::Get_Current_Num()
{
	return number;
}
void UI_Component_User_Input_Box::Init(bool just_num)
{
	is_num = just_num;
}

void UI_Component_Message_Stream::Clear_All_Messages_From_Stream()
{
	for (int i = 0; i < max_message_lines; i++)
	{
		message_array[i] = "-";
	}

	num_messages_in_stream = 0;
}
void UI_Component_Message_Stream::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);

	SDL_Rect draw_rect = { base_rect.x + offset_rect.x + 5, base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h };
	string console_string;

	for (int i = 0; i < min(num_messages_in_stream, max_message_lines); i++)
	{
		if (i == bold_line)
		{
			console_string += " --> ";
		}
		console_string += '-';
		console_string += message_array[i];
		console_string += ' ';
		console_string += '\n';
	}

	service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle(1, draw_rect, console_string, font_type);
}
void UI_Component_Message_Stream::Push_Message_Into_Stream(string new_message)
{
	if (new_message.size() == 0) return;

	for (int i = max_message_lines - 1; i > 0; --i)
	{
		message_array[i] = message_array[i - 1];
	}

	message_array[0] = new_message;

	num_messages_in_stream++;
}
void UI_Component_Message_Stream::Set_Bold_Line(int bLine)
{
	bold_line = bLine;
}
void UI_Component_Message_Stream::Set_Message_At_Array_Num(string new_message, int array_num)
{
	message_array[array_num] = new_message;
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
		service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle(2,text_rect,quantity, font_type,text_color);
	}
}
void UI_Component_Item_Slot_Button::Init(Item_Slot* sPointer)
{
	slot_pointer = sPointer;
}
Item_Slot* UI_Component_Item_Slot_Button::Return_Slot_Pointer()
{
	if (slot_pointer != NULL) return slot_pointer;
	else service_locator->get_UI_pointer()->Push_Message_To_Console("Item slot button trying to return NULL pointer");
}

void UI_Component_Stat_Button::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);
	AI_Stats_Component* ai_stats = reference_object->Return_Stats_Component();
	int stat_value = ai_stats->Return_Stat_Value(stat_name);
	SDL_Rect text_rect = { base_rect.x + offset_rect.x + 3, base_rect.y + offset_rect.y + 5, offset_rect.w, offset_rect.h };
	SDL_Rect stat_rect = { base_rect.x + offset_rect.x + 3 + string_width_offset, base_rect.y + offset_rect.y + 5, offset_rect.w, offset_rect.h };
	service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle( 1,text_rect, stat_string_name,FONT_DEFAULT,{ 255,255,255,255 } );
	service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle( 1,stat_rect, to_string(stat_value),FONT_DEFAULT,{ 255,255,255,255 });
}
void UI_Component_Stat_Button::Init(Object* ref_object, int s_name, string stat_s_name)
{
	reference_object = ref_object;
	stat_name = s_name;
	stat_string_name = stat_s_name;
}

void UI_Component_Object_Details_Display::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);

	object_icon.Draw(base_rect);
	object_title.Draw(base_rect);
	object_description.Draw(base_rect);

	for (int i = 0; i < num_requirements; i++)
	{
		object_requirements[i].Draw(base_rect);
	}
}
void UI_Component_Object_Details_Display::Init(Object* owner_object, int object_type, int object_template_id)
{
	if (object_type != OBJECT_TYPE_NULL)
	{
		string title = "";
		string description = "";
		num_requirements = 0;
		int blueprint_id = 0;

		switch (object_type)
		{
			case OBJECT_TYPE_ITEM:
				{
					Item_Template* item = service_locator->get_Game_Library()->Fetch_Item_Template(object_template_id);
					if (item != NULL && object_template_id != 0)
					{
						title = item->inventory_item_name;
						description = item->inventory_item_description;
						blueprint_id = item->associated_blueprint_id;
					}
				}
			break;
		}

		// Initialize the title and the object icon
		object_icon.Init(object_type, object_template_id);
		object_title.Update_Component_Title(title, { 255,255,255,255 }, 0);
		object_title.Set_Font_Type(FONT_DEFAULT_BOLD);
		object_description.Update_Component_Title(description, { 255,255,255,255 }, 0);

		// Initialize the requirements if they exist
		Blueprint* blueprint = service_locator->get_Game_Library()->Fetch_Blueprint(blueprint_id);
		if (blueprint != NULL)
		{
			num_requirements = blueprint->Num_Items_In_Blueprint;
			for (int i = 0; i < MAX_ITEMS_PER_BLUEPRINT; i++)
			{
				if (i < num_requirements)
				{
					current_requirement_slots[i] = blueprint->blueprint_items[i];
					object_requirements[i].Init(&current_requirement_slots[i]);
				}
				else
				{
					current_requirement_slots[i].item_quantity = 0;
				}
			}

		}
	}
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
void UI_Component_Graphic_Button::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);

	if (template_id != 0 || sprite_clip_override) service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, { base_rect.x + offset_rect.x , base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h }, sprite_clip);
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

int UI_Component_Scroll_Bar::Handle_Click_On_Component()
{
	UI_Component_Generic::Handle_Button_Click();

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
void UI_Component_Scroll_Bar::Init(int length, int bHeight, int num_displayed)
{
	scroll_bar_length = length;
	current_object_index = 0;
	num_objects_displayed_at_once = num_displayed;
	button_height = bHeight;

	up_arrow = UI_Component_Graphic_Button(service_locator, { offset_rect.x, offset_rect.y, button_height, button_height });
	down_arrow = UI_Component_Graphic_Button(service_locator, { offset_rect.x , offset_rect.y + offset_rect.h - button_height, button_height, button_height });
	current_location_indicator = UI_Component_Generic(service_locator, { offset_rect.x, offset_rect.y + button_height, button_height, button_height }, true, { 177,177,177,200 });
	current_location_range = UI_Component_Generic(service_locator, { offset_rect.x, offset_rect.y + button_height, button_height, scroll_bar_length }, true, { 50,50,90,200 });

	up_arrow.Init(OBJECT_TYPE_NULL, 0, { 0 * SPRITE_SIZE,2 * SPRITE_SIZE,1 * SPRITE_SIZE,1 * SPRITE_SIZE });
	down_arrow.Init(OBJECT_TYPE_NULL, 0, { 1 * SPRITE_SIZE,2 * SPRITE_SIZE,1 * SPRITE_SIZE,1 * SPRITE_SIZE });
}
void UI_Component_Scroll_Bar::Update_Scroll_Bar(int num_elements)
{
	num_scroll_elements = num_elements;
	increment_distance = scroll_bar_length / max(1, (num_scroll_elements - num_objects_displayed_at_once + 1));
}
void UI_Component_Scroll_Bar::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);
	up_arrow.Draw(base_rect);
	down_arrow.Draw(base_rect);
	current_location_range.Draw(base_rect);
	current_location_indicator.Draw(base_rect);
}

void UI_Component_Item_Slot_Array::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(base_rect);
	for (int i = 0; i < num_panel_buttons; i++)
	{
		item_slot_array[i].Draw(base_rect);
	}
}
Item_Slot* UI_Component_Item_Slot_Array::Handle_Click_On_Component()
{
	for (int i = 0; i < num_panel_buttons; i++)
	{
		if (item_slot_array[i].Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position()))
		{
			return item_slot_array[i].Return_Slot_Pointer();
		}
	}
}
void UI_Component_Item_Slot_Array::Init(Item_Slot array_pointer[], int num_item_slots)
{
	int current_row = 0;
	int current_column = 0;

	int x_offset = (offset_rect.w - num_columns * SPRITE_SIZE) / 2;
	int y_offset = (offset_rect.h - num_rows * SPRITE_SIZE) / 2;

	for (int i = 0; i < num_item_slots; i++)
	{
		item_slot_array.push_back(UI_Component_Item_Slot_Button(service_locator, { offset_rect.x + x_offset + current_column * SPRITE_SIZE, offset_rect.y + y_offset + current_row * SPRITE_SIZE, SPRITE_SIZE ,SPRITE_SIZE }));
		item_slot_array.back().Init(&array_pointer[i]);
		current_column++;
		num_panel_buttons++;
		if (current_column >= num_columns) current_row++, current_column = 0;
	}
}
	