#pragma once

#include <string>

namespace Cockroach
{
	class Room
	{
	public:
		static const char Air = '_';
		static const char TileBasic = 'B';

		Room(u32 width, u32 height);

		u32 width, height;
		char* data;

		void PlaceTile(char tileType, int2 worldPlacePosition);

		void Save(const std::string& filepath);
		static Ref<Room> Load(const std::string& filepath);
	private:
		std::string Serialize();
		static Ref<Room> Deserialize(std::string data);
	};
}