#include<Console_Panel.h>
#include<Service_Locator.h>
#include<Game_Library.h>
#include<Object.h>
#include<AI_Item_Component.h>
#include<AI_Job_Component.h>

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
	background_component.Draw(draw_system, base_rect);
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
		if (service_locator->get_Game_Library()->Fetch_Tile_Object_Config(i)->ui_type == ui_structure_type)
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
		graphic_button_array[i].Draw(base_rect);
	}
}

void UI_Panel_Object_Inventory::Init(Object* object)
{
	background_component = UI_Component_Generic(service_locator, offset_rect, true);

	AI_Item_Component* object_item_component = object->Return_AI_Item_Component();

	if (object_item_component != NULL)
	{		
		int current_row = 0;
		int current_column = 0;

		int button_width = offset_rect.w / panel_columns;
		int button_height = offset_rect.h / panel_rows;

		for (int i = 0; i < object_item_component->Return_Num_Inventory_Slots(); i++)
		{
			graphic_button_array.push_back(UI_Component_Item_Slot_Button(service_locator, { offset_rect.x + current_column * SPRITE_SIZE, offset_rect.y + current_row * SPRITE_SIZE, SPRITE_SIZE ,SPRITE_SIZE }));
			graphic_button_array.back().Init(object_item_component->Return_Inventory_Slot_As_Pointer(i));
			
			//int message_length = 3;
			//int message_array[3] = { UI_ACTION_CATEGORY_SUPPORTING,UI_ACTION_SUPPORTING_TYPE_SET_STRUCTURE_BUILD,i };
			//graphic_button_array.back().Set_Click_Message(message_length, message_array);
			panel_buttons++;
			current_column++;
			if (current_column >= panel_columns) current_row++, current_column = 0;
		}
	}
	else
	{
		cout << "null item component" << endl;
	}
}

void UI_Panel_Object_Inventory::Check_For_Clicks()
{

}

void UI_Panel_Object_Stats::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(draw_system, base_rect);

	for (int i = 0; i < num_stats; i++)
	{
		graphic_button_array[i].Draw(draw_system, base_rect);
	}
}

void UI_Panel_Object_Stats::Init(Object* object)
{
	background_component = UI_Component_Generic(service_locator, offset_rect, true);

	graphic_button_array.push_back(UI_Component_Stat_Button(service_locator, { offset_rect.x, offset_rect.y, offset_rect.w ,25 }));
	graphic_button_array[0].Init(object, STAT_OBJECT_HEALTH, "Health");
	num_stats++;
}

void UI_Panel_Object_Stats::Check_For_Clicks()
{

}

void UI_Panel_Object_Jobs::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(draw_system, base_rect);


	if (linked_job_component->Return_Current_Job_Pointer() != NULL)
	{
		if (reset == true)
		{
			object_job.Change_Component_Title(linked_job_component->Return_Current_Job_Pointer()->Return_Job_String_Name(), { 5,3,25,25 }, { 255,255,255,255 });
			goal_list.Clear_All_Messages_From_Stream();

			cout << linked_job_component->Return_Current_Num_Goals() << endl;
			if (linked_job_component->Return_Current_Num_Goals() > 0)
			{

				for (int i = 0; i < linked_job_component->Return_Current_Num_Goals(); i++)
				{
					goal_list.Set_Message_At_Array_Num(linked_job_component->Return_Goal_At_Array_Num(i)->goal_string_name, i);
				}
				goal_list.Set_Bold_Line(linked_job_component->Return_Current_Goal_Index());
			}

			reset = false;
		}

		goal_list.Set_Bold_Line(linked_job_component->Return_Current_Goal_Index());
		goal_list.Draw(draw_system, base_rect);
	}
	else
	{
		if (reset == false)
		{
			object_job.Change_Component_Title("No Current Job", { 5,3,25,25 }, { 255,255,255,255 });
			reset = true;
		}
	}

	object_job.Draw(draw_system, base_rect);

}

void UI_Panel_Object_Jobs::Init(Object* object)
{
	linked_object = object;

	background_component = UI_Component_Generic(service_locator, offset_rect, true);
	SDL_Rect object_job_rect = { offset_rect.x, offset_rect.y, offset_rect.w, 25 };

	object_job = UI_Component_Generic(service_locator, object_job_rect);
	linked_job_component = object->Return_AI_Job_Component();
	
	object_job.Change_Component_Title("No Current_Job", { 5,4,25,25 }, { 255,255,255,255 });

	SDL_Rect message_stream_rect = { offset_rect.x, offset_rect.y + 25, offset_rect.w, offset_rect.h - 25 };
	goal_list = UI_Component_Message_Stream(service_locator, message_stream_rect,15);
}

void UI_Panel_Object_Jobs::Check_For_Clicks()
{

}