#include<Game_Library.h>
#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<istream>


using namespace std;

Game_Library::Game_Library()
{
	Load_Tiles_From_Data_File("Data/Tile_Data.csv");
	Load_Entity_Templates("Data/Entity_Templates.csv", "Data/Entity_Animations.csv");
	Load_Item_Templates("Data/Inventory_Data.csv");
	Load_Blueprints("Data/Blueprint_Data.csv");
	Load_Projectiles("Data/Projectile_Data.csv");
	Load_Jobs("Data/Job_Data.csv");
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
		new_config.inventory_pack = stoi(vector_loaded_tiles[i + 1][27]);
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
		new_entity.entity_inventory_pack = stoi(loaded_templates[i][12]);

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
		new_item.inventory_item_type = stoi(vector_loaded_items[i][2]);

		new_item.sprite_specs.x = stoi(vector_loaded_items[i][4]) * SPRITE_SIZE;
		new_item.sprite_specs.y = stoi(vector_loaded_items[i][5]) * SPRITE_SIZE;

		loaded_Items[i - 1] = new_item;
		num_loaded_Items++;
	}
}

void Game_Library::Load_Blueprints(string blueprint_path)
{
	vector<vector<string>> vector_loaded_blueprints = readCSV(blueprint_path);

	for (int i = 1; i < vector_loaded_blueprints.size(); i++)
	{
		Inventory_Template new_blueprint;
		new_blueprint.inventory_pack[0][0] = stoi(vector_loaded_blueprints[i][2]);
		new_blueprint.inventory_pack[0][1] = stoi(vector_loaded_blueprints[i][3]);
		new_blueprint.inventory_pack[1][0] = stoi(vector_loaded_blueprints[i][4]);
		new_blueprint.inventory_pack[1][1] = stoi(vector_loaded_blueprints[i][5]);
		new_blueprint.inventory_pack[2][0] = stoi(vector_loaded_blueprints[i][6]);
		new_blueprint.inventory_pack[2][1] = stoi(vector_loaded_blueprints[i][7]);
		new_blueprint.inventory_pack[3][0] = stoi(vector_loaded_blueprints[i][8]);
		new_blueprint.inventory_pack[3][1] = stoi(vector_loaded_blueprints[i][9]);
		new_blueprint.inventory_pack[4][0] = stoi(vector_loaded_blueprints[i][10]);
		new_blueprint.inventory_pack[4][1] = stoi(vector_loaded_blueprints[i][11]);

		loaded_blueprints[i - 1] = new_blueprint;
		num_loaded_blueprints++;
	}

}

void Game_Library::Load_Jobs(string job_template_path)
{
	vector<vector<string>> vector_loaded_job_templates = readCSV(job_template_path);

	for (int i = 1; i < vector_loaded_job_templates.size(); i++)
	{
		Job_Template new_job_template;
		new_job_template.job_id = stoi(vector_loaded_job_templates[i][0]);
		new_job_template.job_string_name = vector_loaded_job_templates[i][2];
		new_job_template.job_standard_priority = stoi(vector_loaded_job_templates[i][3]);

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

Inventory_Template* Game_Library::Fetch_Blueprint(int blueprint_id)
{
	if (blueprint_id < num_loaded_blueprints)
	{
		return &loaded_blueprints[blueprint_id];
	}
	else
	{
		cout << "blueprint id out of range, returning null blueprint" << endl;
		return &loaded_blueprints[0];
	}
}

Job_Template* Game_Library::Fetch_Job_Template(int job_id)
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