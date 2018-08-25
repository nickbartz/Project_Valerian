#pragma once

#include<Object.h>

class Tile
{
public:
	Tile(int tile_x, int tile_y);
private:
	Object * base_structure = NULL;
	Object* mid_structure = NULL;
	Object* roof_structure = NULL;

	int grid_x = 0;
	int grid_y = 0;
};