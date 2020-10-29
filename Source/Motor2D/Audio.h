#ifndef __Audio_H__
#define __Audio_H__

#include "Module.h"
#include "Dependencies/SDL_mixer\include\SDL_mixer.h"
#include "p2List.h"

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

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	// Set Volumes - If vol < 0 just return current value; 
	int SetMusicVolume(float m_volume);
	int SetFxVolume(float fx_volume);

private:

	_Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;
};

#endif // __Audio_H__