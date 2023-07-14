#include "crpch.h"
#include "Room.h"

#include <fstream>
#include "../Core/Utility.h"
#include "Renderer/Renderer.h"

#include "glm/glm.hpp"

namespace Cockroach
{
	Ref<Room> Room::current = nullptr;

	Room::Room(std::string name, int width, int height, int posX, int posY)
		: name(name), width(width), height(height)
	{
		position = { posX, posY };

		tiles = new TileType[width * height];
		tileUVs = new int2[(width + 1) * (height + 1)];
		backgroundTileUVs = new int2[(width + 1) * (height + 1)];

		memset(tiles, Air, width * height * sizeof(TileType));
		memset(tileUVs, 0, (width + 1) * (height + 1) * sizeof(int2));
		memset(backgroundTileUVs, 0, (width + 1) * (height + 1) * sizeof(int2));
	}

	void Room::Update(float dt)
	{
		for (int i = 0; i < entities.size(); i++)
			entities[i]->Update(dt);
	}

	void Room::Render(Ref<Texture2D> tilemapTexture)
	{
		for (int i = 0; i < (width+1) * (height+1); i++)
		{
			int2 roomPos = { i % (width + 1), i / (width + 1) };
			if (tileUVs[i] != invalidUV)
			{
				Sprite sprite = Sprite::CreateFromCoords(tilemapTexture, tileUVs[i], {8,8});
				Renderer::DrawQuad(float3(RoomToWorldPosition(roomPos), 0)-float3(4,4,0), {8,8}, sprite, WHITE, {0,0,0,0}, false, false);
			}
			if (backgroundTileUVs[i] != invalidUV)
			{
				Sprite sprite = Sprite::CreateFromCoords(tilemapTexture, backgroundTileUVs[i], { 8,8 });
				Renderer::DrawQuad(float3(RoomToWorldPosition(roomPos), 0) - float3(4, 4, 0), { 8,8 }, sprite, WHITE, { 0,0,0,0 }, false, false);
			}
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
			tiles[index] = tileType;

			for (int y = -1; y <= 1; y++)
				for (int x = -1; x <= 1; x++)
					UpdateTileUV(roomPosition.x - x, roomPosition.y + y);

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
					tiles[index] = tileType;

					for (int y = -1; y <= 1; y++)
						for (int x = -1; x <= 1; x++)
							UpdateTileUV(roomPosition.x - x, roomPosition.y + y);
				}
			}

