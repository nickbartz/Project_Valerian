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
}

void Game_Library::Load_Tiles_From_Data_File(string path)
{
	vector<vector<string>> loaded_csv = readCSV(path);

	for (int i = 0; i < loaded_csv.size()-1; i++)
	{
		Object_Config new_config;

		new_config.object_type = OBJECT_TYPE_STRUCTURE;
		new_config.tile_layer = stoi(loaded_csv[i+1][2]);
		
		new_config.structure_type = stoi(loaded_csv[i + 1][5]);

		new_config.structure_id = stoi(loaded_csv[i + 1][0]);
		new_config.render_component_type = stoi(loaded_csv[i + 1][7]);
		new_config.spritesheet = stoi(loaded_csv[i + 1][11]);
		new_config.tile_specs.x = stoi(loaded_csv[i + 1][12]);
		new_config.tile_specs.y = stoi(loaded_csv[i + 1][13]);
		new_config.tile_specs.w = stoi(loaded_csv[i + 1][14]);
		new_config.tile_specs.h = stoi(loaded_csv[i + 1][15]);

		new_config.tile_clip.x = stoi(loaded_csv[i + 1][16]);
		new_config.tile_clip.y = stoi(loaded_csv[i + 1][17]);
		new_config.tile_clip.w = stoi(loaded_csv[i + 1][18]);
		new_config.tile_clip.h = stoi(loaded_csv[i + 1][19]);
		new_config.multiclip_type = stoi(loaded_csv[i + 1][9]);
		new_config.num_animation_frame = stoi(loaded_csv[i + 1][20]);
		
		loaded_tiles[i] = new_config;
		num_loaded_tiles++;
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

Object_Config Game_Library::Fetch_Tile_Object_Config(int tile_id)
{
	if (tile_id < num_loaded_tiles)
	{
		return loaded_tiles[tile_id];
	}

	else
	{
		cout << "tile id out of range, returning null tile" << endl;
		return loaded_tiles[0];
	}
}

bool Game_Library::is_wall(int tile_type)
{
	if (tile_type == 20) return true;
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