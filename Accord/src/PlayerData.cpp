#include "PlayerData.h"

void PlayerData::Save()
{
	string filepath = "saves/data1.txt";
	std::fstream out(filepath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (out)
	{
		out.write((char*)this, sizeof(PlayerData));
	}
	else
	{
		CR_CORE_ERROR("Could not open file '{0}'", filepath);
	}
		
	out.close();
}

void PlayerData::Load()
{
	string filepath = "saves/data1.txt";
	std::fstream in(filepath, std::ios::in || std::ios::binary);
	if (in)
	{
		in.read((char*)this, sizeof(PlayerData));
	}
	else
	{
		CR_CORE_ERROR("Could not open file '{0}'", filepath);
	}

	in.close();
}