#include <Cursor.h>
#include<Draw_System.h>
#include<algorithm>
#include<Service_Locator.h>
#include<Console_Component.h>
#include<Coordinate.h>

Cursor::Cursor(Global_Service_Locator* sPointer)
{
	service_pointer = sPointer;
	camera = { 0, 0,32, 0 };
}

SDL_Rect Cursor::Get_Camera()
{
	return camera;
}

SDL_Point Cursor::Get_Mouse_Position()
{
	return SDL_Point{ current_mouse_x, current_mouse_y };
}

SDL_Point Cursor::Get_Mouse_World_Pos()
{
	int mouse_grid_x = 0;
	int mouse_grid_y = 0;

	int mouse_pos_x = (current_mouse_x - (SCREEN_WIDTH / 2) - camera.x);
	int mouse_pos_y = (current_mouse_y - (SCREEN_HEIGHT / 2) - camera.y);

	if (mouse_pos_x >= 0)
	{
		mouse_grid_x = mouse_pos_x / camera.w * TILE_SIZE;
	}
	else mouse_grid_x = mouse_pos_x / camera.w * TILE_SIZE;

	if (mouse_pos_y >= 0)
	{
		mouse_grid_y = mouse_pos_y / camera.w * TILE_SIZE;
	}
	else mouse_grid_y = mouse_pos_y / camera.w * TILE_SIZE;

	return SDL_Point{ mouse_grid_x,mouse_grid_y };
}

SDL_Point Cursor::Convert_Coord_To_Screen_Pos(Coordinate point, bool center_of_tile)
{
	//SDL_Rect draw_rect = { (pos_rect.x*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, (pos_rect.y*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };


	int x_pos = (point.x*camera.w) + SCREEN_WIDTH / 2 + camera.x;
	int y_pos = (point.y*camera.w) + SCREEN_HEIGHT / 2 + camera.y;

	if (center_of_tile)
	{
		return { x_pos + camera.w/2, y_pos+camera.w/2 };
	}
	else
	{
		return { x_pos, y_pos};
	}
}

void Cursor::Change_Cursor_Icon(int icon_clip_x, int icon_clip_y)
{
	mouse_icon_clip_x = icon_clip_x;
	mouse_icon_clip_y = icon_clip_y;
}

void Cursor::Update_Grid_Position()
{
	Coordinate grid_point = Get_Mouse_Grid_Coord();

	grid_pos = { ((grid_point.x*TILE_SIZE)*camera.w / TILE_SIZE) + SCREEN_WIDTH / 2 + camera.x, ((grid_point.y*TILE_SIZE)*camera.w / TILE_SIZE) + SCREEN_HEIGHT / 2 + camera.y, camera.w, camera.w };
}

int Cursor::Get_Recent_Mouse_Action()
{
	if (left_button == 1)
	{
		if (left_button_previous == 0)
		{
			return INPUT_LEFT_MOUSE_CLICK;
		}
		if (current_mouse_x != held_mouse_x || current_mouse_y != held_mouse_y)
		{
			return INPUT_LEFT_MOUSE_DRAG; // Left Click Drag
		}
	}
	else if (right_button == 1)
	{
		if (right_button_previous == 0) return INPUT_RIGHT_MOUSE_CLICK; // Right Click
		if (current_mouse_x != held_mouse_x || current_mouse_y != held_mouse_y) return INPUT_RIGHT_MOUSE_DRAG; // Right Click Drag
	}
	else if (right_button_previous == 1 && right_button == 0)
	{
		return INPUT_LEFT_MOUSE_UNCLICK; // Left Unclick
	}
	else if (left_button_previous == 1 && left_button == 0)
	{
		return INPUT_RIGHT_MOUSE_UNCLICK; // Right Unclick
	}

	return INPUT_NULL;
}

Coordinate Cursor::Get_Mouse_Grid_Coord()
{
	int mouse_grid_x = 0;
	int mouse_grid_y = 0;
	
	int mouse_pos_x = (current_mouse_x - (SCREEN_WIDTH / 2) - camera.x);
	int mouse_pos_y = (current_mouse_y - (SCREEN_HEIGHT / 2) - camera.y);
	
	if (mouse_pos_x >= 0)
	{
		mouse_grid_x = mouse_pos_x / camera.w;
	}
	else mouse_grid_x = mouse_pos_x / camera.w - 1;

	if (mouse_pos_y >= 0)
	{
		mouse_grid_y = mouse_pos_y / camera.w;
	}
	else mouse_grid_y = mouse_pos_y / camera.w - 1;

	return Coordinate{ mouse_grid_x,mouse_grid_y };
}

