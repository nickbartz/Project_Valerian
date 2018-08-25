#include <Service_Locator.h>
#include<UI.h>
#include<Draw_System.h>
#include<Cursor.h>
#include<Message_Array.h>

Service_Locator::Service_Locator()
{

}

UI * Service_Locator::get_UI_pointer()
{
	return UI_pointer;
}

Message_Array* Service_Locator::get_MB_Pointer()
{
	return msg_bus_pointer;
}

Draw_System* Service_Locator::get_Draw_System_Pointer()
{
	return draw_system_pointer;
}

Cursor* Service_Locator::get_Cursor_Pointer()
{
	return cursor_pointer;
}

void Service_Locator::Register_UI_Pointer(UI* ui_pointer)
{
	UI_pointer = ui_pointer;
}

void Service_Locator::Register_MB_Pointer(Message_Array* msg_pointer)
{
	msg_bus_pointer = msg_pointer;
}

void Service_Locator::Register_Draw_System_Pointer(Draw_System* draw_pointer)
{
	draw_system_pointer = draw_pointer;
}

void Service_Locator::Register_Cursor_Pointer(Cursor* cPointer)
{
	cursor_pointer = cPointer;
}

void Service_Locator::free()
{
	draw_system_pointer = NULL;
	msg_bus_pointer = NULL;
	UI_pointer = NULL;
	cursor_pointer = NULL;
}