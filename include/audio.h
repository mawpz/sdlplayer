#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <atomic>
#include <string>
#include <sstream>
#include <iomanip>

#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class Audio
{
public:
	std::thread AudioThread;
	std::atomic<bool> AudioActive{false};
	bool AudioPaused = false;
	bool AudioFinished = false;
	bool ShowOutput = false;
	Mix_Music* AudioStream;
	const char* Path;
	int VolumeLevel = 100;

	std::string Song;
	std::string Artist;
	std::string Album;
	int Year;

	int Init();
	void Play();
	void Pause();
	void PlayAudio();
	void Skip(double DurationSeconds, int Direction);
	void SetVolume(int NewVolume, int Direction);
	void Stop();
	void Reload();
	void Destroy();

	std::string GetPositionFormatted() const
	{
		if (!AudioActive || !AudioStream) return "00:00";
		double TotalSeconds = Mix_GetMusicPosition(AudioStream);
		if (TotalSeconds < 0) return "00:00";

		int minutes = static_cast<int>(TotalSeconds) / 60;
		int seconds = static_cast<int>(TotalSeconds) % 60;

		std::ostringstream oss;
		oss << std::setfill('0')
			<< std::setw(2) << minutes << ":"
			<< std::setw(2) << seconds;

		return oss.str();
	}
};

int Audio::Init()
{
	if (std::filesystem::exists("/tmp/cover.png"))
		remove("/tmp/cover.png");

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	AudioStream = Mix_LoadMUS(Path);

	TagLib::MPEG::File file(Path);
	TagLib::ID3v2::Tag* Tag = file.ID3v2Tag(true);
	TagLib::ID3v2::FrameList FrameList = Tag->frameList("APIC");
	if (!FrameList.isEmpty()) {
		auto* frame = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(FrameList.front());
		TagLib::ByteVector imageData = frame->picture();

		std::string output = "/tmp/cover.png";
		std::ofstream output_file(output, std::ios::out | std::ios::binary);

		output_file.write(imageData.data(), imageData.size());
		output_file.close();
	}


	if (Tag->title() == "") {
		std::cout << "  \033[1;95mNo Title tag found in file\033[0m. Just using filename" << std::endl;;
		Song = std::filesystem::path(Path).filename().string();
		Artist = "Unknown Artist";
		Album = "Unknown Album";
		Year = 2005;
	} else {
		Song = Tag->title().toCString(true);
		Artist = Tag->artist().toCString(true);
		Album = Tag->album().toCString(true);
		Year = Tag->year();
	}

	if (ShowOutput) {
		std::cout << "Now: \033[1;95m" << Song << "\033[0m by \033[1;93m" \
			<< Artist << "\033[0m on \033[1;92m" << Album << "\033[0m\n";
	}

	return 0;
}

void Audio::Pause()
{
	if (AudioPaused) {
		Mix_ResumeMusic();
		AudioPaused = false;
	} else {
		Mix_PauseMusic();
		AudioPaused = true;
	}
	return;
}

void Audio::Play()
{
	if (AudioActive)
		return;

	AudioThread = std::thread(&Audio::PlayAudio, this);
	return;
}

void Audio::PlayAudio()
{
	if (!AudioActive)
		AudioActive = true;

	Mix_PlayMusic(AudioStream, 0);
	AudioFinished = false;

	while (Mix_PlayingMusic() && AudioActive)
		SDL_Delay(100);

	Mix_HaltMusic();
	AudioActive = false;
	AudioFinished = true;

	return;
}

void Audio::Destroy()
{
	Stop();
	Mix_FreeMusic(AudioStream);
	return;
}

void Audio::Stop()
{
	AudioActive = false;
	if (AudioThread.joinable())
		AudioThread.join();
}

void Audio::Skip(double DurationSeconds, int Direction)
{
	// Direction: 0: Backwards, 1: Forwards
	if (!AudioActive || !AudioStream)
		return;

	double CurrentPosition = Mix_GetMusicPosition(AudioStream);
	double NewPosition;

	if (Direction == 0)
		NewPosition = CurrentPosition - DurationSeconds;
	else
		NewPosition = CurrentPosition + DurationSeconds;

	if (NewPosition < 0) NewPosition = 0;
	Mix_SetMusicPosition(NewPosition);

	return;
}

void Audio::SetVolume(int NewVolume, int Direction) {
	int CurrentVolume = Mix_VolumeMusic(-1);
	int Volume = 100;

	switch (Direction) {
		case 1:
			Volume = CurrentVolume + NewVolume;
			break;
		case 0:
			Volume = CurrentVolume - NewVolume;
			break;
	}

	if (Volume > 100)
		Volume = 100;
	if (Volume < 0)
		Volume = 0;

	Mix_VolumeMusic(Volume);
	VolumeLevel = Volume;
	return;
}

void Audio::Reload() {
	Destroy();
	AudioPaused = false;
	Init();
}

#endif // __AUDIO_H__
