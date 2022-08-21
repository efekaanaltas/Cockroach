#include "crpch.h"
#include "Room.h"

#include <fstream>
#include "Cockroach/Renderer/Renderer.h"

namespace Cockroach
{
	Ref<Room> Room::current = nullptr;

	Room::Room(std::string filepath, int width, int height, int posX, int posY)
		: filepath(filepath), width(width), height(height)
	{
		position = { posX, posY };
		tiles = new Tile[width * height];
		memset(tiles, 0, sizeof(char) * width * height);

		entities = new Entity[3000];
	}

	void Room::Update(float dt)
	{
		for (int i = 0; i < entityCount; i++)
			entities[i].Update(dt);
	}

	void Room::Render(Ref<Texture2D> tilemapTexture)
	{
		for (int i = 0; i < width * height; i++)
		{
			if (tiles[i].type == Air) continue;
			Sprite sprite = Sprite::CreateFromCoords(tilemapTexture, { 11+tiles[i].texCoordOffset.x,2+tiles[i].texCoordOffset.y}, {8,8});
			int2 roomPos = IndexToRoomPosition(i);
			Renderer::DrawQuad(RoomToWorldPosition(roomPos), {8,8}, sprite);
		}
		for (int i = 0; i < entityCount; i++)
		{
			entities[i].Render();
		}
	}

	Entity* Room::AddEntity(int2 position)
	{
		entityCount += 1;
		Entity* e = new Entity(position);
		entities[entityCount] = *e;
		delete e;
		return &entities[entityCount-1];
	}

	void Room::PlaceTile(TileType tileType, int2 worldPosition)
	{
		int2 roomPosition = WorldToRoomPosition(worldPosition);
		int index = RoomPositionToIndex(roomPosition.x, roomPosition.y);

		if (Contains(roomPosition))
		{
			tiles[index].type = tileType;
			tiles[index].texCoordOffset = { 0,0 };

			UpdateTile(roomPosition.x, roomPosition.y);
			UpdateTile(roomPosition.x + 1, roomPosition.y);
			UpdateTile(roomPosition.x - 1, roomPosition.y);
			UpdateTile(roomPosition.x, roomPosition.y - 1);
			UpdateTile(roomPosition.x, roomPosition.y + 1);

			Save(filepath);
		}

	}

	void Room::PlaceTileBox(TileType tileType, int2 worldPositionMin, int2 worldPositionMax)
	{
		if (std::abs(worldPositionMin.x - worldPositionMax.x) > 400 || std::abs(worldPositionMin.y - worldPositionMax.y) > 400)
		{
			CR_CORE_WARN("Placement box too big!");
			return;
		}

		for (int y = worldPositionMin.y; y <= worldPositionMax.y; y++)
			for (int x = worldPositionMin.x; x <= worldPositionMax.x; x++)
			{
				int2 roomPosition = WorldToRoomPosition({ x,y });
				int index = RoomPositionToIndex(roomPosition.x, roomPosition.y);

				if (Contains(roomPosition))
				{
					tiles[index].type = tileType;

					UpdateTile(roomPosition.x, roomPosition.y);
					UpdateTile(roomPosition.x + 1, roomPosition.y);
					UpdateTile(roomPosition.x - 1, roomPosition.y);
					UpdateTile(roomPosition.x, roomPosition.y - 1);
					UpdateTile(roomPosition.x, roomPosition.y + 1);
				}
			}

		Save(filepath);
	}

	void Room::UpdateTile(int x, int y)
	{
		if (!IsFilled(x, y)) return;

		int rightLeft = (IsFilled(x + 1, y) << 1) | (int)IsFilled(x - 1, y);
		int downUp =	(IsFilled(x, y - 1) << 1) | (int)IsFilled(x, y + 1);

		if (rightLeft == 3) rightLeft = 2;
		else if (rightLeft == 2) rightLeft = 3;
		if (downUp == 3) downUp = 2;
		else if (downUp == 2) downUp = 3;

		tiles[RoomPositionToIndex(x, y)].texCoordOffset = { -rightLeft, downUp };
	}

