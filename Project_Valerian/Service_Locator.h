#pragma once

class UI;
class Message_Bus;
class Draw_System;
class Cursor;

class Service_Locator
{
public:
	Service_Locator();

	UI * get_UI_pointer();
	Message_Bus* get_MB_Pointer();
	Draw_System* get_Draw_System_Pointer();
	Cursor* get_Cursor_Pointer();

	void Register_UI_Pointer(UI* ui_pointer);
	void Register_MB_Pointer(Message_Bus* msg_pointer);
	void Register_Draw_System_Pointer(Draw_System* draw_pointer);
	void Register_Cursor_Pointer(Cursor* cursor_pointer);

	void free();

private:
	UI * UI_pointer;
	Message_Bus* msg_bus_pointer;
	Draw_System* draw_system_pointer;
	Cursor* cursor_pointer;
};