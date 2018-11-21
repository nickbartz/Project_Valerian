#include<Game_Library.h>
#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<istream>
#include<AI_Item_Component.h>

using namespace std;

Game_Library::Game_Library(Global_Service_Locator* sLocator)
{
	service_locator = sLocator;
	
	Load_Tiles_From_Data_File("Data/Tile_Data.csv");
	Load_Entity_Templates("Data/Entity_Templates.csv", "Data/Entity_Animations.csv");
	Load_Item_Templates("Data/Inventory_Data.csv");
	Load_Blueprints("Data/Blueprint_Data.csv");
	Load_Blueprint_Packs("Data/Blueprint_Packs.csv");
	Load_Projectiles("Data/Projectile_Data.csv");
	Load_Job_Code_String_To_Enum_Crosswalk();
	Load_Jobs("Data/Job_Data.csv", "Data/Job_Goal_Data.csv");
}

void Game_Library::Load_Job_Code_String_To_Enum_Crosswalk()
{
	job_codes.insert(pair<string, int>("JOB_NULL", JOB_NULL));
	job_codes.insert(pair<string, int>("JOB_INST_START", JOB_INST_START));
	job_codes.insert(pair<string, int>("JOB_INST_NEW_GOAL", JOB_INST_NEW_GOAL));
	job_codes.insert(pair<string, int>("OB_INST_END_GOAL", JOB_INST_END_GOAL));
	job_codes.insert(pair<string, int>("JOB_INST_END", JOB_INST_END));
	job_codes.insert(pair<string, int>("JOB_TYPE_MINING", JOB_TYPE_MINING));
	job_codes.insert(pair<string, int>("JOB_TYPE_CONTAINER_PICKUP", JOB_TYPE_CONTAINER_PICKUP));
	job_codes.insert(pair<string, int>("ACTION_NONE", ACTION_NONE));
	job_codes.insert(pair<string, int>("ACTION_CREATE", ACTION_CREATE));
	job_codes.insert(pair<string, int>("ACTION_DESTROY", ACTION_DESTROY));
	job_codes.insert(pair<string, int>("ACTION_EDIT_INTERNAL", ACTION_EDIT_INTERNAL));
	job_codes.insert(pair<string, int>("ACTION_EDIT_EXTERNAL", ACTION_EDIT_EXTERNAL));
	job_codes.insert(pair<string, int>("ACTION_ASSESS_INTERNAL", ACTION_ASSESS_INTERNAL));
	job_codes.insert(pair<string, int>("ACTION_ASSESS_EXTERNAL", ACTION_ASSESS_EXTERNAL));
	job_codes.insert(pair<string, int>("ACTION_TRANSFER", ACTION_TRANSFER));
	job_codes.insert(pair<string, int>("ACTION_GOAL_TRAVERSAL", ACTION_GOAL_TRAVERSAL));
	job_codes.insert(pair<string, int>("A_C_CREATE_MINING_JOB", A_C_CREATE_MINING_JOB));
	job_codes.insert(pair<string, int>("A_AE_NULL", A_AE_NULL));
	job_codes.insert(pair<string, int>("A_AE_OXYGENATE", A_AE_OXYGENATE));
	job_codes.insert(pair<string, int>("A_AE_CHECK_SIMPLE_DISTANCE", A_AE_CHECK_SIMPLE_DISTANCE));
	job_codes.insert(pair<string, int>("A_AE_CHECK_OBJECT_STAT", A_AE_CHECK_OBJECT_STAT));
	job_codes.insert(pair<string, int>("A_AE_CHECK_FOR_ENTITIES_IN_RADIUS", A_AE_CHECK_FOR_ENTITIES_IN_RADIUS));
	job_codes.insert(pair<string, int>("A_AE_CHECK_IF_OBJECT_HAS_ITEM", A_AE_CHECK_IF_OBJECT_HAS_ITEM));
	job_codes.insert(pair<string, int>("A_EI_OPEN_DOOR", A_EI_OPEN_DOOR));
	job_codes.insert(pair<string, int>("A_EI_WAIT", A_EI_WAIT));
	job_codes.insert(pair<string, int>("A_EI_OPEN_DOOR", A_EI_OPEN_DOOR));
	job_codes.insert(pair<string, int>("A_EI_SET_WAIT_TIMER", A_EI_SET_WAIT_TIMER));
	job_codes.insert(pair<string, int>("A_EI_CLOSE_DOOR", A_EI_CLOSE_DOOR));
	job_codes.insert(pair<string, int>("A_EI_SET_TARGET_COORD_TO_OBJECT", A_EI_SET_TARGET_COORD_TO_OBJECT));
	job_codes.insert(pair<string, int>("A_EI_PAUSE_FOR_FURTHER_INSTRUCTIONS", A_EI_PAUSE_FOR_FURTHER_INSTRUCTIONS));
	job_codes.insert(pair<string, int>("A_EE_TOGGLE_MINE_OBJECT", A_EE_TOGGLE_MINE_OBJECT));
	job_codes.insert(pair<string, int>("A_EE_FIRE_MINING_LASER_AT_STRUCTURE", A_EE_FIRE_MINING_LASER_AT_STRUCTURE));
	job_codes.insert(pair<string, int>("LOWER_THAN_OR_EQUAL_TO", LOWER_THAN_OR_EQUAL_TO));
}

