#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>
#include "j1Collision.h"
#include "j1Window.h"
#include "Brofiler/Brofiler.h"


j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());
	redCollision = config.child("collision1").attribute("red").as_int();
	yellowCollision = config.child("collision1").attribute("yellow").as_int();
	magentaCollision = config.child("collision1").attribute("magenta").as_int();
	greenCollison = config.child("collision1").attribute("green").as_int();
	redCollision2 = config.child("collision2").attribute("red").as_int();
	yellowCollision2 = config.child("collision2").attribute("yellow").as_int();
	magentaCollision2 = config.child("collision2").attribute("magenta").as_int();
	greenCollison2 = config.child("collision2").attribute("green").as_int();
	speed[0] = config.child("parallax").attribute("speed").as_float();
	speed[1] = config.child("parallax").attribute("speed2").as_float();
	offset= config.child("offset").attribute("offset").as_int();
	
	paralaxRef[0] = offset;
	paralaxRef[1] = offset;

	return ret;
}

// Called before quitting
bool j1Map::CleanUp()
{
	MapData* MapataItem = &data;
	for(int i=0;i < numberStages; ++i)
	{
		LOG("Unloading map");

		// Remove all tilesets
		p2List_item<TileSet*>* item;
		item = MapataItem->tilesets.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}
		MapataItem->tilesets.clear();

		//// Remove all items from lists

		p2List_item<MapLayer*>* item2;
		item2 = MapataItem->layers.start;

		p2List_item<p2SString*>* itemproperties;
		itemproperties = item2->data->properties.name.start;

		while (item2 != NULL)
		{
			while (itemproperties != NULL)
			{

				RELEASE(itemproperties->data);
				if (itemproperties == item2->data->properties.name.end)
					break;
				itemproperties = itemproperties->next;
			}
			item2->data->properties.name.clear();

			itemproperties = item2->data->properties.value.start;

			while (itemproperties != NULL)
			{
				RELEASE(itemproperties->data);
				if (itemproperties == item2->data->properties.value.end)
					break;
				itemproperties = itemproperties->next;
			}
			item2->data->properties.value.clear();

			RELEASE(item2->data);
			item2 = item2->next;
		}
		MapataItem->layers.clear();

		// Remove all Pralax image
		p2List_item<ImageLayer*>* item3;
		item3 = MapataItem->paralaxlist.start;

		while (item3 != NULL)
		{
			RELEASE(item3->data);
			item3 = item3->next;
		}
		MapataItem->paralaxlist.clear();


		// Clean up the pugui tree
		map_file.reset();

		MapataItem = &data2;
	}

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name, MapData& dataRef)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{

		ret = LoadMap(dataRef);
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		dataRef.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			dataRef.layers.add(lay);
	}

	//Load Image info ----------------------------
	pugi::xml_node paralaxNode;
	for (paralaxNode = map_file.child("map").child("imagelayer"); paralaxNode && ret; paralaxNode = paralaxNode.next_sibling("imagelayer"))
	{
		ImageLayer* imageList = new ImageLayer();

		ret = LoadParallax(paralaxNode, imageList);

		if (ret == true)
			dataRef.paralaxlist.add(imageList);
	}


	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", dataRef.width, dataRef.height);
		LOG("tile_width: %d tile_height: %d", dataRef.tile_width, dataRef.tile_height);

		p2List_item<TileSet*>* item = dataRef.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = dataRef.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			LOG("layer has %i properties", l->properties.numproperties);
			for (int i = 0; i < l->properties.numproperties-1; ++i)
			{
				LOG("property name is %s", l->properties.name.At(i)->data->GetString());
				LOG("property value is %s", l->properties.value.At(i)->data->GetString());
			}
			item_layer = item_layer->next;
		}

		p2List_item<ImageLayer*>* item_imageParalax = dataRef.paralaxlist.start;
		while (item_imageParalax != NULL)
		{
			ImageLayer* i = item_imageParalax->data;
			LOG("Paralax image ----");
			LOG("name: %s", i->name.GetString());
			LOG("tile width: %d tile height: %d", i->width, i->height);
			item_imageParalax = item_imageParalax->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap(MapData& data)
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		//Loading properties
		data.properties.LoadProperties(map);

		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());



		//loading gameobjects
		pugi::xml_node mapIterator;
		for (mapIterator = map.child("objectgroup"); mapIterator; mapIterator = mapIterator.next_sibling("objectgroup"))
		{
			p2SString tmp(mapIterator.first_attribute().as_string());
			if (tmp == "initpos")
			{
				data.initpos.x = mapIterator.child("object").attribute("x").as_int();
				data.initpos.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "finalpos")
			{
				data.finalpos.x = mapIterator.child("object").attribute("x").as_int();
				data.finalpos.y = mapIterator.child("object").attribute("y").as_int() + mapIterator.child("object").attribute("height").as_int();
			}
			else if (tmp == "slime1")
			{
				data.slime1.x = mapIterator.child("object").attribute("x").as_int();
				data.slime1.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "slime2")
			{
				data.slime2.x = mapIterator.child("object").attribute("x").as_int();
				data.slime2.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "bat1")
			{
				data.bat1.x = mapIterator.child("object").attribute("x").as_int();
				data.bat1.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "bat2")
			{
				data.bat2.x = mapIterator.child("object").attribute("x").as_int(); 
				data.bat2.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "checkpoint1")
			{
				data.checkpoint1.x = mapIterator.child("object").attribute("x").as_int();
				data.checkpoint1.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "checkpoint2")
			{
				data.checkpoint2.x = mapIterator.child("object").attribute("x").as_int();
				data.checkpoint2.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "checkpoint3")
			{
				data.checkpoint3.x = mapIterator.child("object").attribute("x").as_int();
				data.checkpoint3.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "orb1")
			{
				data.orb.x = mapIterator.child("object").attribute("x").as_int();
				data.orb.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "orb2")
			{
				data.orb2.x = mapIterator.child("object").attribute("x").as_int();
				data.orb2.y = mapIterator.child("object").attribute("y").as_int();
			}
			else if (tmp == "orb3")
			{
				data.orb3.x = mapIterator.child("object").attribute("x").as_int();
				data.orb3.y = mapIterator.child("object").attribute("y").as_int();
			}
		}
		

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}



	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	//set->redCollision=tileset_node.attribute("")
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	layer->properties.LoadProperties(node);

	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool Properties::LoadProperties(pugi::xml_node& node)
{
	// node has to be layer, map, tileset
	bool ret = true;

	pugi::xml_node properties = node.child("properties");

	numproperties = properties.first_child().attribute("value").as_int();

	//We get the second property node

	properties = properties.first_child().next_sibling();
	
	

	for (int i = 0; i < numproperties && properties; ++i)
	
	{
			p2SString* tmp = new p2SString();
			p2SString* tmp2 = new p2SString();

		tmp->operator=(properties.attribute("name").as_string());
		this->name.add(tmp);
		tmp2->operator=(properties.attribute("value").as_string());
		this->value.add(tmp2);

		if(i!= numproperties-1)
		properties = properties.next_sibling();
	}
	// fill the custom properties from an xml_node

	return ret;
}

