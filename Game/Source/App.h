#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "List.h"
#include "PerfTimer.h"
#include "Timer.h"

#include "PugiXml/src/pugixml.hpp"

#define CONFIG_FILENAME		"config.xml"
#define SAVE_STATE_FILENAME "save_game.xml"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class SceneLogo;
class Scene;
class SceneIntro;
class SceneWin;
class SceneLose;
class SceneLevel2;
class Map;
class Player;
//class Enemy;
class EntityManager;
//class Coins;
class Entity;
class ModuleFadeToBlack;
class PathFinding;
class ModuleFonts;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

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

    // Methods to request Load / Save
	void LoadGameRequest();
	void SaveGameRequest() const;
	void LoadConfigRequested();
	void SaveConfigRequested() const;

	void SetLastScene(Module* scene) { lastScene = scene; }
	float GetCapMs();

	int GetFramerate();
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
	bool LoadGame(const SString _filename);
	bool SaveGame(const SString _filename) const;


public:

	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Audio* audio;
	SceneLogo* sceneLogo;
	Scene* scene;
	SceneIntro* sceneIntro;
	SceneWin* sceneWin;
	SceneLose* sceneLose;
	SceneLevel2* sceneLevel2;
	Map* map;
	Player* player;
	//Enemy* enemy;
	EntityManager* entityManager;
	//Coins* coins;
	Entity* entity;
	ModuleFadeToBlack* fade;
	PathFinding* pathfinding;
	ModuleFonts* fonts;

private:

	int argc;
	char** args;
	SString title;
	SString organization;
	int framerate;
	List<Module *> modules;

	uint frames;
	PerfTimer ptimer;
	uint64 frameCount = 0;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;
	uint32 lastSecFrameCount = 0;
	uint32 prevLastSecFrameCount = 0;
	uint32 framesOnLastSecond = 0;
	uint32 lastFrameMs = 0;
	float dt = 0.0f;
	float perfTime;
	float oldLastFrame = 0.0f;
	float timeFramesSecond = 0.0f;
	float	cappedMs = -1;

	mutable bool saveGameRequested;
	bool loadGameRequested;

	pugi::xml_document stateFile;
	pugi::xml_node rootStateFile;

	mutable bool saveConfigRequested;
	bool loadConfigRequested;
	SString filenameGame = "save_game.xml";
	SString filenameConfig = "config.xml";

	float fPS = 0;

	Module* lastScene;
};

extern App* app;

#endif	// __APP_H__