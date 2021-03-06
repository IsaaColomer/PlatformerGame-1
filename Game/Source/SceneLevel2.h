#ifndef __SCENE_LEVEL_2_H__
#define __SCENE_LEVEL_2_H__

#include "Module.h"
#include "Animation.h"
#include "Point.h"

#include "PugiXml\src\pugixml.hpp"

struct SDL_Texture;

struct AnimationFather2
{
	iPoint position;
	Animation* currentAnimation;
	SDL_Texture* texture;
};

class SceneLevel2 : public Module
{
public:

	SceneLevel2();

	// Destructor
	virtual ~SceneLevel2();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	void SetDebugCollaider(bool value = NULL);
	bool* GetDebugCollaider() { return &debugCollisions; }
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();


private:

	void Parallax();
	void DebugKeys();

	bool debugCollisions = false;
	bool victory = false;
	bool lose = false;
	AnimationFather2 animationFather;
	Animation idleAnim;
	SDL_Texture* img;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	float speedImg = 0;
};

#endif // __SCENELEVEL2_H__

