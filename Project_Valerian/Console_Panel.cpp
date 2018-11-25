#include<Console_Panel.h>
#include<Service_Locator.h>
#include<Game_Library.h>
#include<Object.h>
#include<AI_Item_Component.h>
#include<AI_Job_Component.h>
#include<AI_Stats_Component.h>
#include<Cursor.h>
#include<UI.h>
#include<Scene_Graph.h>

UI_Panel_Generic::UI_Panel_Generic(Global_Service_Locator* sLocator, int wName, SDL_Rect rect)
{
	service_locator = sLocator;
	window = wName;
	offset_rect = rect;
}
void UI_Panel_Generic::Change_Rect(SDL_Rect new_rect)
{
	offset_rect = new_rect;
}
bool UI_Panel_Generic::Check_If_Click_In_Panel_Rect(SDL_Rect base_rect, SDL_Point mouse_location)
{
	SDL_Rect panel_location = { base_rect.x + offset_rect.x, base_rect.y + offset_rect.y, offset_rect.w, offset_rect.h };

	if (SDL_PointInRect(&mouse_location, &panel_location)) return true;
	else return false;
}
void UI_Panel_Generic::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{

}
int UI_Panel_Generic::Get_Panel_Name()
{
	return panel_name;
}
void UI_Panel_Generic::Init()
{

}

void UI_Panel_Console::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(base_rect);
	message_stream.Draw(base_rect);
}
void UI_Panel_Console::Push_Message_Into_Stream(string new_message)
{
	message_stream.Push_Message_Into_Stream(new_message);
}

void UI_Panel_Object_Inventory::Handle_Mouse_Click()
{
	if (item_array.Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position()))
	{
		Item_Slot* clicked_item_slot = item_array.Handle_Click_On_Component();
		inventory_details.Init(linked_object, OBJECT_TYPE_ITEM, clicked_item_slot->slot_item.item_template_id);
	}
}
void UI_Panel_Object_Inventory::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(base_rect);
	item_array.Draw(base_rect);
	equipment_array.Draw(base_rect);
	inventory_details.Draw(base_rect);
}
void UI_Panel_Object_Inventory::Init(Object* object)
{
	background_component = UI_Component_Generic(service_locator, offset_rect, true);
	linked_object = object;

	AI_Item_Component* object_item_component = linked_object->Return_AI_Item_Component();

	if (object_item_component != NULL) item_array.Init(object_item_component->Return_Entire_Inventory_As_Pointer(), object_item_component->Return_Num_Inventory_Slots());
}

void UI_Panel_Object_Jobs::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(base_rect);

	if (linked_job_component->Return_Current_Job_Pointer() != NULL)
	{
		object_job.Update_Component_Title(linked_job_component->Return_Current_Job_Pointer()->Return_Job_String_Name(), { 255,255,255,255 });
		goal_list.Clear_All_Messages_From_Stream();

		if (linked_job_component->Return_Current_Num_Goals() > 0)
		{
			for (int i = linked_job_component->Return_Current_Num_Goals() -1; i >= 0; i--)
			{
				goal_list.Push_Message_Into_Stream(linked_job_component->Return_Goal_At_Array_Num(i)->goal_string_name);
			}
		}

		goal_list.Set_Bold_Line(linked_job_component->Return_Current_Goal_Index());
		goal_list.Draw(base_rect);
	}
	else
	{
		object_job.Update_Component_Title("No Current Job", { 255,255,255,255 });
	}

	object_job.Draw(base_rect);

}
void UI_Panel_Object_Jobs::Handle_Mouse_Click()
{
	reset = true;
}
void UI_Panel_Object_Jobs::Init(Object* object)
{
	linked_object = object;

	background_component = UI_Component_Generic(service_locator, offset_rect, true);
	SDL_Rect object_job_rect = { offset_rect.x, offset_rect.y, offset_rect.w, 25 };

	object_job = UI_Component_Generic(service_locator, object_job_rect);
	linked_job_component = object->Return_AI_Job_Component();

	object_job.Update_Component_Title("No Current_Job", { 255,255,255,255 });

	SDL_Rect message_stream_rect = { offset_rect.x, offset_rect.y + 25, offset_rect.w, offset_rect.h - 25 };
	goal_list = UI_Component_Message_Stream(service_locator, message_stream_rect);
}

