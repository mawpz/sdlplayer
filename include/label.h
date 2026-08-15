// vim: filetype=cpp

#ifndef __LABEL_H__
#define __LABEL_H__

#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Label {
public:
	const char* FontPath = NULL;
	int FontSize = 12;
	const char* Text = "[ Default Text ]";
	SDL_Rect Position = { 0, 0, 0, 0 };
	SDL_Color Color = {255, 255, 255, 255};
	SDL_Surface* Surface;
	SDL_Texture* Texture;
	SDL_Renderer* Renderer;

	int Init();
	int Update(const char* NewText, ...);
	void Show();
	void Destroy();
private:
	TTF_Font* Font;
};

int Label::Init()
{
	Font = TTF_OpenFont(FontPath, FontSize);
	Surface = TTF_RenderText_Blended(Font, Text, Color);
	Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
	Position = { Position.x, Position.y, Surface->w, Surface->h };

	return 0;
}

void Label::Destroy()
{
	SDL_DestroyTexture(Texture);
	SDL_FreeSurface(Surface);
	TTF_CloseFont(Font);
}

int Label::Update(const char* NewText, ...)
{
	char Buffer[128];

	va_list Args;
	va_start(Args, NewText);
	vsnprintf(Buffer, sizeof(Buffer), NewText, Args);
	va_end(Args);

	Text = Buffer;

	if (Texture)
		SDL_DestroyTexture(Texture);

	SDL_Surface* TempSurface = TTF_RenderText_Blended(Font, Text, Color);
	Texture = SDL_CreateTextureFromSurface(Renderer, TempSurface);
	Position.w = TempSurface->w;
	Position.h = TempSurface->h;
	SDL_FreeSurface(TempSurface);

	return 0;
}

void Label::Show()
{
	SDL_RenderCopy(Renderer, Texture, NULL, &Position);
	return;
}

#endif // __LABEL_H__ 
