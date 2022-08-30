#pragma once

#include "Cockroach.h"

class EditorCursor
{
public:
	enum BrushMode { Tile, Entity, Room };

	static void Update(float dt);
	static void Render();

	static int2 WorldPosition();

	static BrushMode brushMode;
	static int entityType;
	static bool isBoxPlacing;
	static int2 boxPlaceStartPos;
};