void UI_Panel_Object_Production::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(base_rect);
	build_button.Draw(base_rect);
	blueprint_list.Draw(base_rect);
	blueprint_details.Draw(base_rect);
	production_amount.Draw(base_rect);
}
void UI_Panel_Object_Production::Init(Object* object)
{
	// These are percentages so if the overarching window changes, then the figures will change automatically

	linked_object = object;

	blueprint_list.Init();
	blueprint_details.Init(object);
	production_amount.Init(true);
	production_amount.Set_Font_Type(FONT_LARGE_BOLD);

	build_button.Update_Component_Title("Build");
	build_button.Set_Font_Type(FONT_LARGE_BOLD);

	UI_Component_Generic new_button = UI_Component_Generic(service_locator, { 0,0,200,0 }, true);

	AI_Item_Component* object_inventory = object->Return_AI_Item_Component();
	int num_production_blueprints = object_inventory->Return_Num_Production_Blueprints();

	for (int i = 0; i < num_production_blueprints; i++)
	{
		Blueprint* production_blueprint = object_inventory->Return_Blueprint_At_Slot(i);
		string name = production_blueprint->blueprint_string_name;
		new_button.Set_Background_Color({ 150,150,150,50 });
		new_button.Update_Component_Title(name, { 255,255,255,255 });
		int message[2] = { UI_ACTION_SUPPORTING_TYPE_SET_BLUEPRINT_ID, production_blueprint->Blueprint_ID };
		new_button.Set_Click_Message(2, message);
		blueprint_list.Add_Button_To_Button_List(new_button);
	}

}
void UI_Panel_Object_Production::Set_Current_Blueprint_ID(int blueprint_id)
{
	current_blueprint_id = blueprint_id;
}
void UI_Panel_Object_Production::Handle_Mouse_Click()
{
	// If the click is in the blueprint box
	if (blueprint_list.Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position()))
	{
		UI_Component_Generic* clicked_component = blueprint_list.Handle_Click_On_Component();
		if (clicked_component != NULL && clicked_component->Get_Message_Length() == 2)
		{
			vector<int> message = clicked_component->Get_Message_In_Vector();
			if (message[0] == UI_ACTION_SUPPORTING_TYPE_SET_BLUEPRINT_ID)
			{
				current_blueprint_id = message[1];
				int associated_template_id = service_locator->get_Game_Library()->Fetch_Blueprint(current_blueprint_id)->associated_template_id;
				blueprint_details.Init(linked_object, OBJECT_TYPE_ITEM, associated_template_id);
			}
		}
	}
	// If the click is in the production amount box 
	else if (production_amount.Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position()))
	{
		production_amount.Handle_Click_On_Component();
	}
	
	// If the click is in the build button
	else if (build_button.Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position()))
	{
		int build_number = production_amount.Get_Current_Num();
		if (build_number > 0 && current_blueprint_id != 0)
		{
			Blueprint* requested_blueprint = service_locator->get_Game_Library()->Fetch_Blueprint(current_blueprint_id);

			// Add the blueprint request to the item's blueprint request queue //
			linked_object->Return_AI_Item_Component()->Request_Production_Of_Item_From_Blueprint(requested_blueprint, build_number);
			
			// Send a message to the console telling the player that this happened
			service_locator->get_UI_pointer()->Push_Message_To_Console(to_string(build_number) + " Blueprints added for: " + linked_object->Return_Stats_Component()->Get_Structure_Common_Name());
			build_button.Currently_Clicked(true);
		}
	}
}

void UI_Panel_Object_Stats::Handle_Mouse_Click()
{

}
void UI_Panel_Object_Stats::Init(Object* object)
{
	base_static_stats.Init(object);
	base_static_stats.Add_Stat_Button(STAT_CURRENT_X_TILE,"Curent X Tile");
	base_static_stats.Add_Stat_Button(STAT_CURRENT_Y_TILE, "Curent Y Tile");
	base_static_stats.Add_Stat_Button(STAT_ENTITY_SPEED, "Entity speed");
	base_static_stats.Add_Stat_Button(STAT_OBJECT_FACTION, "Object Faction");
	base_static_stats.Add_Stat_Button(STAT_OBJECT_HEALTH, "Object Health");
	base_static_stats.Add_Stat_Button(STAT_STRUCTURE_IMPASSABLE, "Impassable");
	base_static_stats.Add_Stat_Button(STAT_STRUCTURE_OXYGEN_LEVEL, "Oxygen Level");
	base_static_stats.Add_Stat_Button(STAT_STRUCTURE_BUILT_LEVEL, "Built Level");

	dynamic_stats.Init(object);
	dynamic_stats.Add_Stat_Button(STAT_ENTITY_HUNGER, "Hunger", true);
	dynamic_stats.Add_Stat_Button(STAT_ENTITY_OXYGEN, "Oxygen Deprivation", true);
	dynamic_stats.Add_Stat_Button(STAT_ENTITY_TIREDNESS, "Exhaustion", true);
	dynamic_stats.Add_Stat_Button(STAT_ENTITY_FEAR, "Fear", true);
	dynamic_stats.Add_Stat_Button(STAT_ENTITY_ENNUI, "Boredom", true);
}
void UI_Panel_Object_Stats::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(base_rect);
	base_static_stats.Draw(base_rect);
	dynamic_stats.Draw(base_rect);
}

void UI_Panel_Structure_Create_Type::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	background_component.Draw(base_rect);

	for (int i = 0; i < panel_buttons; i++)
	{
		graphic_button_array[i].Draw(base_rect);
	}
}
void UI_Panel_Structure_Create_Type::Handle_Mouse_Click()
{
	for (int i = 0; i < panel_buttons; i++)
	{
		if (graphic_button_array[i].Check_If_Click_In_Component_Rect(service_locator->get_Cursor_Pointer()->Get_Mouse_Position())) graphic_button_array[i].Handle_Button_Click();
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
		if (service_locator->get_Game_Library()->Fetch_Structure_Template(i)->ui_type == ui_structure_type)
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
