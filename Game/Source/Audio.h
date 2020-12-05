#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"
#include "External/SDL_mixer\include\SDL_mixer.h"
#include "List.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);
	void StopMusic();
	void ResumeMusic();

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	// Set Volumes - If vol < 0 just return current value; 
	int SetMusicVolume(float mVolume);
	int SetFxVolume(float fxVolume);

private:

	_Mix_Music*			music;
	List<Mix_Chunk*>	fx;

public:

	float mVolume;
	float fxVolume;
	float maxVolume = 1.0f;
};

#endif // __Audio_H__