#pragma once

#include "Cockroach.h"

class EditorCursor
{
public:
	enum BrushMode { Tile, Entity, Decoration, Room };

	static void Update(float dt);
	static void Render();

	static int2 WorldPosition();

	static BrushMode brushMode;
	static Cockroach::Room::TileType tileType;
	static int entityType;
	static int decorationType;
	static bool isBoxPlacing;
	static int2 boxPlaceStartPos;
};