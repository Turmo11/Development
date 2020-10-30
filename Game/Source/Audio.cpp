#include "p2Defs.h"
#include "p2Log.h"
#include "Audio.h"
#include "p2List.h"
#include "Input.h"

#include "External/SDL/include/SDL.h"
#include "External/SDL_mixer\include\SDL_mixer.h"

Audio::Audio() : Module()
{
	music = NULL;
	name.create("audio");
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake(pugi::xml_node& config)
{
	volume = config.child("volume").attribute("value").as_float();
	maxVolume = config.child("maxVolume").attribute("value").as_float();
	
	LOG("Loading Audio Mixer");

	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	return ret;
}

bool Audio::Start()
{
	SetFxVolume(volume);
	SetMusicVolume(volume);
	return true;
}

// Load Game State
bool Audio::Load(pugi::xml_node& data)
{
	volume = data.attribute("volume").as_float();
	SetFxVolume(volume);
	SetMusicVolume(volume);
	LOG("Loading Volume");
	return true;
}

// Save Game State
bool Audio::Save(pugi::xml_node& data) const
{
	data.append_attribute("volume") = volume;

	return true;
}


// Called before quitting
bool Audio::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}

	p2List_item<Mix_Chunk*>* item;
	for (item = fx.start; item != NULL; item = item->next)
		Mix_FreeChunk(item->data);

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fadeTime)
{
	bool ret = true;

	if (!active)
		return false;

	if (music != NULL)
	{
		if (fadeTime > 0.0f)
		{
			Mix_FadeOutMusic(int(fadeTime * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if (music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if (fadeTime > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fadeTime * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

void Audio::StopMusic()
{
	Mix_PauseMusic();
	LOG("Stop music");
}
void Audio::ResumeMusic()
{
	Mix_ResumeMusic();
	LOG("Resume music");
}

// Load WAV
unsigned int Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if (!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if (chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.add(chunk);
		ret = fx.count();
	}

	return ret;
}

// Play WAV
bool Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if (!active)
		return false;

	if (id > 0 && id <= fx.count())
	{
		Mix_PlayChannel(-1, fx[id - 1], repeat);
	}

	return ret;
}

int Audio::SetMusicVolume(float mVolume)
{
	if (mVolume >= 0.0f)
	{
		return Mix_VolumeMusic(MIX_MAX_VOLUME * mVolume);
	}
	else 
	{
		return Mix_VolumeMusic(-1);
	}
}

int Audio::SetFxVolume(float fxVolume)
{
	if (fxVolume >= 0.0f)
	{
		return Mix_Volume(-1, MIX_MAX_VOLUME * fxVolume);
	}
	else
	{
		return Mix_Volume(-1, -1);
	}
}

