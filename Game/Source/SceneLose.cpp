#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "SceneLose.h"
#include "ModuleFadeToBlack.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

SceneLose::SceneLose() : Module()
{
	name.Create("sceneLose");
}

SceneLose::~SceneLose()
{
}

bool SceneLose::Awake()
{
	LOG("Loading SceneLose");
	bool ret = true;
	return ret;
}

bool SceneLose::Start()
{
	app->SetLastScene((Module*)this);
	transition = false;

	app->audio->PlayMusic("Assets/Audio/Music/MusicLost.ogg");
	img = app->tex->Load("Assets/Textures/end_screen.png");
	animationEnd.texture = app->tex->Load("Assets/Textures/dino_sprites_dead.png");
	animationEnd.position = { 480 , 345 };
	idleAnim.loop = true;
	idleAnim.speed = 0.008f;

	for (int i = 0; i < 9; i++)
		idleAnim.PushBack({ 336 * i,0, 336, 336 });

	animationEnd.currentAnimation = &idleAnim;

	SDL_QueryTexture(img, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;

	timer.Start();
	return true;
}

bool SceneLose::PreUpdate()
{
	return true;
}

bool SceneLose::Update(float dt)
{
	animationEnd.currentAnimation->Update();
	if (CheckChangeFPS(app->GetFramerate()))
	idleAnim.speed = (dt*100 )*0.08f;

	return true;
}

bool SceneLose::PostUpdate()
{
	bool ret = true;
	SDL_Rect rectIntro;
	rectIntro = animationEnd.currentAnimation->GetCurrentFrame();

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if ((app->input->GetKey(SDL_SCANCODE_KP_ENTER) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN
		|| app->input->GetKey(SDL_SCANCODE_RETURN2) == KEY_DOWN) && !transition && timer.ReadSec() > CCOOLDOWNSCENE)
	{
		app->fade->FadeToBlack(this, (Module*)app->scene, 60.f);
		return true;
	}

	app->render->DrawTexture(img, app->render->camera.x, app->render->camera.y);
	app->render->DrawTexture(animationEnd.texture, animationEnd.position.x, animationEnd.position.y, &rectIntro);

	return ret;
}

bool SceneLose::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->tex->UnLoad(img);
	app->tex->UnLoad(animationEnd.texture);
	img = nullptr;
	animationEnd.texture = nullptr;
	active = false;
	return true;
}
