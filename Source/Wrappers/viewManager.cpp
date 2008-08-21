#include "..\Helpers\datatypes.h"
#include "viewManager.h"
#include "renderer.h"
#include "..\Objects\camera.h"

#pragma region constructor/destructor
viewManager::viewManager(void)
{
	theRenderer = NULL;
}

viewManager::~viewManager(void)
{
}
#pragma endregion

viewManager* viewManager::getInstance()
{
	static viewManager Picasso;

	return &Picasso;
}

#pragma region init/shutdown
void viewManager::initialize(renderer* aRenderer)
{
	theRenderer = aRenderer;
}

void viewManager::shutdown()
{
	releaseAllTextures();
	releaseAllMeshes();
}
#pragma endregion

#pragma region load functions
int viewManager::loadTexture(const char filename[], uint colorKey)
{
	if(!filename)
		return -1;

	for(unsigned c = 0; c < textureList.size(); c++)
	{
		if(textureList[c] && strcmp(filename, textureList[c]->filename) == 0)
		{
			if(!textureList[c]->ref)
				theRenderer->LoadTexture(textureList[c], filename, colorKey);
			textureList[c]->ref++;
			return c;
		}
	}

	int id = -1;
	//Check to see if there's any not being used
	for(unsigned c = 0; c < textureList.size(); c++)
	{
		if(textureList[c] == NULL)
		{
			id = c;
			break;
		}
	}
	textureData* holder = new textureData;
	theRenderer->LoadTexture(holder, filename, colorKey);
	holder->ref = 1;
	if(-1 == id) //No open spaces
	{
		textureList.push_back(holder);
		return int(textureList.size() - 1);
	}
	else
		textureList[id] = holder;

	return id;
}

int viewManager::loadMesh(char filename[])
{
	return -1;
}
#pragma endregion

#pragma region release functions
void viewManager::releaseTexture(int textureID)
{
	textureList[textureID]->ref--;

	if(0 == textureList[textureID]->ref && textureList[textureID]->texture)
	{
		theRenderer->ReleaseTexture(textureList[textureID]->texture);
		delete textureList[textureID];
		textureList[textureID] = NULL;
	}
}

void viewManager::releaseMesh(int meshID)
{
}

void viewManager::releaseAllTextures()
{
	for(unsigned c = 0; c < textureList.size(); c++)
	{
		if(textureList[c])
		{
			if(textureList[c]->texture)
			{
				theRenderer->ReleaseTexture(textureList[c]->texture);
				textureList[c]->texture = NULL;
			}
			delete textureList[c];
		}
	}

	textureList.clear();
}

void viewManager::releaseAllMeshes()
{
}
#pragma endregion

#pragma region draw
void viewManager::drawTexture(int id, const vector3* pos, const matrix * transform, const rect* section,
		const vector3* center, uint color)
{
	theRenderer->RenderSprite(textureList[id]->texture, pos, transform, section, center, color);
}
#pragma endregion

pt viewManager::getTextureDimensions(int id) const
{
	return pt(textureList[id]->width, textureList[id]->height);
}