int Game_Library::Fetch_Structure_Type_ID_From_Name(string structure_type)
{
	for (int i = 0; i < num_loaded_tiles; i++)
	{
		if (structure_type == loaded_tiles[i].structure_type_string)
		{
			return loaded_tiles[i].structure_type;
		}
	}
	
	return 0;
}

int Game_Library::Get_Object_Type_Enum_From_Object_Type_String(string object_type)
{
	if (object_type == "OBJECT_TYPE_STRUCTURE") return OBJECT_TYPE_STRUCTURE;
	else if (object_type == "OBJECT_TYPE_SCAFFOLD") return OBJECT_TYPE_SCAFFOLD;
	else if (object_type == "OBJECT_TYPE_ENTITY") return OBJECT_TYPE_ENTITY;
	else if (object_type == "OBJECT_TYPE_ITEM") return OBJECT_TYPE_ITEM;
	else if (object_type == "OBJECT_TYPE_CONTAINER") return OBJECT_TYPE_CONTAINER;
	else if (object_type == "OBJECT_TYPE_PROJECTILE") return OBJECT_TYPE_PROJECTILE;
	else if (object_type == "OBJECT_TYPE_NULL") return OBJECT_TYPE_NULL;
}

int Game_Library::Get_Job_Code_From_String(string query)
{
	try
	{
		return stoi(query);
	}
	catch(exception& e)
	{
		if (job_codes[query] == 0) cout << "found a job code that's not in the system" << endl;
		return job_codes[query];
	}
}

