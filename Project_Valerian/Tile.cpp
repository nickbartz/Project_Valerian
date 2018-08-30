#include<Tile.h>

Tile::Tile(Coordinate gPoint)
{
	grid_point = gPoint;
}

bool Tile::Update_Tile_Structure(Object* new_structure, int tile_layer)
{
	bool replacement = false;
	
	switch (tile_layer)
	{
	case TILE_LAYER_BASE:
		if (base_structure != NULL && base_structure->Get_Assigned_Flag() == OBJECT_ASSIGNED)
		{
			base_structure->Set_Assigned_Flag(OBJECT_UNASSIGNED);
			replacement = true;
		}
		base_structure = new_structure;
		break;
	case TILE_LAYER_MID:
		if (mid_structure != NULL && mid_structure->Get_Assigned_Flag() == OBJECT_ASSIGNED)
		{
			mid_structure->Set_Assigned_Flag(OBJECT_UNASSIGNED);
			replacement = true;
		}
		mid_structure = new_structure;
		break;
	case TILE_LAYER_ROOF:
		if (roof_structure != NULL && roof_structure->Get_Assigned_Flag() == OBJECT_ASSIGNED)
		{
			replacement = true;
			roof_structure->Set_Assigned_Flag(OBJECT_UNASSIGNED);
		}
		roof_structure = new_structure;
		break;
	}

	return replacement;
}
