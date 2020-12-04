#include <iostream> 

#include "Defs.h"
#include "Log.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"

#include "Scene.h"
#include "TitleScene.h"
#include "GameOverScene.h"
#include "LogoScene.h"

#include "Map.h"
#include "Pathfinding.h"
#include "EntityPlayer.h"
#include "Pickups.h"
#include "Collisions.h"
#include "WalkingEnemy.h"
#include "FadeToBlack.h"

#include "Application.h"



// Constructor
Application::Application(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(perfTimer);
	wantToSave = wantToLoad = false;

	frameCount = 0;

	input = new Input();
	win = new Window();
	render = new Render();
	tex = new Textures();
	audio = new Audio();

	scene = new Scene();
	titleScene = new TitleScene();
	gameOverScene = new GameOverScene();
	logoScene = new LogoScene();

	map = new Map();
	pathfinding = new Pathfinding();
	player = new EntityPlayer();
	pickups = new Pickups();
	collisions = new Collisions();
	fadeToBlack = new FadeToBlack();
	walkingEnemy	= new WalkingEnemy();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);

	//player and scenes
	AddModule(scene);
	AddModule(gameOverScene);
	AddModule(titleScene);
	AddModule(logoScene);
	AddModule(walkingEnemy);
	AddModule(fadeToBlack);
	AddModule(pickups);
	AddModule(player);

	AddModule(collisions);


	// render last to swap buffer
	AddModule(render);

	pause = false;

	PERF_PEEK(perfTimer);
}

// Destructor
Application::~Application()
{
	// release modules
	ListItem<Module*>* item = modules.end;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void Application::AddModule(Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool Application::Awake()
{
	PERF_START(perfTimer);

	pugi::xml_document	configFile;
	pugi::xml_node		config;
	pugi::xml_node		appConfig;

	bool ret = false;

	config = LoadConfig(configFile);

	frameCap = CAP_AT_30;

	if (config.empty() == false)
	{
		// self-config
		ret = true;
		appConfig = config.child("app");
		title.Create(appConfig.child("title").child_value());
		organization.Create(appConfig.child("organization").child_value());
		frameCap = config.child("app").attribute("framerate_cap").as_uint();
		frameCapOn = config.child("app").attribute("frame_cap_on").as_bool();
		initFrameCap = frameCap;

		loadGame = config.child("file_system").child("load_file").child_value();
		saveGame = config.child("file_system").child("save_file").child_value();

	}

	if (ret == true)
	{
		ListItem<Module*>* item;
		item = modules.start;

		while (item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	PERF_PEEK(perfTimer);

	return ret;
}

// Called before the first frame
bool Application::Start()
{
	PERF_START(perfTimer);

	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}
	scene->active = false;
	titleScene->active = false;
	gameOverScene->active = false;
	player->active = false;
	pickups->active = false;
	map->active = false;

	startupTimer.Start();
	PERF_PEEK(perfTimer);

	return ret;
}

// Called each loop iteration
bool Application::Update()
{
	bool ret = true;
	PrepareUpdate();

	if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = DoUpdate();

	if (ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node Application::LoadConfig(pugi::xml_document& configFile) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = configFile.load_file("config.xml");

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = configFile.child("config");

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frameCount++;
	frameslastSecond++;

	dt = frameTimer.ReadSec();						//Keeps track of the amount of time that has passed since last frame in seconds (processing time of a frame: Frame 1: 0.033secs, ...).
	frameTimer.Start();

}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (wantToSave == true)
		SavegameNow();

	if (wantToLoad == true)
		LoadGameNow();

	//------------ Framerate Calculations ------------
	if (lastSecondTimer.ReadMs() > 1000)
	{
		lastSecondTimer.Start();
		prevSecFrames = frameslastSecond;
		frameslastSecond = 0;
	}

	uint frameCapMs = 1000 / frameCap;
	uint currentFrameMs = frameTimer.Read();

	if (frameCapOn)
	{
		if (currentFrameMs < frameCapMs)						//If the current frame processing time is lower than the specified frame_cap. Timer instead of PerfTimer was used because SDL_Delay is inaccurate.
		{
			trueDelayTimer.Start();

			SDL_Delay(frameCapMs - currentFrameMs);				//SDL_Delay delays processing for a specified time. In this case, it delays for the difference in ms between the frame cap (30fps so 33,3ms per frame) and the current frame.

			uint intendedDelay = frameCapMs - currentFrameMs;

			//LOG("We waited for %d milliseconds and got back in %f", intended_delay, true_delay_timer.ReadMs());
		}
	}

	float avgFps = frameCount / startupTimer.ReadSec();
	secondsSinceStartup = startupTimer.ReadSec();
	uint32 lastFrameMs = frameTimer.Read();
	uint32 framesOnLastUpdate = prevSecFrames;					//Keeps track of how many frames were processed the last second.

	if (frameCapOn)
	{
		frameCapActive = "On";
	}
	else
	{
		frameCapActive = "Off";
	}

	if (vsyncOn)
	{
		vsyncActive = "On";
	}
	else
	{
		vsyncActive = "Off";
	}

	static char title[256];

	sprintf_s(title, 256, "Av.FPS: %.2f / Last Frame Ms: %02u / Last sec frames: %i / Last dt: %.3f / Time since startup: %.3f / Frame Count: %llu / Vsync: %s / Frame cap: %s",
		avgFps, lastFrameMs, framesOnLastUpdate, dt, secondsSinceStartup, frameCount, vsyncActive, frameCapActive);

	app->win->SetTitle(title);
}

// Call modules before each loop iteration
bool Application::PreUpdate()
{
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool Application::DoUpdate()
{
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool Application::PostUpdate()
{
	bool ret = true;
	ListItem<Module*>* item;
	Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool Application::CleanUp()
{
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int Application::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* Application::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* Application::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* Application::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void Application::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	wantToLoad = true;
}

// ---------------------------------------
void Application::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	wantToSave = true;
}

// ---------------------------------------
void Application::GetSaveGames(List<SString>& listToFill) const
{
	// need to add functionality to file_system module for this to work
}

bool Application::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(loadGame.GetString());

	if (result != NULL)
	{
		LOG("Loading new Game State from %s...", loadGame.GetString());

		root = data.child("game_state");

		ListItem<Module*>* item = modules.start;
		ret = true;

		while (item != NULL && ret == true)
		{
			ret = item->data->Load(root.child(item->data->name.GetString()));
			item = item->next;
		}

		data.reset();
		if (ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", loadGame.GetString(), result.description());

	wantToLoad = false;
	return ret;
}

bool Application::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", saveGame.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	ListItem<Module*>* item = modules.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
	}

	if (ret == true)
	{
		data.save_file(saveGame.GetString());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	wantToSave = false;
	return ret;
}