void Game_Library::Load_Tiles_From_Data_File(string tiles_path)
{
	vector<vector<string>> vector_loaded_tiles = readCSV(tiles_path);

	for (int i = 0; i < vector_loaded_tiles.size()-1; i++)
	{
		Structure_Template new_config;

		new_config.object_type = OBJECT_TYPE_STRUCTURE;
		new_config.tile_layer = stoi(vector_loaded_tiles[i+1][2]);
		
		new_config.structure_type = stoi(vector_loaded_tiles[i + 1][5]);

		new_config.structure_id = stoi(vector_loaded_tiles[i + 1][0]);
		new_config.structure_name = vector_loaded_tiles[i + 1][3];
		new_config.structure_type_string = vector_loaded_tiles[i + 1][4];
		new_config.render_component_type = stoi(vector_loaded_tiles[i + 1][7]);
		new_config.spritesheet = stoi(vector_loaded_tiles[i + 1][11]);
		new_config.tile_specs.x = stoi(vector_loaded_tiles[i + 1][12]);
		new_config.tile_specs.y = stoi(vector_loaded_tiles[i + 1][13]);
		new_config.tile_specs.w = stoi(vector_loaded_tiles[i + 1][14]);
		new_config.tile_specs.h = stoi(vector_loaded_tiles[i + 1][15]);
		new_config.tile_clip.x = stoi(vector_loaded_tiles[i + 1][16])*SPRITE_SIZE;
		new_config.tile_clip.y = stoi(vector_loaded_tiles[i + 1][17])*SPRITE_SIZE;
		new_config.tile_clip.w = stoi(vector_loaded_tiles[i + 1][18])*SPRITE_SIZE;
		new_config.tile_clip.h = stoi(vector_loaded_tiles[i + 1][19])*SPRITE_SIZE;
		new_config.multiclip_type = stoi(vector_loaded_tiles[i + 1][9]);
		new_config.num_animation_frame = stoi(vector_loaded_tiles[i + 1][20]);
		new_config.job_type = stoi(vector_loaded_tiles[i + 1][21]);
		new_config.ui_type = stoi(vector_loaded_tiles[i + 1][23]);
		new_config.icon_clip_x = stoi(vector_loaded_tiles[i + 1][24])*SPRITE_SIZE;
		new_config.icon_clip_y = stoi(vector_loaded_tiles[i + 1][25])*SPRITE_SIZE;
		new_config.is_inaccessible = stoi(vector_loaded_tiles[i + 1][26]);
		new_config.max_built_level = stoi(vector_loaded_tiles[i + 1][27]);
		new_config.has_starter_inventory = stoi(vector_loaded_tiles[i + 1][28]);
		new_config.scaffold_blueprint_id = stoi(vector_loaded_tiles[i + 1][29]);
		new_config.blueprint_pack_id = stoi(vector_loaded_tiles[i + 1][30]);
		loaded_tiles[i] = new_config;
		num_loaded_tiles++;	
	}
}

void Game_Library::Load_Entity_Templates(string entity_template_path, string entity_animation_path)
{
	vector<vector<string>> loaded_templates = readCSV(entity_template_path);

	vector<vector<string>> loaded_animations = readCSV(entity_animation_path);

	for (int i = 1; i < loaded_templates.size(); i++)
	{
		Entity_Template new_entity;
		new_entity.render_component = stoi(loaded_templates[i][5]);
		new_entity.num_entity_components = stoi(loaded_templates[i][7]);
		new_entity.num_entity_animations = stoi(loaded_templates[i][6]);
		new_entity.entity_id = stoi(loaded_templates[i][1]);
		new_entity.entity_has_starter_inventory = stoi(loaded_templates[i][12]);

		for (int q = 0; q < new_entity.num_entity_components; q++)
		{
			int component_number = stoi(loaded_templates[i][8 + q]);
			
			for (int p = 0; p < new_entity.num_entity_animations; p++)
			{
				new_entity.entity_animations[p][q].component_clip.x = Query_Vector_Table(loaded_animations, 2, 0, component_number, 3, p, 4)*SPRITE_SIZE;
				new_entity.entity_animations[p][q].component_clip.y = Query_Vector_Table(loaded_animations, 2, 0, component_number, 3, p, 5)*SPRITE_SIZE;
				new_entity.entity_animations[p][q].component_clip.w = Query_Vector_Table(loaded_animations, 2, 0, component_number, 3, p, 6)*SPRITE_SIZE;
				new_entity.entity_animations[p][q].component_clip.h = Query_Vector_Table(loaded_animations, 2, 0, component_number, 3, p, 7)*SPRITE_SIZE;
				new_entity.entity_animations[p][q].component_max_frames = Query_Vector_Table(loaded_animations, 2, 0, component_number, 3, p, 8);
			}
		}

		loaded_Entities[i-1] = new_entity;
		num_loaded_entities++;
	}
}

