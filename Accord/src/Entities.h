#pragma once

#include "Cockroach.h"
#include "Components.h"

#include "Game.h"

using namespace Cockroach;

namespace Entities
{
	enum EntityType
	{
		Payga, Camera, SpikeLeft, SpikeRight, SpikeDown, SpikeUp
	};

	Entity* Create(int2 position, int entityType);

}