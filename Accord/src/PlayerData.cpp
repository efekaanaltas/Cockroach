#include "PlayerData.h"

void PlayerData::Save()
{
	std::string filepath = "saves/data1.txt";
	std::fstream out(filepath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!out) CR_CORE_ERROR("Could not open file '{0}'", filepath);

	out << GenerateProperty("PlayerPosX", playerPosition.x);
	out << GenerateProperty("PlayerPosY", playerPosition.y); out << '\n';
	out << GenerateProperty("Fullscreen", fullscreen); out << '\n';
	out << GenerateProperty("MutedMaster", mutedMaster);
	out << GenerateProperty("MutedSFX", mutedSFX);
	out << GenerateProperty("MutedMusic", mutedMusic);
		
	out.close();
}

void PlayerData::Load()
{
	std::string filepath = "saves/data1.txt";
	std::fstream in(filepath, std::ios::in || std::ios::binary);
	if (!in) CR_CORE_ERROR("Could not open file '{0}'", filepath);

	std::stringstream stream;
	stream << in.rdbuf();

	playerPosition = { GetProperty<int>(stream, "PlayerPosX"), GetProperty<int>(stream, "PlayerPosY") };
	fullscreen =	GetProperty<bool>(stream, "Fullscreen");
	mutedMaster =	GetProperty<bool>(stream, "MutedMaster");
	mutedSFX =		GetProperty<bool>(stream, "MutedSFX");
	mutedMusic =	GetProperty<bool>(stream, "MutedMusic");

	in.close();
}