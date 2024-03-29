#pragma once

#include <string>
#include "Game/Entity.h"
#include "Game/Rect.h"

namespace Cockroach
{
	class Room
	{
	public:
		enum TileType : char
		{
			Air = '0', TileBasic = 'B', BackgroundBasic = 'C'
		};

		static Ref<Room> current;
		inline static const std::string roomDir = "assets/rooms/";
		inline static const int2 invalidUV = { -1, -1 };

		Room(std::string name, int width, int height, int posX, int posY);

		std::string name;
		int width, height;
		int2 position = { 0,0 };
		TileType* tiles;
		int2* tileUVs;
		int2* backgroundTileUVs;
		std::vector<Entity*> entities;

		void Reset();
		void Update();
		void Render(Ref<Texture2D> tilemapTexture, Rect cameraBounds);

		void AddEntity(Entity* entity);
		void RemoveEntity(Entity* entity);

		void PlaceTile(TileType tileType, int2 worldPosition);
		void PlaceTileBox(TileType tileType, int2 worldPositionMin, int2 worldPositionMax);
		void Resize(int newWidth, int newHeight);
		void UpdateTileUV(int x, int y);
		void UpdateTileUVAll();

		bool IsFilled(int x, int y, TileType type);

		std::string Filepath() { return roomDir + name; }
		bool CollidesWith(Rect rect, int xForesense, int yForesense);
		bool OverlapsWith(Rect rect, int xForesense, int yForesense);
		bool Contains(int2 roomPosition);
		bool Contains(Rect rect);

		Rect Bounds() { return Rect(RoomToWorldPosition({ 0,0 }), RoomToWorldPosition({ width, height })); }
		int RoomPositionToIndex(int x, int y) { return y * width + x; }
		int2 IndexToRoomPosition(int index) { return { index % width, index / width }; }
		int2 CenterPoint() { return position + int2(width*4 - 4, height*4 - 4); }
		int2 WorldToRoomPosition(int2 worldPos) { return glm::floor((float2)(worldPos - 8*position) / 8.0f); }
		int2 RoomToWorldPosition(int2 roomPos) { return position * 8 + roomPos * 8; }

		void Save();
		static Ref<Room> Load(const std::string& name);
		void Rename(const std::string& newName);
	};
}