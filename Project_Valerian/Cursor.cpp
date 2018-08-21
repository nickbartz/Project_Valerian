#include <Cursor.h>
#include<Draw_System.h>
#include<algorithm>
#include<Service_Locator.h>
#include<Console_Component.h>

Cursor::Cursor(Service_Locator* sPointer)
{
	service_pointer = sPointer;
}

void Cursor::Update()
{
	Age_Mouse();

	for (int i = 0; i < current_num_input_messages; i++) Parse_Input_Message(input_message_local[i].Return_Event());

	if (left_button_previous == 1 && left_button == 0 && currently_clicked_component != NULL)
	{
		currently_clicked_component->Currently_Clicked(false);
		currently_clicked_component = NULL;
	}

	if (currently_clicked_component == NULL) Draw();

	Clear_Messages();
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

void Cursor::Push_Message_To_Cursor(Input_Message message)
{
	input_message_local[current_num_input_messages] = message;

	current_num_input_messages++;
	if (current_num_input_messages > MAX_INPUT_MESSAGES) current_num_input_messages = 0;
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
}

void Cursor::Set_Currently_Clicked_Component(Console_Component* component)
{
	currently_clicked_component = component;
}

void Cursor::Clear_Messages()
{
	// Set the index of input messages back to 0 so it starts empty essentially
	current_num_input_messages = 0;
}