void Game_Library::Load_Item_Templates(string item_template_path)
{
	vector<vector<string>> vector_loaded_items = readCSV(item_template_path);

	for (int i = 1; i < vector_loaded_items.size(); i++)
	{
		Item_Template new_item;
		new_item.inventory_item_id = stoi(vector_loaded_items[i][0]);
		new_item.inventory_item_type = vector_loaded_items[i][3];

		new_item.sprite_specs.x = stoi(vector_loaded_items[i][4]) * SPRITE_SIZE;
		new_item.sprite_specs.y = stoi(vector_loaded_items[i][5]) * SPRITE_SIZE;

		loaded_Items[i - 1] = new_item;
		num_loaded_Items++;
	}
}

void Game_Library::Load_Blueprints(string blueprints_path)
{
	vector<vector<string>> vector_loaded_blueprints = readCSV(blueprints_path);

	for (int i = 1; i < vector_loaded_blueprints.size(); i++)
	{
		Blueprint new_blueprint;

		new_blueprint.Blueprint_ID = stoi(vector_loaded_blueprints[i][0]);
		new_blueprint.blueprint_string_name = vector_loaded_blueprints[i][1];
		new_blueprint.associated_template_id = stoi(vector_loaded_blueprints[i][2]);
		new_blueprint.associated_object_type = Get_Object_Type_Enum_From_Object_Type_String(vector_loaded_blueprints[i][3]);
		if (vector_loaded_blueprints[i][4] == "BLUEPRINT_NULL") new_blueprint.associated_blueprint_type = BLUEPRINT_NULL;
		else if (vector_loaded_blueprints[i][4] == "BLUEPRINT_INVENTORY") new_blueprint.associated_blueprint_type = BLUEPRINT_INVENTORY;
		else if (vector_loaded_blueprints[i][4] == "BLUEPRINT_SCAFFOLD") new_blueprint.associated_blueprint_type = BLUEPRINT_SCAFFOLD;
		else if (vector_loaded_blueprints[i][4] == "BLUEPRINT_PRODUCTION") new_blueprint.associated_blueprint_type = BLUEPRINT_PRODUCTION;
		new_blueprint.Num_Items_In_Blueprint = stoi(vector_loaded_blueprints[i][5]);
		for (int p = 0; p < new_blueprint.Num_Items_In_Blueprint; p++)
		{
			Item_Slot new_item_slot;
			new_item_slot.slot_item.item_template_id = stoi(vector_loaded_blueprints[i][6 + 2 * p]);
			new_item_slot.item_quantity = stoi(vector_loaded_blueprints[i][7 + 2 * p]);
			new_blueprint.blueprint_items[p] = new_item_slot;
		}

		loaded_blueprints[i-1] = new_blueprint;
		num_loaded_blueprints++;
	}
}

void Game_Library::Load_Blueprint_Packs(string blueprint_packs_path)
{
	vector<vector<string>> vector_loaded_blueprints = readCSV(blueprint_packs_path);

	for (int i = 1; i < vector_loaded_blueprints.size(); i++)
	{
		Blueprint_Pack new_blueprint_pack;

		new_blueprint_pack.blueprint_pack_id = stoi(vector_loaded_blueprints[i][0]);

		int error = 0;

		for (int p = 3; p < 3 + stoi(vector_loaded_blueprints[i][2]); p++)
		{
			if (stoi(vector_loaded_blueprints[i][p]) != 0)
			{
				new_blueprint_pack.blueprint_id_array.push_back(stoi(vector_loaded_blueprints[i][p]));
			}
			else break;
		}

		loaded_blueprint_packs[num_loaded_blueprint_packs] = new_blueprint_pack;
		num_loaded_blueprint_packs++;
	}
}

void Game_Library::Load_Jobs(string job_template_path, string goal_template_path)
{
	vector<vector<string>> vector_loaded_job_templates = readCSV(job_template_path);

	for (int i = 1; i < vector_loaded_job_templates.size(); i++)
	{
		Job new_job_template = Job(service_locator, stoi(vector_loaded_job_templates[i][0]), vector_loaded_job_templates[i][2]);
		new_job_template.Set_Job_Priority(stoi(vector_loaded_job_templates[i][3]));
		loaded_jobs[i - 1] = new_job_template;
		num_loaded_jobs++;
	}
}

