#include "crpch.h"
#include "Room.h"

#include <fstream>
#include "Cockroach/Renderer/Renderer.h"

namespace Cockroach
{
	Room::Room(int width, int height)
		: width(width), height(height)
	{
		data = new Tile[width * height];
		memset(data, 0, sizeof(char) * width * height);
	}

	void Room::Render()
	{
		Ref<Texture2D> texture = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");
		for (int i = 0; i < width * height; i++)
		{
			if (data[i].type == Air) continue;
			Ref<SubTexture2D> sprite = SubTexture2D::CreateFromCoords(texture, { 11+data[i].texCoordOffset.x,2+data[i].texCoordOffset.y}, {8,8});
			Renderer::DrawQuad(IndexToRoomPosition(i) * 8, { 8,8 }, sprite);
		}
	}

	void Room::PlaceTile(TileType tileType, int2 worldPosition)
	{
		int roomPosition = worldPosition.x / 8 + worldPosition.y / 8 * width;

		if (0 <= roomPosition && roomPosition < width * height)
			data[roomPosition].type = tileType;

		Save("assets/scenes/room1.txt");
	}

	void Room::PlaceTileBox(TileType tileType, int2 worldPositionMin, int2 worldPositionMax)
	{
		for (int y = worldPositionMin.y; y <= worldPositionMax.y; y++)
			for (int x = worldPositionMin.x; x <= worldPositionMax.x; x++)
			{
				int roomPosition = x / 8 + y / 8 * width;

				if (0 <= roomPosition && roomPosition < width * height)
					data[roomPosition].type = tileType;
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

		data[RoomPositionToIndex(x, y)].texCoordOffset = { -rightLeft, downUp };
	}

	bool Room::IsFilled(int x, int y)
	{
		int index = RoomPositionToIndex(x, y);
		if (!Contains(index)) return true; // Little hack to make autotiling on borders easier
		return data[index].type != Air;
	}

	void Room::Save(const std::string& filepath)
	{
		std::string data = Serialize();
		std::fstream out(filepath, std::ios::out | std::ios::binary | std::ios::trunc);

		if (out)
		{
			out << data;
		}
		else
			CR_CORE_ERROR("Could not open file '{0}'", filepath);
		out.close();
	}

	Ref<Room> Room::Load(const std::string& filepath)
	{
		std::string result;
		std::fstream in(filepath, std::ios::in);

		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
				CR_CORE_ERROR("Could not read from file '{0}'", filepath);
		}
		else
			CR_CORE_ERROR("Could not open file '{0}'", filepath);
		in.close();

		return Room::Deserialize(result);
	}

	std::string Room::Serialize()
	{
		std::string save = std::to_string(width) + " " + std::to_string(height) + " ";
		for (int i = 0; i < width * height; i++)
			save += data[i].type;
		return save;
	}

	Ref<Room> Room::Deserialize(std::string data)
	{
		std::vector<std::string> strings = std::vector<std::string>();
		int current = 0, i = 0, start = 0, end = 0;
		while (i <= data.length())
		{
			if (data[i] == ' ' || i == data.length())
			{
				end = i;
				std::string sub = "";
				sub.append(data, start, end - start);
				strings.push_back(sub);
				current++;
				start = end + 1;
			}
			i++;
		}

		Ref<Room> room = CreateRef<Room>(std::stoi(strings[0]), std::stoi(strings[1]));
		const char* tileData = strings[2].c_str();
		for (int i = 0; i < room->width * room->height; i++)
			room->data[i].type = (TileType)tileData[i];
		for (int y = 0; y < room->height; y++)
			for (int x = 0; x < room->width; x++)
				room->UpdateTile(x, y);
		return room;
	}
}