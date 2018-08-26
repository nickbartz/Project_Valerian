#include<Object.h>
#include<SDL.h>
#include<Global_Constants.h>
#include<AI_Component.h>

Object::Object(SDL_Rect location, Service_Locator* sLocator)
{
	core_render = new Core_Render(service_locator);
	service_locator = sLocator;
	object_pos = location;
}

void Object::Assign_Simple_Clip_Tile_Renderer(Service_Locator* service_locator, SDL_Rect tile_specs, int spritesheet,SDL_Rect clip,double ang, SDL_Point* point, SDL_RendererFlip flip )
{
	simple_tile_clip = new Simple_Clip_Tile_Renderer(tile_specs, service_locator, spritesheet, clip, ang, point, flip);
}

void Object::Assign_Background_Renderer(Service_Locator* service_locator, int spritesheet, SDL_Rect clip, double ang, SDL_Point* point, SDL_RendererFlip flip)
{
	background_clip = new Background_Renderer(service_locator, spritesheet, clip, ang, point, flip);
}


void Object::Draw(SDL_Rect camera, SDL_Rect pos_rect)
{
	if (core_render != NULL) core_render->Draw();
	if (simple_tile_clip != NULL) simple_tile_clip->Draw();
	if (background_clip != NULL) background_clip->Draw(pos_rect);
	if (tile_multi_clip != NULL) tile_multi_clip->Draw();
}

void Object::free()
{
	if (core_render != NULL) delete core_render, core_render = NULL;
	if (simple_tile_clip != NULL) delete simple_tile_clip, simple_tile_clip = NULL;
	if (background_clip != NULL) delete background_clip, background_clip = NULL;
	if (tile_multi_clip != NULL) delete tile_multi_clip, tile_multi_clip = NULL;
}