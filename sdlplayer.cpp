#include <iostream>
#include <string>
#include <filesystem>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <magic.h>

#include "./include/image.h"
#include "./include/audio.h"
#include "./include/label.h"

int window_width = 1040;
int window_height = 230;
const char* window_title = "SDLplayer";

const char* default_font = "/usr/share/fonts/TTF/IosevkaNerdFont-Regular.ttf";
const char* italic_font = "/usr/share/fonts/TTF/IosevkaNerdFont-Italic.ttf";

void CreatePlaceholderImage()
{
	// PPM Documentation @ <https://netpbm.sourceforge.net/doc/ppm.html>
	int w = 360;
	int h = 360;
	FILE *image = fopen("/tmp/placeholder.ppm", "wb");
	fprintf(image, "P6\n");
	fprintf(image, "%d %d\n", w, h);
	fprintf(image, "255\n");

	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if ((x/60 + y/60) % 2) {
				fputc(0xc4, image);
				fputc(0xa7, image);
				fputc(0xe7, image);
			} else {
				fputc(0x21, image);
				fputc(0x20, image);
				fputc(0x2e, image);
			}
		}
	}
	fclose(image);
	return;
}

std::string GetMIMEType(const std::string &file)
{
	magic_t cookie = magic_open(MAGIC_MIME_TYPE);
	if (cookie == nullptr)
		return "unknown";

	if (magic_load(cookie, nullptr)) {
		std::cout << magic_error(cookie) << "\n";
		magic_close(cookie);
		return "unknown";
	}

	const char* mime = magic_file(cookie, file.c_str());
	std::string Result = (mime == nullptr) ? "unknown" : mime;
	magic_close(cookie);
	return Result;
}

const char* GetLineOfFile(std::string Path, int Line)
{
	FILE* file = fopen(Path.c_str(), "r");
	if (!file)
		return NULL;

	char Buffer[256];
	int CurrentLine = 0;
	char* LineText = NULL;

	while (fgets(Buffer, sizeof(Buffer), file) != NULL) {
		CurrentLine++;

		if (CurrentLine == Line) {
			size_t len = strlen(Buffer);
			if (len > 0 && Buffer[len - 1] == '\n')
				Buffer[len - 1] = '\0';

			LineText = strdup(Buffer);
			break;
		}
	}

	fclose(file);
	return LineText;

}

int InitSDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);

	return 0;
}

