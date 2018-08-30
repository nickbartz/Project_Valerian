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

	for (int i = 1; i < loaded_csv.size(); i++)
	{
		Object_Config new_config;

		new_config.tile_layer = loaded_csv[i][2][0] - '0';
		
		new_config.structure_type = loaded_csv[i][6][0] - '0';
		new_config.structure_id = loaded_csv[i][0][0] - '0';
		new_config.render_component_type = loaded_csv[i][7][0] - '0';
		new_config.spritesheet = loaded_csv[i][11][0] - '0';

		new_config.tile_specs.x = loaded_csv[i][12][0] - '0';
		new_config.tile_specs.y = loaded_csv[i][13][0] - '0';
		new_config.tile_specs.w = loaded_csv[i][14][0] - '0';
		new_config.tile_specs.h = loaded_csv[i][15][0] - '0';

		new_config.tile_clip.x = loaded_csv[i][16][0] - '0';
		new_config.tile_clip.y = loaded_csv[i][17][0] - '0';
		new_config.tile_clip.w = loaded_csv[i][18][0] - '0';
		new_config.tile_clip.h = loaded_csv[i][19][0] - '0';
		new_config.multiclip_type = loaded_csv[i][9][0] - '0';
		new_config.num_animation_frame = loaded_csv[i][20][0] - '0';
		
		loaded_tiles[i - 1] = new_config;
	}

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