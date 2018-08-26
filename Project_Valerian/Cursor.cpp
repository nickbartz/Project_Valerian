#include <Cursor.h>
#include<Draw_System.h>
#include<algorithm>
#include<Service_Locator.h>
#include<Console_Component.h>

Cursor::Cursor(Service_Locator* sPointer)
{
	service_pointer = sPointer;
	camera = { 0, 0,32, 0 };
}

SDL_Rect Cursor::Get_Camera()
{
	return camera;
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

	if (left_button_previous == 1 && left_button == 0 && currently_clicked_component != NULL)
	{
		currently_clicked_component->Currently_Clicked(false);
		currently_clicked_component = NULL;
	}

	if (currently_clicked_component == NULL) Draw();

}

void Cursor::Draw()
{
	if (left_button == 1)
	{
		int x = min(current_mouse_x, held_mouse_x);
		int y = min(current_mouse_y, held_mouse_y);
		int w = abs(current_mouse_x - held_mouse_x);
		int h = abs(current_mouse_y - held_mouse_y);
		drag_rect = { x,y,w,h };

		Draw_System::Primitive_Instruction drag_overlay = { 1, drag_rect, true,{ 100,100,255,100 } };
		service_pointer->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(drag_overlay);
		drag_overlay = { 1, drag_rect, false,{ 255,255,255,255 } };
		service_pointer->get_Draw_System_Pointer()->Add_Primitive_To_Render_Cycle(drag_overlay);
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
		if (event.wheel.y > 0 && camera.w <= 50) camera.w++;
		else if (event.wheel.y < 0 && camera.w > 0) camera.w--;
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

void Cursor::Set_Currently_Clicked_Component(Console_Component* component)
{
	currently_clicked_component = component;
}
