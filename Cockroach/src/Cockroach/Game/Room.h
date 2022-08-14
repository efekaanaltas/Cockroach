#pragma once

#include <string>
#include "Cockroach/Game/Entity.h"
#include "Rect.h"

namespace Cockroach
{
	class Room
	{
	public:
		enum TileType : char
		{
			Air = '0', TileBasic = 'B'
		};

		struct Tile
		{
			TileType type;
			int2 texCoordOffset;
		};

		static Ref<Room> current;

		static const int2 tileTexCoordLUT[4][4];

		Room(int width, int height);

		int width, height;
		int2 position = { 0,0 };
		Tile* tiles;
		Entity* entities;

		int entityCount = 0;

		void Update(float dt);
		void Render(Ref<Texture2D> tilemapTexture);

		Entity* AddEntity(int2 position);

		void PlaceTile(TileType tileType, int2 worldPosition);
		void PlaceTileBox(TileType tileType, int2 worldPositionMin, int2 worldPositionMax);
		void UpdateTile(int x, int y);

		bool IsFilled(int x, int y);

		bool CollidesWith(Rect rect, int xForesense, int yForesense);
		bool Contains(int2 roomPosition);
		int RoomPositionToIndex(int x, int y) { return y * width + x; }
		int2 IndexToRoomPosition(int index) { return { index % width, index / width }; }
		int2 CenterPoint() { return position + int2(width*4 - 4, height*4 - 4); }
		int2 WorldToRoomPosition(int2 worldPos) { return (worldPos - position) / 8; }
		int2 RoomToWorldPosition(int2 roomPos) { return position + roomPos * 8; }

		void Save(const std::string& filepath);
		static Ref<Room> Load(const std::string& filepath, std::function<Entity* (int2, int)> entityCreateFn);
	};
}