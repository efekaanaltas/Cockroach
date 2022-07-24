#include "crpch.h"
#include "Room.h"

#include <fstream>
#include "Cockroach/Renderer/Renderer.h"

namespace Cockroach
{
	Ref<Room> Room::current = nullptr;

	Room::Room(int width, int height)
		: width(width), height(height)
	{
		tiles = new Tile[width * height];
		memset(tiles, 0, sizeof(char) * width * height);

		entities = new Entity[3000];
		Entity* e = new Entity();
		entities[0] = *e;
		delete e;
	}

	void Room::Update(float dt)
	{
		for (int i = 0; i < entityCount; i++)
			entities[i].Update(dt);
	}

	void Room::Render()
	{
		Ref<Texture2D> texture = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");
		for (int i = 0; i < width * height; i++)
		{
			if (tiles[i].type == Air) continue;
			Ref<SubTexture2D> sprite = SubTexture2D::CreateFromCoords(texture, { 11+tiles[i].texCoordOffset.x,2+tiles[i].texCoordOffset.y}, {8,8});
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
		Entity* e = new Entity(position);
		Room::current->entities[0];
		entities[Room::current->entityCount] = *e;
		delete e;
		return &entities[Room::current->entityCount++];
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

			Save("assets/scenes/room1.txt");
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

		Save("assets/scenes/room1.txt");
	}

	void Room::UpdateTile(int x, int y)
	{
		if (!IsFilled(x, y)) return;

		int rightLeft = (IsFilled(x + 1, y) << 1) | IsFilled(x - 1, y);
		int downUp =	(IsFilled(x, y - 1) << 1) | IsFilled(x, y + 1);

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

	bool Room::CollidesWith(int left, int right, int bottom, int top)
	{
		int2 corners[] = { {left, bottom}, {right-1, bottom}, {left,top-1}, {right-1,top-1} }; // Decrement right and top so they don't sample the next tile

		for (int i = 0; i < 4; i++)
		{
			int2 roomPosition = WorldToRoomPosition(corners[i]);
			int index = RoomPositionToIndex(roomPosition.x, roomPosition.y);
			if (Contains(roomPosition) && tiles[index].type != Air) // Do not use IsFilled() as it is intended for autotiling and not collisions
				return true;
		}
		return false;
	}

	bool Room::Contains(int2 roomPosition)
	{
		int index = RoomPositionToIndex(roomPosition.x, roomPosition.y);
		return (0 <= roomPosition.x && roomPosition.x < width && 0 <= roomPosition.y && roomPosition.y < height && 0 <= index && index < width* height);
	}

	void Room::Save(const std::string& filepath)
	{
		std::fstream out(filepath, std::ios::out | std::ios::binary | std::ios::trunc);

		if (out)
		{
			out << width << ' ';
			out << height << ' ';
			for (int i = 0; i < width * height; i++)
				out << tiles[i].type;
			for (int i = 0; i < entityCount; i++)
			{
				out << '\n';
				out << "ID: " << entities[i].ID << ", ";
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

			int width, height;
			stream >> width;
			stream >> height;

			char* data = new char[width*height];
			stream >> data;

			room = CreateRef<Room>(width, height);
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

				int ID = 0, pX = 0, pY = 0;
				stream.seekg(line.find("ID:") + 3);
				stream >> ID;
				stream.seekg(line.find("X:") + 2);
				stream >> pX;
				stream.seekg(line.find("Y:") + 2);
				stream >> pY;

				room->entities[room->entityCount++] = *entityCreateFn(int2(pX, pY), ID);
			}
		}
		else
			CR_CORE_ERROR("Could not open file '{0}'", filepath);
		in.close();
		
		return room;
	}
}