#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "SString.h"
#include "Module.h"
#include "Animation.h"
#include "PugiXml/src/pugixml.hpp"


enum State {

	IDLE,
	JUMP,
	WALK,
	RUN,
};

enum MoveDirection {

	WALK_R,
	WALK_L,
};
enum TypeCollision {

	VICTORY,
	COLLISION,
	CHECK_POINT,
};

class Entity : public Module
{

public:

	Entity();
	~Entity();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);

	// Called before the first frame
	virtual bool Start();
	// Called each loop iteration
	virtual bool PreUpdate();

	// Called each loop iteration
	virtual bool Update(float dt);
	// Called each loop iteration
	virtual bool PostUpdate();
	// Called before quitting
	virtual bool CleanUp();

	// Virtual methods to Load state
	 bool LoadState(pugi::xml_node&);
	// Virtual methods to Save state
	 bool SaveState(pugi::xml_node&) const;
public:

	bool isAlive= false;


};

#endif // __MODULE_H__