#pragma once

#include <string>

namespace Cockroach
{
	class Room
	{
	public:
		static const char Air = ' ';
		static const char TileBasic = 'A';

		Room(u32 width, u32 height);

		u32 width, height;
		char* data;

		std::string Serialize();
		static Room* Deserialize(std::string data);
	};
}