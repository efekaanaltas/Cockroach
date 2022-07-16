#include "crpch.h"
#include "Room.h"

#include <fstream>

namespace Cockroach
{
	Room::Room(u32 width, u32 height)
		: width(width), height(height)
	{
		data = new char[width * height];
		memset(data, 0, sizeof(char) * width * height);
	}

	void Room::PlaceTile(char tileType, int2 worldPosition)
	{
		i32 roomPosition = worldPosition.x / 8 + worldPosition.y / 8 * width;

		if (0 <= roomPosition && roomPosition < sizeof(char) * width * height)
			data[roomPosition] = tileType;

		Save("assets/scenes/room1.txt");
	}

	void Room::PlaceTileBox(char tileType, int2 worldPositionMin, int2 worldPositionMax)
	{
		for (i32 y = worldPositionMin.y; y <= worldPositionMax.y; y++)
			for (i32 x = worldPositionMin.x; x <= worldPositionMax.x; x++)
			{
				i32 roomPosition = x / 8 + y / 8 * width;

				if (0 <= roomPosition && roomPosition < sizeof(char) * width * height)
					data[roomPosition] = tileType;
			}

		Save("assets/scenes/room1.txt");
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
		u32 a = width;
		u32 b = height;
		return std::to_string(width) + " " + std::to_string(height) + " " + data;
	}

	Ref<Room> Room::Deserialize(std::string data)
	{
		std::vector<std::string> strings = std::vector<std::string>();
		i32 current = 0, i = 0, start = 0, end = 0;
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
		strcpy(room->data, strings[2].c_str());
		return room;
	}
}