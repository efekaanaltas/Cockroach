#pragma once

#include "Cockroach.h"
using namespace Cockroach;

struct PlayerData
{
	PlayerData()
	{
		Load();
	}

	int2 playerPosition = ZERO;

	void Save();
	void Load();
};

enum PropertyName
{
	SaveSlot, PlayerPosX, PlayerPosY
};