int main(int argc, char* argv[])
{
	if (!argv[1]) {
		std::cerr << "No file provided" << std::endl;
		return -1;
	}

	std::string Arg1_MIME_Type = GetMIMEType(argv[1]);
	int CurrentSong = 1;

	if (Arg1_MIME_Type == "text/plain")
	{
		std::cout << "Using Playlist File: \033[1;92m" << argv[1] << "\033[0m" << std::endl;
	}

	if (std::filesystem::exists("/tmp/placeholder.ppm"))
		remove("/tmp/placeholder.ppm");

	CreatePlaceholderImage();

	SDL_Window* window;
	SDL_Surface* window_surface;

	InitSDL();

	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				  window_width, window_height, SDL_WINDOW_SHOWN);

	if (!window) {
		std::cout << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	window_surface = SDL_GetWindowSurface(window);
	SDL_FillRect(window_surface, NULL, SDL_MapRGB(window_surface->format, 0x00, 0x00, 0x00));
	SDL_UpdateWindowSurface(window);

	Label SongName;
	SongName.Renderer = renderer;
	SongName.FontSize = 36;
	SongName.Position = { 235, 35, 0, 0 };
	SongName.FontPath = default_font;
	SongName.Text = "Example Song";
	SongName.Color = { 224, 222, 244, 255 };
	SongName.Init();

	Label Artist;
	Artist.Renderer = renderer;
	Artist.FontSize = 20;
	Artist.Position = { 235, 100, 0, 0 };
	Artist.Color =  {  110, 106, 134, 255 };
	Artist.FontPath = italic_font;
	Artist.Text = "Artist";
	Artist.Init();

	Label Album;
	Album.Renderer = renderer;
	Album.FontSize = 16;
	Album.Position = { 235, 125, 0, 0 };
	Album.Color = {  110, 106, 134, 255 };
	Album.FontPath = italic_font;
	Album.Text = "Album (Year)";
	Album.Init();

	Label SongPosition;
	SongPosition.Renderer = renderer;
	SongPosition.FontSize = 16;
	SongPosition.Position = { 235, 155, 0, 0 };
	SongPosition.Color =  {  110, 106, 134, 255 };
	SongPosition.FontPath = default_font;
	SongPosition.Text = "00:00";
	SongPosition.Init();

	Label Volume;
	Volume.Renderer = renderer;
	Volume.FontSize = 16;
	Volume.Position = { 235, 175, 0, 0 };
	Volume.Color =  {  110, 106, 134, 255 };
	Volume.FontPath = default_font;
	Volume.Text = "Volume: 100%";
	Volume.Init();

	Image AlbumCover;
	AlbumCover.Renderer = renderer;
	AlbumCover.Path = "/tmp/placeholder.ppm";
	AlbumCover.Position = { 15, 15, 200, 200 };
	AlbumCover.Init();

	Audio AudioFile;
	if (Arg1_MIME_Type != "text/plain")
		AudioFile.Path = argv[1];
	else
		AudioFile.Path = GetLineOfFile(argv[1], CurrentSong);

	AudioFile.Init();

	SDL_Event e;
	int i = 2;
	bool is_active = true;

	AudioFile.Play();

	if (std::filesystem::exists("/tmp/cover.png"))
		AlbumCover.Update("/tmp/cover.png");
	else
		AlbumCover.Update("/tmp/placeholder.ppm");

	SongName.Update("%s", AudioFile.Song.c_str());
	Artist.Update("%s", AudioFile.Artist.c_str());
	Album.Update("%s (%d)", AudioFile.Album.c_str(), AudioFile.Year);

	while (is_active)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				is_active = false;

			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
					case SDLK_SPACE:
						AudioFile.Pause();
						break;

					case SDLK_LEFT:
						AudioFile.Skip(5, 0);
						break;

					case SDLK_RIGHT:
						AudioFile.Skip(5, 1);
						break;

					case SDLK_UP:
						AudioFile.SetVolume(5, 1);
						Volume.Update("Volume: %d%%", AudioFile.VolumeLevel);
						break;

					case SDLK_DOWN:
						AudioFile.SetVolume(5, 0);
						Volume.Update("Volume: %d%%", AudioFile.VolumeLevel);
						break;

					case SDLK_n:
						AudioFile.AudioFinished = true;
						break;

					case SDLK_p:
						if (i - 2 < 1)
							i = 1;
						else
							i = i - 2;
						AudioFile.AudioFinished = true;
						break;

					case SDLK_q:
						is_active = false;
						break;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 25, 23, 36, 255);
		SDL_RenderClear(renderer);

		AlbumCover.Show();
		SongName.Show();
		Album.Show();
		Artist.Show();
		SongPosition.Show();
		Volume.Show();

		if (AudioFile.AudioPaused)
			SongPosition.Update("%s [Paused]", AudioFile.GetPositionFormatted().c_str());
		else
			SongPosition.Update("%s", AudioFile.GetPositionFormatted().c_str());

		if (AudioFile.AudioFinished) {
			if (argv[i] + 1 == NULL && Arg1_MIME_Type != "text/plain")
				AudioFile.Destroy();
			else {
				if (Arg1_MIME_Type != "text/plain")
					AudioFile.Path = argv[i];
				else {
					CurrentSong = i;
					AudioFile.Path = GetLineOfFile(argv[1], i);
				}
				AudioFile.Reload();
				AudioFile.Play();

				SongName.Update("%s", AudioFile.Song.c_str());
				Artist.Update("%s", AudioFile.Artist.c_str());
				Album.Update("%s (%d)", AudioFile.Album.c_str(), AudioFile.Year);
				if (std::filesystem::exists("/tmp/cover.png"))
					AlbumCover.Update("/tmp/cover.png");
				else
					AlbumCover.Update("/tmp/placeholder.ppm");
				i++;
			}
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	AudioFile.Destroy();

	SDL_Quit();

	return 0;
}