		Save();
	}

	void Room::Resize(int newWidth, int newHeight)
	{
		TileType* newTiles = new TileType[newWidth * newHeight];

		for (int y = 0; y < newHeight; y++)
		{
			for (int x = 0; x < newWidth; x++)
			{
				int oldIndex = RoomPositionToIndex(x, y);
				int newIndex = y * newWidth + x;
				if (0 <= x && 0 <= y && x < newWidth && x < width && y < newHeight && y < height)
					newTiles[newIndex] = tiles[oldIndex];
				else
					newTiles[newIndex] = Air;
			}
		}

		delete[] tiles;
		tiles = newTiles;

		width = newWidth;
		height = newHeight;
		
		int2* newTileUVs = new int2[(newWidth + 1) * (newHeight + 1)];
		int2* newBackgroundTileUVs = new int2[(newWidth + 1) * (newHeight + 1)];

		delete[] tileUVs;
		delete[] backgroundTileUVs;

		tileUVs = newTileUVs;
		backgroundTileUVs = newBackgroundTileUVs;
		UpdateTileUVAll();

		//Save();
	}

	void Room::UpdateTileUV(int x, int y)
	{
		const int2 uvOffsetLUT[16] =
		{
			{-1,-1}, {-4,+1}, {-3,+1}, {-1,+2},
			{-4,+2}, {+0,+1}, {-3,+0}, {+0,+2},
			{-3,+2}, {-4,+0}, {-2,+1}, {-2,+2},
			{-1,+0}, {-0,+0}, {-2,+0}, {-1,+1},
		};

		{
			bool ld = IsFilled(x-1,y-1,	TileBasic);
			bool md = IsFilled(x,y-1,	TileBasic);
			bool lm = IsFilled(x-1,y,	TileBasic);
			bool mm = IsFilled(x,y,		TileBasic);

			int indexNum = ld*8+md*4+lm*2+mm;
			int UVStartY = 3 * random(0, 2);
			int2 uv = indexNum > 0 ? int2(4, UVStartY) + uvOffsetLUT[indexNum] : invalidUV;
			tileUVs[x + y * (width + 1)] = uv;
		}

		{
			bool ld = IsFilled(x-1,y-1,	BackgroundBasic);
			bool md = IsFilled(x,y-1,	BackgroundBasic);
			bool lm = IsFilled(x-1,y,	BackgroundBasic);
			bool mm = IsFilled(x,y,		BackgroundBasic);

			int indexNum = ld * 8 + md * 4 + lm * 2 + mm;
			int UVStartY = 3 * random(0, 2);
			int2 uv = indexNum > 0 ? int2(9, UVStartY) + uvOffsetLUT[indexNum] : invalidUV;
			backgroundTileUVs[x + y * (width + 1)] = uv;
		}
	}

	void Room::UpdateTileUVAll()
	{
		for (int y = 0; y < height + 1; y++)
			for (int x = 0; x < width + 1; x++)
				UpdateTileUV(x, y);
	}

	bool Room::IsFilled(int x, int y, TileType type)
	{
		int2 roomPosClamped = glm::clamp(int2(x,y), {0,0}, {width - 1, height - 1});
		int index = RoomPositionToIndex(roomPosClamped.x, roomPosClamped.y);
		return tiles[index] == type;
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
				if (Contains({x, y}) && tiles[index] == TileBasic) // Do not use IsFilled() as it is intended for autotiling and not collisions
					return true;
			}
		}
		return false;
	}

	bool Room::OverlapsWith(Rect rect, int xForesense, int yForesense)
	{
		return Bounds().OverlapsWith(rect, xForesense, yForesense);
	}

	bool Room::Contains(int2 roomPosition)
	{
		int index = RoomPositionToIndex(roomPosition.x, roomPosition.y);
		return (0 <= roomPosition.x && roomPosition.x < width && 0 <= roomPosition.y && roomPosition.y < height && 0 <= index && index < width * height);
	}

	bool Room::Contains(Rect rect)
	{	
		return Contains(WorldToRoomPosition(rect.min)) && Contains(WorldToRoomPosition(rect.max-ONEi));
	}

	void Room::Save()
	{
		std::fstream out(Filepath(), std::ios::out | std::ios::binary | std::ios::trunc);
		if (!out) CR_CORE_ERROR("Could not open file '{0}'", Filepath());
		
		out << width << ' ';
		out << height << ' ';
		out << position.x << ' ';
		out << position.y << ' ';
		for (int i = 0; i < width * height; i++)
			out << tiles[i];
		out << '\n';
		for (int i = 0; i < entities.size(); i++)
			out << entities[i]->GenerateDefinition().ToString();
		
		out.close();
	}

	Ref<Room> Room::Load(const std::string& name)
	{
		Ref<Room> room;
		std::string filepath = roomDir + name;
		std::fstream in(filepath, std::ios::in ||std::ios::binary);
		if (!in) CR_CORE_ERROR("Could not open file '{0}'", filepath);
		
		std::string line;
		std::getline(in, line);
		std::stringstream stream(line);

		int width, height, posX, posY;
		stream >> width;
		stream >> height;
		stream >> posX;
		stream >> posY;

		char* data = new char[width*height];
		stream.ignore(1);
		stream.read(data, width * height);

		room = CreateRef<Room>(name, width, height, posX, posY);

		for (int i = 0; i < room->width * room->height; i++)
			room->tiles[i] = (TileType)data[i];
		room->UpdateTileUVAll();

		while (std::getline(in, line))
		{
			std::stringstream stream(line);
			room->AddEntity(CreateEntity(stream));
		}

		in.close();
		
		return room;
	}

	void Room::Rename(const std::string& newName)
	{
		if (newName.empty()) { CR_CORE_ERROR("New name is empty"); return; }
		if (name == newName) { CR_CORE_ERROR("New name is same as old name."); return; }
		
		if (rename(Filepath().c_str(), (roomDir + newName).c_str()) == 0)
			name = newName;
		
		else CR_CORE_ERROR("Room could not be renamed from {0} to {1}.", Filepath(), (roomDir + newName));
	}
}