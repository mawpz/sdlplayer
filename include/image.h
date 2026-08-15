#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <iostream>
#include <SDL2/SDL_image.h>

class Image {
public:
	const char* Path;
	SDL_Rect Position = { 0, 0, 0, 0 };
	SDL_Surface* Surface;
	SDL_Texture* Texture;
	SDL_Renderer* Renderer;

	int Init();
	int Update(const char* NewImage);
	void Destory();
	void Show();
};

int Image::Init()
{
	Surface = IMG_Load(Path);
	Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
	SDL_FreeSurface(Surface);

	return 0;
}

int Image::Update(const char* NewImage)
{
	Path = NewImage;
	SDL_Surface* TempSurface = IMG_Load(Path);
	Texture = SDL_CreateTextureFromSurface(Renderer, TempSurface);
	SDL_FreeSurface(TempSurface);
	return 0;
}

void Image::Show()
{
	SDL_RenderCopy(Renderer, Texture, NULL, &Position);
	return;
}

#endif // __IMAGE_H__
