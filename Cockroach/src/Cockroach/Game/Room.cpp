#include "crpch.h"
#include "Room.h"

namespace Cockroach
{
	Room::Room(u32 width, u32 height)
		: width(width), height(height)
	{
		data = new char[width * height];
		memset(data, 0, sizeof(char) * width * height);
	}

	std::string Room::Serialize()
	{
		std::string b = std::to_string(width);
		std::string a = std::to_string(width) + " " + std::to_string(height) + " " + data;
		return a;
	}

	Room* Room::Deserialize(std::string data)
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
		Room* room = new Room(std::stoi(strings[0]), std::stoi(strings[1]));
		strcpy(room->data, strings[2].c_str());
		return room;
	}
}