void Game_Library::Load_Projectiles(string projectile_template_path)
{
	vector<vector<string>> vector_loaded_templates = readCSV(projectile_template_path);

	for (int i = 1; i < vector_loaded_templates.size(); i++)
	{
		Projectile_Template new_projectile_template;
		new_projectile_template.projectile_template_id = stoi(vector_loaded_templates[i][0]);
		new_projectile_template.projectile_string = vector_loaded_templates[i][2];
		new_projectile_template.sprite_clip.x = stoi(vector_loaded_templates[i][3])*SPRITE_SIZE;
		new_projectile_template.sprite_clip.y = stoi(vector_loaded_templates[i][4])*SPRITE_SIZE;
		new_projectile_template.sprite_clip.w = stoi(vector_loaded_templates[i][5])*SPRITE_SIZE;
		new_projectile_template.sprite_clip.h = stoi(vector_loaded_templates[i][6])*SPRITE_SIZE;
		new_projectile_template.projectile_power = stoi(vector_loaded_templates[i][7]);
		new_projectile_template.projectile_speed = stoi(vector_loaded_templates[i][8]);
		new_projectile_template.num_animation_frames = stoi(vector_loaded_templates[i][9]);
		new_projectile_template.projectile_range = stoi(vector_loaded_templates[i][10]);
		new_projectile_template.projectile_splash = stoi(vector_loaded_templates[i][11]);
		new_projectile_template.animation_delay = stoi(vector_loaded_templates[i][12]);
		new_projectile_template.projectile_color.r  = stoi(vector_loaded_templates[i][13]);
		new_projectile_template.projectile_color.g = stoi(vector_loaded_templates[i][14]);
		new_projectile_template.projectile_color.b = stoi(vector_loaded_templates[i][15]);
		new_projectile_template.projectile_color.a = stoi(vector_loaded_templates[i][16]);
		new_projectile_template.is_point_not_sprite = stoi(vector_loaded_templates[i][17]);
		new_projectile_template.render = stoi(vector_loaded_templates[i][18]);

		loaded_projectiles[i - 1] = new_projectile_template;
		num_loaded_projectiles++;
	}
}

vector<vector<int>> Game_Library::Create_Room_From_Data_File(int x_tile_start, int y_tile_start, string filename)
{
	vector<vector<string>> room_tiles = readCSV(filename);
	vector<vector<int>> room_tiles_int;

	for (int p = 0; p < room_tiles.size(); p++)
	{
		vector<int> new_vec;
		
		for (int i = 0; i < room_tiles[p].size(); i++)
		{
			new_vec.push_back(stoi(room_tiles[p][i]));
		}

		room_tiles_int.push_back(new_vec);
	}
	return room_tiles_int;
}

vector<string> Game_Library::readCSVRow(const string &row) {
	CSVState state = CSVState::UnquotedField;
	std::vector<std::string> fields{ "" };
	size_t i = 0; // index of the current field
	for (char c : row) {
		switch (state) {
		case CSVState::UnquotedField:
			switch (c) {
			case ',': // end of field
				fields.push_back(""); i++;
				break;
			case '"': state = CSVState::QuotedField;
				break;
			default:  fields[i].push_back(c);
				break;
			}
			break;
		case CSVState::QuotedField:
			switch (c) {
			case '"': state = CSVState::QuotedQuote;
				break;
			default:  fields[i].push_back(c);
				break;
			}
			break;
		case CSVState::QuotedQuote:
			switch (c) {
			case ',': // , after closing quote
				fields.push_back(""); i++;
				state = CSVState::UnquotedField;
				break;
			case '"': // "" -> "
				fields[i].push_back('"');
				state = CSVState::QuotedField;
				break;
			default:  // end of quote
				state = CSVState::UnquotedField;
				break;
			}
			break;
		}
	}
	return fields;
}

