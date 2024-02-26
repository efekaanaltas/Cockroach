#include "PlayerData.h"

void PlayerData::Save()
{
	string filepath = "saves/data1.txt";
	std::fstream out(filepath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!out) CR_CORE_ERROR("Could not open file '{0}'", filepath);

	out << GenerateProperty("PlayerPosX", playerPosition.x);
	out << GenerateProperty("PlayerPosY", playerPosition.y); out << '\n';
	out << GenerateProperty("Fullscreen", fullscreen); out << '\n';
	out << GenerateProperty("MutedMaster", mutedMaster);
	out << GenerateProperty("MutedSFX", mutedSFX);
	out << GenerateProperty("MutedMusic", mutedMusic); out << '\n';
	out << GenerateProperty("LeftKey", actionKeys[0]);
	out << GenerateProperty("RightKey", actionKeys[1]);
	out << GenerateProperty("DownKey", actionKeys[2]);
	out << GenerateProperty("UpKey", actionKeys[3]);
	out << GenerateProperty("JumpKey", actionKeys[4]);
	out << GenerateProperty("DashKey", actionKeys[5]); out << '\n';
		
	out.close();
}

void PlayerData::Load()
{
	string filepath = "saves/data1.txt";
	std::fstream in(filepath, std::ios::in || std::ios::binary);
	if (!in) CR_CORE_ERROR("Could not open file '{0}'", filepath);

	stringstream stream;
	stream << in.rdbuf();

	playerPosition = { GetProperty<int>(stream, "PlayerPosX"), GetProperty<int>(stream, "PlayerPosY") };
	fullscreen =	GetProperty<bool>(stream, "Fullscreen");
	mutedMaster =	GetProperty<bool>(stream, "MutedMaster");
	mutedSFX =		GetProperty<bool>(stream, "MutedSFX");
	mutedMusic =	GetProperty<bool>(stream, "MutedMusic");

	actionKeys[0] =	GetProperty<u16>(stream, "LeftKey");
	actionKeys[1] =	GetProperty<u16>(stream, "RightKey");
	actionKeys[2] =	GetProperty<u16>(stream, "DownKey");
	actionKeys[3] =	GetProperty<u16>(stream, "UpKey");
	actionKeys[4] =	GetProperty<u16>(stream, "JumpKey");
	actionKeys[5] =	GetProperty<u16>(stream, "DashKey");

	in.close();
}