void Cursor::Collect_Bus_Messages()
{
	for (int i = 0; i < service_pointer->get_MB_Pointer()->count_input_messages; i++)
	{
		Parse_Input_Message(service_pointer->get_MB_Pointer()->Input_Message_Array[i].Return_Event());
	}
}

void Cursor::Update()
{
	Age_Mouse();

	if (left_button == false && currently_clicked_component != NULL)
	{
		currently_clicked_component->Currently_Clicked(false);
		currently_clicked_component = NULL;
	}
}

void Cursor::Draw()
{
	if (left_button == 1 && currently_clicked_component == NULL)
	{
		int x = min(current_mouse_x, held_mouse_x);
		int y = min(current_mouse_y, held_mouse_y);
		int w = abs(current_mouse_x - held_mouse_x);
		int h = abs(current_mouse_y - held_mouse_y);
		drag_rect = { x,y,w,h };

		service_pointer->get_Draw_System_Pointer()->Draw_Primitive_Directly(service_pointer->get_Game_Renderer(), drag_rect, { 100,100,255,100 }, true);
		service_pointer->get_Draw_System_Pointer()->Draw_Primitive_Directly(service_pointer->get_Game_Renderer(), drag_rect, { 255,255,255,255 }, false);
	}
	else if (currently_clicked_component == NULL)
	{
		// For some reason this function to draw the grid highlight eats like 50fps on its own
		//service_pointer->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(1, grid_pos, true, { 100,100,255,100 });
		///

		service_pointer->get_Draw_System_Pointer()->Draw_Spritesheet_Directly(service_pointer->get_Game_Renderer(), SPRITESHEET_ICON, { current_mouse_x,current_mouse_y,SPRITE_SIZE,SPRITE_SIZE }, { mouse_icon_clip_x, mouse_icon_clip_y, SPRITE_SIZE, SPRITE_SIZE });

	}
	else
	{
		service_pointer->get_Draw_System_Pointer()->Draw_Spritesheet_Directly(service_pointer->get_Game_Renderer(), SPRITESHEET_ICON, { current_mouse_x,current_mouse_y,SPRITE_SIZE,SPRITE_SIZE }, { mouse_icon_clip_x, mouse_icon_clip_y, SPRITE_SIZE, SPRITE_SIZE });
	}
}

void Cursor::Age_Mouse()
{
	if (left_button == true && left_button_previous == false) left_button_previous = true;
	if (right_button == true && right_button_previous == false) right_button_previous = true;

	if (left_button == false && left_button_previous == true) left_button_previous = false;
	if (right_button == false && right_button_previous == true) right_button_previous = false;

	last_mouse_x = current_mouse_x;
	last_mouse_y = current_mouse_y;
}

void Cursor::Parse_Input_Message(SDL_Event event)
{
	if (event.type == SDL_MOUSEMOTION)
	{	
		last_mouse_x = current_mouse_x;
		last_mouse_y = current_mouse_y;		
		current_mouse_x = event.motion.x;
		current_mouse_y = event.motion.y;
		Update_Grid_Position();
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			left_button = true;
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			right_button = true;
		}
		held_mouse_x = event.button.x;
		held_mouse_y = event.button.y;
		current_mouse_x = event.button.x;
		current_mouse_y = event.button.y;
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			left_button = false;
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			right_button = false;
		}
		held_mouse_x = event.button.x;
		held_mouse_y = event.button.y;
		current_mouse_x = event.button.x;
		current_mouse_y = event.button.y;
	}
	else if (event.type == SDL_MOUSEWHEEL)
	{
		if (event.wheel.y > 0 && camera.w <= 48) camera.w++;
		else if (event.wheel.y < 0 && camera.w > 16) camera.w--;
	}
	else if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_a) camera.x += 20;
		else if (event.key.keysym.sym == SDLK_d) camera.x -= 20;
		else if (event.key.keysym.sym == SDLK_w) camera.y += 20;
		else if (event.key.keysym.sym == SDLK_s) camera.y -= 20;
		else if (event.key.keysym.sym == SDLK_MINUS && camera.w > DISPLAY_MIN_ZOOM) camera.w -= 4;
		else if (event.key.keysym.sym == SDLK_EQUALS && camera.w <= DISPLAY_MAX_ZOOM) camera.w += 4;
	}
}

void Cursor::Set_Currently_Clicked_Component(UI_Component_Generic* component)
{
	currently_clicked_component = component;
}
