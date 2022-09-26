#include "crpch.h"
#include "Room.h"

#include <fstream>
#include "Renderer/Renderer.h"

namespace Cockroach
{
	Ref<Room> Room::current = nullptr;

	Room::Room(std::string name, int width, int height, int posX, int posY)
		: name(name), width(width), height(height)
	{
		position = { posX, posY };
		tiles = new Tile[width * height];
	}

	void Room::Update(float dt)
	{
		for (int i = 0; i < entities.size(); i++)
			entities[i]->Update(dt);
	}

	void Room::Render(Ref<Texture2D> tilemapTexture)
	{
		for (int i = 0; i < width * height; i++)
		{
			if (tiles[i].type == Air) continue;
			Sprite sprite = Sprite::CreateFromCoords(tilemapTexture, tiles[i].texCoordOffset, {8,8});
			int2 roomPos = IndexToRoomPosition(i);
			Renderer::DrawQuad(RoomToWorldPosition(roomPos), {8,8}, sprite);
		}
		for (int i = 0; i < entities.size(); i++)
			entities[i]->Render();
	}

	void Room::AddEntity(Entity* entity)
	{
		entities.push_back(entity);
	}

	void Room::RemoveEntity(Entity* entity)
	{
		for (int i = 0; i < entities.size(); i++)
			if (entities[i]->ID == entity->ID)
				entities.erase(entities.begin() + i);
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

			Save();
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

		Save();
	}

	void Room::Resize(int newWidth, int newHeight)
	{
		Tile* newTiles = new Tile[newWidth * newHeight];

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int oldIndex = RoomPositionToIndex(x, y);
				int newIndex = y * newWidth + x;
				if(0 <= x && 0 <= y && x < newWidth && x < width && y < newHeight && y < height)
					newTiles[newIndex] = tiles[oldIndex];
			}
		}

		delete[] tiles;
		tiles = newTiles;

		width = newWidth;
		height = newHeight;

		//Save();
	}

	void Room::UpdateTile(int x, int y)
	{
		TileType type = tiles[RoomPositionToIndex(x, y)].type;

		if (!IsFilled(x, y, type)) return;

		int rightLeft = (IsFilled(x + 1, y, type) << 1) | (int)IsFilled(x - 1, y, type);
		int downUp =	(IsFilled(x, y - 1, type) << 1) | (int)IsFilled(x, y + 1, type);

		if (rightLeft == 3) rightLeft = 2;
		else if (rightLeft == 2) rightLeft = 3;
		if (downUp == 3) downUp = 2;
		else if (downUp == 2) downUp = 3;

		int xStart = type == TileBasic ? 11 : 19;

		tiles[RoomPositionToIndex(x, y)].texCoordOffset = { xStart-rightLeft, 2+downUp };
	}

	bool Room::IsFilled(int x, int y, TileType type)
	{
		int index = RoomPositionToIndex(x, y);
		if (!Contains({x, y})) return true; // Little hack to make autotiling on borders easier
		return tiles[index].type == type;
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
				if (Contains({x, y}) && tiles[index].type == TileBasic) // Do not use IsFilled() as it is intended for autotiling and not collisions
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
		return (0 <= roomPosition.x && roomPosition.x < width && 0 <= roomPosition.y && roomPosition.y < height && 0 <= index && index < width * height);
	}

	bool Room::Contains(Rect rect)
	{
		return Contains(WorldToRoomPosition(rect.min)) && Contains(WorldToRoomPosition(rect.max));
	}

	void Room::Save()
	{
		std::fstream out(Filepath(), std::ios::out | std::ios::binary | std::ios::trunc);

		if (out)
		{
			out << width << ' ';
			out << height << ' ';
			out << position.x << ' ';
			out << position.y << ' ';
			for (int i = 0; i < width * height; i++)
				out << tiles[i].type;
			for (int i = 0; i < entities.size(); i++)
			{
				out << '\n';
				out << "E: " << entities[i]->type << ", ";
				out << "X: " << entities[i]->position.x << ", ";
				out << "Y: " << entities[i]->position.y;
			}
		}
		else
			CR_CORE_ERROR("Could not open file '{0}'", Filepath());
		out.close();
	}

	Ref<Room> Room::Load(const std::string& name)
	{
		Ref<Room> room;
		std::string filepath = roomDir + name;
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

			room = CreateRef<Room>(name, width, height, posX, posY);

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

				room->AddEntity(CreateEntity(int2(pX, pY), type));
			}
		}
		else
			CR_CORE_ERROR("Could not open file '{0}'", filepath);
		in.close();
		
		return room;
	}

	void Room::Rename(const std::string& newName)
	{
		if (name == newName)
		{
			CR_CORE_ERROR("New name is same as old name.");
			return;
		}

		if (rename(Filepath().c_str(), (roomDir + newName).c_str()))
			name = newName;
		else
			CR_CORE_ERROR("Room could not be renamed from {0} to {1}.", Filepath(), (roomDir + newName));
	}
}