	bool Room::IsFilled(int x, int y)
	{
		int index = RoomPositionToIndex(x, y);
		if (!Contains({x, y})) return true; // Little hack to make autotiling on borders easier
		return tiles[index].type != Air;
	}

	bool Room::CollidesWith(Rect rect, int xForesense, int yForesense)
	{
		int2 minRoomPos = WorldToRoomPosition({ rect.min.x + xForesense, rect.min.y + yForesense });
		int2 maxRoomPos = WorldToRoomPosition({ rect.max.x + xForesense - 1, rect.max.y - 1 + yForesense }); // Do not sample next tile (-1)

		for (int y = minRoomPos.y; y <= maxRoomPos.y; y++)
		{
			for (int x = minRoomPos.x; x <= maxRoomPos.x; x++)
			{
				int index = RoomPositionToIndex(x, y);
				if (Contains({x, y}) && tiles[index].type != Air) // Do not use IsFilled() as it is intended for autotiling and not collisions
					return true;
			}
		}
		return false;
	}

	bool Room::OverlapsWith(Rect rect, int xForesense, int yForesense)
	{
		Rect roomRect = Rect(RoomToWorldPosition({0,0}), RoomToWorldPosition({width, height}));
		return roomRect.OverlapsWith(rect, xForesense, yForesense);
	}

	bool Room::Contains(int2 roomPosition)
	{
		int index = RoomPositionToIndex(roomPosition.x, roomPosition.y);
		return (0 <= roomPosition.x && roomPosition.x < width && 0 <= roomPosition.y && roomPosition.y < height && 0 <= index && index < width* height);
	}

	bool Room::Contains(Rect rect)
	{
		return Contains(WorldToRoomPosition(rect.min)) && Contains(WorldToRoomPosition(rect.max));
	}

	void Room::Save(const std::string& filepath)
	{
		std::fstream out(filepath, std::ios::out | std::ios::binary | std::ios::trunc);

		if (out)
		{
			out << width << ' ';
			out << height << ' ';
			out << position.x << ' ';
			out << position.y << ' ';
			for (int i = 0; i < width * height; i++)
				out << tiles[i].type;
			for (int i = 0; i < entityCount; i++)
			{
				out << '\n';
				out << "E: " << entities[i].type << ", ";
				out << "X: " << entities[i].position.x << ", ";
				out << "Y: " << entities[i].position.y;
			}
		}
		else
			CR_CORE_ERROR("Could not open file '{0}'", filepath);
		out.close();
	}

	Ref<Room> Room::Load(const std::string& filepath, std::function<Entity*(int2, int)> entityCreateFn)
	{
		Ref<Room> room;
		std::fstream in(filepath, std::ios::in ||std::ios::binary);

		if (in)
		{
			std::string line;
			std::getline(in, line);
			std::stringstream stream(line);

			int width, height, posX, posY;
			stream >> width;
			stream >> height;
			stream >> posX;
			stream >> posY;

			char* data = new char[width*height];
			stream >> data;

			room = CreateRef<Room>(filepath, width, height, posX, posY);
			if (Room::current == nullptr)
				Room::current = room; // bruh

			for (int i = 0; i < room->width * room->height; i++)
				room->tiles[i].type = (TileType)data[i];
			for (int y = 0; y < room->height; y++)
				for (int x = 0; x < room->width; x++)
					room->UpdateTile(x, y);

			while (std::getline(in, line))
			{
				std::stringstream stream(line);

				int type = 0, pX = 0, pY = 0;
				stream.seekg(line.find("E:") + 2);
				stream >> type;
				stream.seekg(line.find("X:") + 2);
				stream >> pX;
				stream.seekg(line.find("Y:") + 2);
				stream >> pY;

				room->entities[room->entityCount] = *entityCreateFn(int2(pX, pY), type);
			}
		}
		else
			CR_CORE_ERROR("Could not open file '{0}'", filepath);
		in.close();
		
		return room;
	}
}