bool j1Map::LoadParallax(pugi::xml_node& node, ImageLayer* image)
{
	bool ret = true;

	image->name = node.attribute("name").as_string();
	image->width = node.child("image").attribute("width").as_int();
	image->height = node.child("image").attribute("height").as_int();
	image->texture = App->tex->Load(PATH(folder.GetString(), node.child("image").attribute("source").as_string()));

	return ret;
}

bool j1Map::ColliderDrawer(MapData& data)
{
	bool ret = true;

	MapLayer* layer;

	for (uint l = 0; l < data.layers.count(); l++)
	{
		layer = data.layers.At(l)->data;

		if (layer->properties.GetProperties("Nodraw").operator==("1"))
		{

			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int tile_id = layer->Get(x, y);

					if (tile_id > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id, data);

						if (tile_id > tileset->firstgid)
						{

							iPoint pos = MapToWorld(x, y, data);

							if (tile_id == redCollision || tile_id== redCollision2)
								App->coll->AddCollider({ pos.x,pos.y,data.tile_width,data.tile_height }, COLLIDER_TYPE::COLLIDER_FLOOR,this);

							else if (tile_id == yellowCollision || tile_id == yellowCollision2)
								App->coll->AddCollider({ pos.x,pos.y,data.tile_width,data.tile_height }, COLLIDER_TYPE::COLLIDER_SPIKES,this);

							else if (tile_id == magentaCollision || tile_id == magentaCollision2)
								App->coll->AddCollider({ pos.x,pos.y,data.tile_width,data.tile_height }, COLLIDER_TYPE::COLLIDER_PLATFORM,this);

							else if (tile_id == greenCollison || tile_id == greenCollison2)
								App->coll->AddCollider({ pos.x,pos.y,data.tile_width,data.tile_height }, COLLIDER_TYPE::COLLIDER_ROOF, this);

						}
					}
				}
			}
		}
	}



	return ret;
}

