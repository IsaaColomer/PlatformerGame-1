#include "App.h"
#include "ModuleFonts.h"
#include "Textures.h"
#include "Render.h"

#include "Defs.h"
#include "Log.h"
#include<string.h>

ModuleFonts::ModuleFonts() : Module()
{
	name.Create("fonts");

}

ModuleFonts::~ModuleFonts(){
	
}

bool ModuleFonts::CleanUp(){

	LOG("Freeing all fonts\n");

	for (uint i = 0; i < MAX_FONTS; ++i){
		if (fonts[i].texture != nullptr){
			app->tex->UnLoad(fonts[i].texture);
			fonts[i].texture = nullptr;
		}
	}
	
	return true;
}

// Load new texture from file path
int ModuleFonts::Load(const char* texture_path, const char* characters, uint rows, int image_w, int image_h)
{
	int id = -1;

	if(texture_path == nullptr || characters == nullptr || rows == 0)
	{
		LOG("Could not load font");
		return id;
	}

	SDL_Texture* tex = app->tex->Load(texture_path);

	if(tex == nullptr || strlen(characters) >= MAX_FONT_CHARS)
	{
		LOG("Could not load font at %s with characters '%s'", texture_path, characters);
		return id;
	}

	id = 0;
	for(; id < MAX_FONTS; ++id)
		if(fonts[id].texture == nullptr)
			break;

	if(id == MAX_FONTS)
	{
		LOG("Cannot load font %s. Array is full (max %d).", texture_path, MAX_FONTS);
		return id;
	}

	Font& font = fonts[id];

	font.texture = tex;
	font.rows = rows;

	// TODO 1: Finish storing font data

	// totalLength ---	length of the lookup table
	// table ---------  All characters displayed in the same order as the texture
	// columns -------  Amount of chars per row of the texture
	// char_w --------	Width of each character
	// char_h --------	Height of each character
	font.totalLength = strlen(characters);
	strcpy_s(font.table, characters);
	font.columns = font.totalLength / font.rows;

	font.char_h = (image_h / font.rows);
	//font.char_h = 15;

	font.char_w = (image_w / font.columns);
	//font.char_w = 13;

	LOG("Successfully loaded BMP font from %s", texture_path);

	return id;
}

void ModuleFonts::UnLoad(int font_id)
{
	LOG("Freeing a font texture\n");


	if(font_id >= 0 && font_id < MAX_FONTS && fonts[font_id].texture != nullptr)
	{
		app->tex->UnLoad(fonts[font_id].texture);
		fonts[font_id].texture = nullptr;
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
}

void ModuleFonts::BlitText(int x, int y, int font_id, const char* text) const
{
	if(text == nullptr || font_id < 0 || font_id >= MAX_FONTS || fonts[font_id].texture == nullptr)
	{
		LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}

	const Font* font = &fonts[font_id];
	SDL_Rect spriteRect;
	uint len = strlen(text);

	spriteRect.w = font->char_w;
	spriteRect.h = font->char_h;
	spriteRect.x = spriteRect.y = 0;
	int columna = 0;



	for (uint i = 0; i < len; ++i)
	{
		uint charIndex = 0;

		// TODO 2: Find the character in the table, its position in the texture and then Blit
		// 1 - Find the location of the current character in the lookup table
		for (uint j = 0; j < font->totalLength; j++) {
			if (font->table[j] == text[i]) {
				charIndex = j;
				break;
			}
		}
		// Retrieve the position of the current character in the sprite
		spriteRect.x = spriteRect.w * (charIndex % font->columns);
		spriteRect.y = spriteRect.h * (charIndex / font->columns);

		app->render->DrawTexture(font->texture, x, y, &spriteRect);
		// 4 - Advance the position where we blit the next character
		x += spriteRect.w + 2; 

	}
	/*
			// TODO 2: Find the character in the table, its position in the texture and then Blit
		for (int j = 0; j < font->totalLength; j++) {
			// 1 - Find the location of the current character in the lookup table
			if (font->table[j] == text[i]) {
				columna = SDL_floor(j / font->columns);
				spriteRect.y = spriteRect.h * columna;
				// 2 - Retrieve the position of the current character in the sprite
				spriteRect.x = spriteRect.w * (j-(font->columns * columna));
				// 3 - Blit the character at its proper position
				App->render->Blit(font->texture, x, y, &spriteRect, 0, false);
			}
		}
		// 4 - Advance the position where we blit the next character
		x += spriteRect.w + 2;  
	 */

}

