#pragma once

#include<SDL.h>
class Service_Locator;

class Core_Render
{
public:
	Core_Render(Service_Locator* service_locator = NULL);
	void Draw();

protected:
	Service_Locator * service_locator = NULL;
};


class Simple_Clip_Render :public Core_Render
{
public:
	Simple_Clip_Render(Service_Locator* service_locator, int sheet_num, SDL_Rect clip, double ang = 0.0, SDL_Point* point = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) :Core_Render(service_locator)
	{
		spritesheet_num = sheet_num;
		sprite_clip = clip;
		angle = ang;
		center = point;
	}

	void Draw(SDL_Rect pos_rect);

private:
	int spritesheet_num;
	SDL_Rect sprite_clip;
	double angle;
	SDL_RendererFlip flip;
	SDL_Point* center = NULL;
};

class Multi_Clip_Render :public Core_Render
{
public:
	Multi_Clip_Render() : Core_Render()
	{

	}

	void Draw();
};