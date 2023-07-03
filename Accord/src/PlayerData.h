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
	PlayerData Load();
};

enum PropertyName
{
	SaveSlot, PlayerPosX, PlayerPosY
};