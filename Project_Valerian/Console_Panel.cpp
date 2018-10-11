#include<Console_Panel.h>
#include<Service_Locator.h>
#include<Game_Library.h>
#include<Object.h>
#include<AI_Item_Component.h>

UI_Panel_Generic::UI_Panel_Generic(Global_Service_Locator* sLocator, int wName, SDL_Rect rect)
{
	service_locator = sLocator;
	window = wName;
	offset_rect = rect;
}

void UI_Panel_Generic::Init()
{

}

int UI_Panel_Generic::Get_Panel_Name()
{
	return panel_name;
}

void UI_Panel_Generic::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{

}

void UI_Panel_Generic::Change_Rect(SDL_Rect new_rect)
{
	offset_rect = new_rect;
}

void UI_Panel_Console::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	message_stream.Draw(draw_system, base_rect);
}

void UI_Panel_Console::Push_Message_Into_Stream(string new_message)
{
	message_stream.Push_Message_Into_Stream(new_message);
}

void UI_Panel_Structure_Create_Type::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(draw_system, base_rect);

	for (int i = 0; i < panel_buttons; i++)
	{
		graphic_button_array[i].Draw(draw_system, base_rect);
	}
}

void UI_Panel_Structure_Create_Type::Init(int ui_structure_type)
{
	background_component = UI_Component_Generic(service_locator, offset_rect, true);

	int num_structure_templates = service_locator->get_Game_Library()->Get_Num_Structure_Template();
	int current_row = 0;
	int current_column = 0;

	int button_width = offset_rect.w / panel_columns;
	int button_height = offset_rect.h / panel_rows;

	for (int i = 0; i < num_structure_templates; i++)
	{
		if (service_locator->get_Game_Library()->Fetch_Tile_Object_Config(i).ui_type == ui_structure_type)
		{
			graphic_button_array.push_back(UI_Component_Graphic_Button(service_locator, { offset_rect.x + current_column * SPRITE_SIZE, offset_rect.y + current_row * SPRITE_SIZE, SPRITE_SIZE ,SPRITE_SIZE }));
			graphic_button_array.back().Init(OBJECT_TYPE_STRUCTURE, i);
			int message_length = 3;
			int message_array[3] = { UI_ACTION_CATEGORY_SUPPORTING,UI_ACTION_SUPPORTING_TYPE_SET_STRUCTURE_BUILD,i };
			graphic_button_array.back().Set_Click_Message(message_length, message_array);
			panel_buttons++;
			current_column++;
			if (current_column >= panel_columns) current_row++, current_column = 0;
		}
	}

}

void UI_Panel_Structure_Create_Type::Check_For_Clicks()
{
	for (int i = 0; i < panel_buttons; i++)
	{
		graphic_button_array[i].Check_For_Click();
	}
}

void UI_Panel_Object_Inventory::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(draw_system, base_rect);

	for (int i = 0; i < panel_buttons; i++)
	{
		graphic_button_array[i].Draw(draw_system, base_rect);
	}
}

void UI_Panel_Object_Inventory::Init(Object* object)
{
	background_component = UI_Component_Generic(service_locator, offset_rect, true);

	AI_Item_Component* object_item_component = (AI_Item_Component*)object->Return_Object_Component_Pointer(OBJECT_COMPONENT_ITEM);

	if (object_item_component != NULL)
	{		
		int current_row = 0;
		int current_column = 0;

		int button_width = offset_rect.w / panel_columns;
		int button_height = offset_rect.h / panel_rows;

		for (int i = 0; i < object_item_component->Return_Num_Inventory_Slots(); i++)
		{
			graphic_button_array.push_back(UI_Component_Graphic_Button(service_locator, { offset_rect.x + current_column * SPRITE_SIZE, offset_rect.y + current_row * SPRITE_SIZE, SPRITE_SIZE ,SPRITE_SIZE }));
			graphic_button_array.back().Init(OBJECT_TYPE_ITEM, object_item_component->Return_Item_In_Slot(i).slot_item.item_template_id);
			
			//int message_length = 3;
			//int message_array[3] = { UI_ACTION_CATEGORY_SUPPORTING,UI_ACTION_SUPPORTING_TYPE_SET_STRUCTURE_BUILD,i };
			//graphic_button_array.back().Set_Click_Message(message_length, message_array);
			panel_buttons++;
			current_column++;
			if (current_column >= panel_columns) current_row++, current_column = 0;
		}
	}
}

void UI_Panel_Object_Inventory::Check_For_Clicks()
{

}