vector<vector<string>> Game_Library::readCSV(string path) {

	filebuf fb;
	vector<vector<string>> table;

	if (fb.open(path, ios::in))
	{
		istream in(&fb);


		string row;

		while (!in.eof()) {
			getline(in, row);
			if (in.bad() || in.fail()) {
				break;
			}
			auto fields = readCSVRow(row);
			table.push_back(fields);
		}

		fb.close();
	}

	return table;
}

int Game_Library::Query_Vector_Table( vector<vector<string>> table, int num_columns_to_crossreference, int column_1, int value_1, int column_2, int value_2, int value_column)
{
	for (int i = 1; i < table.size(); i++)
	{
		switch (num_columns_to_crossreference)
		{
		case 1:
			if (stoi(table[i][column_1]) == value_1) return stoi(table[i][value_column]);
			break;
		case 2:
			if (stoi(table[i][column_1]) == value_1 && stoi(table[i][column_2]) == value_2) return stoi(table[i][value_column]);
			break;
		}
	}
}

Structure_Template* Game_Library::Fetch_Tile_Object_Config(int tile_id)
{
	if (tile_id < num_loaded_tiles)
	{
		return &loaded_tiles[tile_id];
	}

	else
	{
		cout << "tile id out of range, returning null tile" << endl;
		return &loaded_tiles[0];
	}
}

Entity_Template* Game_Library::Fetch_Entity_Template(int entity_id)
{
	if (entity_id < num_loaded_entities)
	{
		return &loaded_Entities[entity_id];
	}
	else
	{
		cout << "entity id out of range, returning null entity" << endl;
		return &loaded_Entities[0];
	}
}

Item_Template* Game_Library::Fetch_Item_Template(int item_id)
{
	
	if (item_id < num_loaded_Items)
	{
		return &loaded_Items[item_id];
	}
	else
	{
		cout << "item id out of range, returning null item" << endl;
		return &loaded_Items[0];
	}
}

Blueprint* Game_Library::Fetch_Blueprint(int blueprint_id)
{
	if (blueprint_id < num_loaded_blueprints && blueprint_id != 0) return &loaded_blueprints[blueprint_id];
	else return NULL;
}

Blueprint_Pack* Game_Library::Fetch_Blueprint_Pack(int blueprint_pack)
{
	return &loaded_blueprint_packs[blueprint_pack];
}

vector<Blueprint*> Game_Library::Fetch_All_Blueprints_Of_Type_For_Object(int type, int object_type, int object_template_id)
{
	vector<Blueprint*> requested_blueprints;
	
	for (int i = 0; i < num_loaded_blueprints; i++)
	{
		if (loaded_blueprints[i].associated_blueprint_type == type && loaded_blueprints[i].associated_object_type == object_type && loaded_blueprints[i].associated_template_id == object_template_id)
		{
			requested_blueprints.push_back(&loaded_blueprints[i]);	
		}
	}
	
	return requested_blueprints;
}

Job* Game_Library::Fetch_Job_Template(int job_id)
{
	return &loaded_jobs[job_id];
}

Projectile_Template* Game_Library::Fetch_Projectile_Template(int projectile_id)
{
	return &loaded_projectiles[projectile_id];
}

int Game_Library::Get_Num_Structure_Template()
{
	return num_loaded_tiles;
}

bool Game_Library::is_wall(int tile_type)
{
	if (tile_type == 20 || tile_type == 8) return true;
	else return false;
}
bool Game_Library::is_null(int tile_type)
{
	if (tile_type == STRUCTURE_TYPE_NULL) return true;
	else return false;
}
bool Game_Library::is_floor(int tile_type)
{
	if (tile_type == 10) return true;
	else return false;
}
bool Game_Library::is_door(int tile_type)
{
	if (tile_type == 8) return true;
	else return false;
}