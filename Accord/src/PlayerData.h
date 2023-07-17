#pragma once

#include "Cockroach.h"
using namespace Cockroach;

struct PlayerData
{
	int2 playerPosition = ZERO;
	bool fullscreen = true;
	bool muted = false;

	PlayerData()
	{
		Load();
	}

	void Save();
	void Load();
};

enum PropertyName
{
	SaveSlot, PlayerPosX, PlayerPosY
};