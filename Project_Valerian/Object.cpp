#include<Object.h>
#include<SDL.h>
#include<Global_Constants.h>

Object::Object(Service_Locator* sLocator, int x_grid, int y_grid, int w, int h)
{
	core_render = new Core_Render(service_locator);
	service_locator = sLocator;

	grid_x = x_grid;
	grid_y = y_grid;
	tile_width = w;
	tile_height = h;

}

void Object::Assign_Simple_Clip_Renderer(int spritesheet,SDL_Rect clip,double ang, SDL_Point* point, SDL_RendererFlip flip )
{
	simple_clip = new Simple_Clip_Render(service_locator, spritesheet, clip, ang, point, flip);
}

void Object::Draw(SDL_Rect camera)
{
	SDL_Rect pos_rect = { (grid_x*camera.w) + SCREEN_WIDTH + camera.x, (grid_y*camera.w) + SCREEN_HEIGHT + camera.y, tile_width*camera.w, tile_height*camera.w };
	
	if (core_render != NULL) core_render->Draw();
	if (simple_clip != NULL) simple_clip->Draw(pos_rect);
	if (multi_clip != NULL) multi_clip->Draw();
}

void Object::Draw_From_Instance(SDL_Rect pos_rect)
{
	SDL_Rect render_rect = { pos_rect.x, pos_rect.y, pos_rect.w, pos_rect.h};
	
	if (simple_clip != NULL) simple_clip->Draw(render_rect);
}

void Object::free()
{
	if (core_render != NULL) delete core_render, core_render = NULL;
	if (simple_clip != NULL) delete simple_clip, simple_clip = NULL;
	if (multi_clip != NULL) delete multi_clip, multi_clip = NULL;
}