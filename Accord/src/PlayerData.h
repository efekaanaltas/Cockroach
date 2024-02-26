#pragma once

#include "Cockroach.h"
using namespace Cockroach;

struct PlayerData
{
	int2 playerPosition = ZERO;
	bool fullscreen = true;
	bool mutedMaster = false;
	bool mutedSFX = false;
	bool mutedMusic = false;

	u16 actionKeys[6];

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