void j1Map::Draw(MapData &data)
{
	BROFILER_CATEGORY("Map_Draw", Profiler::Color::Bisque);

	if (map_loaded == false)
		return;

	for (int x = 0; x < data.paralaxlist.count(); ++x)
	{
		App->render->Blit(data.paralaxlist[x]->texture,
			paralaxRef[x],
			0,
			&data.paralaxlist[x]->GetParalaxRect());
	}


	//draw all the layers and not just the first one
	MapLayer* layer;


	for (uint l = 0; l < data.layers.count(); l++)
	{
		layer = data.layers.At(l)->data;

		if (layer->properties.GetProperties("Nodraw").operator==("1"))
			continue;

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id,data);

					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);

						iPoint pos = MapToWorld(x, y, data);


						if ((pos.x + data.tile_width)*App->win->GetScale() >= -App->render->camera.x && pos.x <= -App->render->camera.x + App->render->camera.w
							&& (pos.y + data.tile_height)*App->win->GetScale() >= -App->render->camera.y && pos.y <= -App->render->camera.y + App->render->camera.h)
						{
							App->render->Blit(tileset->texture, pos.x, pos.y, &r);
						}
					}
				}
			}
		}
	}

}

iPoint j1Map::MapToWorld(int x, int y ,MapData& Data) const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y, MapData& Data) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2);
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}


TileSet* j1Map::GetTilesetFromTileId(int id,MapData& data) const
{
	//method so we pick the right Tileset based on a tile id

	TileSet* tileset;

	int j = data.tilesets.count();

	for (int i = 0; i < j; ++i)
	{
		tileset = data.tilesets.At(i)->data;

		if (tileset == NULL)
		{
			LOG("Problem in tileset get");
		}

		if (id >= tileset->firstgid)
		{
			if (data.tilesets.At(i + 1) != NULL)
			{

				if (id <= data.tilesets.At(i + 1)->data->firstgid)
				{
					if (tileset == NULL)
					{
						LOG("Problem in tileset get");
					}
					return tileset;
				}
				else
					continue;
			}
			return tileset;
		}
	}

	LOG("No tileset matches tile_id");
}

SDL_Rect TileSet::GetTileRect(int id) const
{

	int relative_id = id - firstgid; 

	SDL_Rect rect;

	rect.w = tile_width;
	rect.h = tile_height;

	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));


	return rect;
}

SDL_Rect ImageLayer::GetParalaxRect() const
{
	SDL_Rect rect;

	rect.w = width;
	rect.h = height;

	rect.x = 0;
	rect.y = 0;


	return rect;
}

p2SString Properties::GetProperties(const char * request)
{
	p2SString tmp;
	bool requestfound = false;
	pugi::xml_parse_result result;


		for (int i = 0; i < name.count(); ++i)
		{
			tmp.operator= (name.At(i)->data->GetString());
			if (tmp.operator==(request) == true)
			{
				requestfound = true;
				tmp = value.At(i)->data->GetString();
				break;
			}
			
		}

		if (requestfound == false)
			tmp = result.description();


	return tmp;
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer, MapData& Data) const
{
	bool ret = false;
	p2List_item<MapLayer*>* item;
	//item = Data.layers.start;

	for (item = Data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.GetProperties("Navigation") == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < Data.height; ++y)
		{
			for (int x = 0; x < Data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id,Data) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
					map[i] = ts->properties.Get("walkable", 1);
					}*/
				}
			}
		}

		*buffer = map;
		width = Data.width;
		height = Data.height;
		ret = true;

		break;
	}

	return ret;
}

