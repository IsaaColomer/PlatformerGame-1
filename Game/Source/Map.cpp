
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Input.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    folder.Create(config.child("folder").child_value());

    return ret;
}

// Draw the map (all requried layers)
void Map::Draw()
{
	if (mapLoaded == false) return;

	// L04: DONE 5: Prepare the loop to draw all tilesets + DrawTexture()
	MapLayer* layer = data.layers.start->data;

	for (int y = 0; y < data.height; ++y)
	{
		for (int x = 0; x < data.width; ++x)
		{
			int tileId = layer->Get(x, y);
			if (tileId > 0)
			{
				// L04: TODO 9: Complete the draw function
				iPoint vec = MapToWorld(x, y);
				app->render->DrawTexture(data.tilesets.At(0)->data->texture, vec.x, vec.y, &data.tilesets.At(0)->data->GetTileRect(tileId));

			}
		}
	}
}

// L04: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	//ret.x = x * data.tileWidth;
	//ret.y = y * data.tileHeight;

	// L05: TODO 1: Add isometric map to world coordinates
	ret.x = (x - y) * (data.tileWidth * 0.5f)- data.tileWidth * 0.5f;
	ret.y = (x + y) * (data.tileHeight * 0.5f);
	return ret;
}

// L05: TODO 2: Add orthographic world to map coordinates
iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);
	/*if (x > 0 && y > 0 && x < ((data.tileWidth*data.width)-2*data.tileWidth) && y < (data.tileHeight * data.height))
	{
		ret.x = x / data.tileWidth;
		ret.y = y / data.tileHeight;
	}*/
	// L05: TODO 3: Add the case for isometric maps to WorldToMap
	ret.x = (x / data.tileWidth  + y / data.tileHeight ) ;
	ret.y = (y / data.tileHeight  - (x / data.tileWidth ));
	return ret;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect rect = { 0 };

	// L04: DONE 7: Get relative Tile rectangle
	int relativeId = id - firstgid;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + spacing) * (relativeId % numTilesWidth));
	rect.y = margin + ((rect.h + spacing) * (relativeId / numTilesWidth));
	
	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L03: DONE 2: Make sure you clean up any memory allocated from tilesets/map
    // Remove all tilesets
	ListItem<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// L04: DONE 2: clean up all layer data
	// Remove all layers
	ListItem<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	// Clean up the pugui tree
	mapFile.reset();

    return true;
}

// Load new map
bool Map::Load(const char* filename)
{
    bool ret = true;
    SString tmp("%s%s", folder.GetString(), filename);

    pugi::xml_parse_result result = mapFile.load_file(tmp.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", filename, result.description());
        ret = false;
    }

	// Load general info
    if(ret == true)
    {
        // L03: DONE 3: Create and call a private function to load and fill all your map data
		ret = LoadMap();
	}

    // L03: DONE 4: Create and call a private function to load a tileset
    // remember to support more any number of tilesets!
	pugi::xml_node tileset;
	for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true) ret = LoadTilesetDetails(tileset, set);

		if (ret == true) ret = LoadTilesetImage(tileset, set);

		data.tilesets.add(set);
	}

	// L04: DONE 4: Iterate all layers and load each of them
	// Load layer info
	pugi::xml_node layer;
	for (layer = mapFile.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.add(lay);
	}
    
    if(ret == true)
    {
        // L03: TODO 5: LOG all the data loaded iterate all tilesets and LOG everything
		LOG("Successfully parsed map XML file: %s", filename);
		LOG("Width: %d	Hight: %d", data.width, data.height);
		LOG("TileWidth: %d	TileHight: %d", data.tileWidth, data.tileHeight);
		for (int i = 0; i < data.tilesets.count(); i++)
		{
			LOG("TileSet ----");
			LOG("Name: %s	FirstGid: %d", data.tilesets.At(i)->data->name.GetString(), data.tilesets.At(i)->data->firstgid);
			LOG("Tile width: %d", data.tilesets.At(i)->data->tileWidth);
			LOG("Tile Height: %d", data.tilesets.At(i)->data->tileHeight);
			LOG("Spacing: %d", data.tilesets.At(i)->data->spacing);
			LOG("Margin: %d", data.tilesets.At(i)->data->margin);
			LOG("NumTilesWidth: %d", data.tilesets.At(i)->data->numTilesWidth);
			LOG("NumTilesHeight: %d", data.tilesets.At(i)->data->numTilesHeight);
		}


		// L04: TODO 4: LOG the info for each loaded layer
		for (int i = 0; i < data.layers.count(); i++)
		{
			LOG("Layer ----");
			LOG("Name: %s", data.layers.At(i)->data->name.GetString());
			LOG("Tile width: %d", data.layers.At(i)->data->width);
			LOG("Tile Height: %d", data.layers.At(i)->data->height);
		}
    }

    mapLoaded = ret;

    return ret;
}

// L03: TODO: Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = mapFile.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		// L03: TODO: Load map general properties
		data.width = map.attribute("width").as_int(0);
		data.height = map.attribute("height").as_int(0);
		data.tileWidth = map.attribute("tilewidth").as_int(0);
		data.tileHeight = map.attribute("tileheight").as_int(0);
	}

	return ret;
}

// L03: TODO: Load Tileset attributes
bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	
	// L03: TODO: Load Tileset attributes
	set->name = tileset_node.attribute("name").as_string("");
	set->firstgid = tileset_node.attribute("firstgid").as_int(0);
	set->tileWidth = tileset_node.attribute("tilewidth").as_int(0);
	set->tileHeight= tileset_node.attribute("tileheight").as_int(0);
	set->spacing = tileset_node.attribute("spacing").as_int(0);
	set->margin = tileset_node.attribute("margin").as_int(0);
	set->numTilesWidth = tileset_node.attribute("numTilesWidth").as_int(4);
	set->numTilesHeight = tileset_node.attribute("numTilesHeight").as_int(6);
	return ret;
}

// L03: TODO: Load Tileset image
bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		// L03: TODO: Load Tileset image
		set->texture = app->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string("")));
		set->texWidth = image.attribute("width").as_int(0);
		set->texHeight = image.attribute("height").as_int(0);
	}

	return ret;
}

// L04: TODO 3: Create the definition for a function that loads a single layer
bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	// L04: TODO 3: Load a single layer

	bool ret = true;
	layer->name = node.attribute("name").as_string("");
	layer->width = node.attribute("width").as_int(0);
	layer->height = node.attribute("height").as_int(0);
	int size = layer->width * layer->height;
	layer->data = new uint[size];
	pugi::xml_node tile = node.child("data").child("tile");
	for (int i = 0; i < size; i++)
	{
		layer->data[i] = tile.attribute("gid").as_uint(0);
		tile = tile.next_sibling("tile");
	}

	return ret;
}
