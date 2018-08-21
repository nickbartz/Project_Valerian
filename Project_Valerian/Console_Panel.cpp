#include<Console_Panel.h>

Console_Panel::Console_Panel(int window_name, SDL_Rect rect)
{
	offset_rect = rect;
	window = window_name;

}

int Console_Panel::Get_Panel_Name()
{
	return panel_name;
}

void Console_Panel::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{

}

void Console_Panel::Change_Rect(SDL_Rect new_rect)
{
	offset_rect = new_rect;
}

void Console_Message_Panel::Draw(Draw_System* draw_system, SDL_Rect base_rect)
{
	message_stream.Draw(draw_system, base_rect);
}

void Console_Message_Panel::Push_Message_Into_Stream(string new_message)
{
	message_stream.Push_Message_Into_Stream(new_message);
}