#ifndef __APP_H__
#define __APP_H__

#include "List.h"
#include "Module.h"
#include "External/PugiXml\src\pugixml.hpp"
#include "PerfTimer.h"
#include "Timer.h"

// Utility Classes
class Timer;
class PerfTimer;

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class TitleScene;
class GameOverScene;
class LogoScene;
class Map;
class EntityPlayer;
class Pickups;
class Collisions;
class WalkingEnemy;
class FadeToBlack;
class Application
{
public:

	// Constructor
	Application(int argc, char* args[]);

	// Destructor
	virtual ~Application();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	float GetDt() const;
	float GetUnpausableDt() const;

	void LoadGame();
	void SaveGame() const;
	void GetSaveGames(List<SString>& listToFill) const;

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	Window*			win;
	Input*			input;
	Render*			render;
	Textures*		tex;
	Audio*			audio;
	Scene*			scene;
	TitleScene*		titleScene;
	GameOverScene*  gameOverScene;
	LogoScene*		logoScene;
	Map*			map;
	EntityPlayer*	player;
	Pickups*		pickups;
	Collisions*		collisions;
	WalkingEnemy*	walkingEnemy;
	FadeToBlack*	fadeToBlack;

	uint				frameCap;								// Stores the frames per second cap to be applied.
	uint				initFrameCap;						// Stores the original frame cap at application start.
	float				secondsSinceStartup;					// Secons that have elapsed since app start.

	bool				frameCapOn;						// Keeps track whether the frame cap is on or off.
	bool				vsyncOn;						// Keeps track whether Vsync is on or off.
	bool				pause;
	

private:

	List<Module*>		modules;
	int					argc;
	char**				args;

	SString			title;
	SString			organization;

	mutable bool		wantToSave;
	bool				wantToLoad;
	SString				loadGame;
	mutable SString		saveGame;

	//Framerate
	uint64					frameCount;
	Timer					startupTimer;							// Used to keep track of time since app start.
	Timer					frameTimer;								// Keeps track of everything time related in the span of a frame.
	PerfTimer				perfTimer;								// Creates a pointer to PerfTimer tool. Gives access to j1PerfTimer's elements. Used to keep track of time since app start.
	PerfTimer				lastSecondTimer;						// Creates a pointer to PerfTimer tool. Used to calculate variables in spans of one second.
	uint32					lastUpdateMs;							// Calculates the amount of milliseconds that the last update spent running.
	uint32					frameslastSecond;						// Calculates the amount of frames that where processed the last second.
	uint32					prevSecFrames;

	PerfTimer				trueDelayTimer;							// Timer that will be used to see the actual amount of delay that was applied to cap the framerate.
	float					dt;										// Keeps track of the amount of time in milliseconds that has passed in a frame. 
																	// Will be used to make everything (update()) be in the same timestep.
	char*					frameCapActive;							// String that is set to 'On' when the frame cap is on and  'Off' when it is off.
	char*					vsyncActive;
};

extern Application* app;

#endif