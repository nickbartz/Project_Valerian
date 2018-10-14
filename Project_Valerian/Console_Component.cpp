#include<Console_Component.h>
#include<Cursor.h>
#include<Service_Locator.h>
#include<Game_Library.h>
#include<UI.h>
#include<AI_Item_Component.h>
#include<Game_Library.h>

UI_Component_Generic::UI_Component_Generic(Global_Service_Locator* sLocator, SDL_Rect placement_rect, bool component_is_filled, SDL_Color bColor, bool is_highlight, SDL_Color hColor)
{
	service_locator = sLocator;
	
	offset_rect = placement_rect;
	filled = component_is_filled;
	fill_color = bColor;
	highlight = is_highlight;
	highlight_color = hColor;
}

// Functions that determine what type of component this becomes

void UI_Component_Generic::Init()
{

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

// DEPERECTATED DO NOT USE
void UI_Component_Generic::Make_Button(string button_title, SDL_Rect tOffset, int bAction, SDL_Color tColor)
{
	title = button_title;
	text_offset = tOffset;
	text_color = tColor;
	draw_title = true;
}

void UI_Component_Generic::Set_Font_Type(int fType)
{
	font_type = fType;
}

void UI_Component_Generic::Assign_Window(int window_name)
{
	window = window_name;
}

void UI_Component_Generic::Assign_Panel(int panel_name) 
{
	panel = panel_name;
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

// Functions that access certain components

SDL_Rect UI_Component_Generic::Return_Rect()
{
	return current_screen_rect;
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

// Functions that check for clicks 

bool UI_Component_Generic::Currently_Clicked()
{
	return currently_clicked;
}

bool UI_Component_Generic::Currently_Clicked(bool state_override)
{
	currently_clicked = state_override;
	return currently_clicked;
}

void UI_Component_Generic::Check_For_Click()
{
	Cursor* cursor = service_locator->get_Cursor_Pointer();
	
	SDL_Point new_point = cursor->Get_Mouse_Position();

	if (SDL_PointInRect(&new_point, &current_screen_rect))
	{
		if (Currently_Clicked() == false && cursor->left_button == true && cursor->left_button_previous == false)
		{
			Currently_Clicked(true);
			cursor->Set_Currently_Clicked_Component(this);
			service_locator->get_UI_pointer()->Load_Action_To_UI(message_length, message_array);
		}
	}
}

void UI_Component_Generic::Change_Component_Title(string new_title)
{
	title = new_title;
}

// Draw Functions

void UI_Component_Generic::Draw(Draw_System* draw_system, SDL_Rect base_rect)
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

void UI_Component_Message_Stream::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(draw_system, base_rect);

	int spacer = 0;
	for (int i = 0; i < max_message_lines; i++)
	{
		SDL_Rect draw_rect = { base_rect.x + offset_rect.x + 5, base_rect.y + offset_rect.y + spacer*20 + 5, offset_rect.w, offset_rect.h };
		Draw_System::Text_Instruction new_text_instruction = { 1,draw_rect, font_type,message_array[i] };
		draw_system->Add_Text_Job_To_Render_Cycle(new_text_instruction);
		spacer++;
	}
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

void UI_Component_Graphic_Button::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(draw_system, base_rect);

	if (template_id !=0 || sprite_clip_override) draw_system->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, { base_rect.x + offset_rect.x , base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h }, sprite_clip);
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

void UI_Component_Graphic_Button::Fetch_Sprite_Details_From_Object_ID()
{
	switch (object_type)
	{
	case OBJECT_TYPE_STRUCTURE:
		{
			Structure_Template new_structure;
			new_structure = service_locator->get_Game_Library()->Fetch_Tile_Object_Config(template_id);
			sprite_clip = { new_structure.icon_clip_x,new_structure.icon_clip_y, SPRITE_SIZE, SPRITE_SIZE };
		}
		break;
	case OBJECT_TYPE_ENTITY:
		break;
	case OBJECT_TYPE_ITEM:
		{
			Item_Template new_item = service_locator->get_Game_Library()->Fetch_Item_Template(template_id);
			sprite_clip = { new_item.sprite_specs.x, new_item.sprite_specs.y, SPRITE_SIZE, SPRITE_SIZE };
		}
		break;
	}
}

void UI_Component_Item_Slot_Button::Draw(SDL_Rect base_rect)
{
	UI_Component_Generic::Draw(service_locator->get_Draw_System_Pointer(), base_rect);

	if (slot_pointer != NULL && slot_pointer->item_quantity > 0)
	{
		sprite_clip = service_locator->get_Game_Library()->Fetch_Item_Template(slot_pointer->slot_item.item_template_id).sprite_specs;
		service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, { base_rect.x + offset_rect.x , base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h }, sprite_clip);

		Set_Font_Type(FONT_SMALL_BOLD);
		string quantity = to_string(slot_pointer->item_quantity);

		int text_width = service_locator->get_Draw_System_Pointer()->Return_Text_Width(font_type, quantity);
		int text_height = service_locator->get_Draw_System_Pointer()->Return_Text_Height(font_type, quantity);
		SDL_Rect text_rect = { base_rect.x + offset_rect.x + SPRITE_SIZE - text_width - 2, base_rect.y + offset_rect.y + SPRITE_SIZE - text_height, text_width, text_height };
		SDL_Rect background_rect = { base_rect.x + offset_rect.x + SPRITE_SIZE - text_width - 5, base_rect.y + offset_rect.y + SPRITE_SIZE - text_height + 1, text_width + 5, text_height -1};
		service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(2, background_rect, true, { 50,50,25,255 });
		service_locator->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(2, background_rect, false, { 255,255,255,255 });
		service_locator->get_Draw_System_Pointer()->Add_Text_Job_To_Render_Cycle({ 2,text_rect,font_type,quantity, text_color });
	}
}

void UI_Component_Item_Slot_Button::Init(Item_Slot* sPointer)
{
	slot_pointer = sPointer;
}