#include<Draw_System.h>
#include<Service_Locator.h>
#include<Render_Component.h>
#include<iostream>
#include<SDL.h>

using namespace std;

Core_Render::Core_Render(Service_Locator* sLocator)
{
	service_locator = sLocator;
}

void Core_Render::Draw()
{

}

void Simple_Clip_Render::Draw(SDL_Rect pos_rect)
{
	service_locator->get_Draw_System_Pointer()->Add_Sprite_Render_Job_To_Render_Cycle(spritesheet_num, pos_rect, sprite_clip, angle, center,flip);
}

void Multi_Clip_Render::Draw()
{

}