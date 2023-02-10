#include "crpch.h"
#include "Utility.h"

namespace Cockroach
{
	std::vector<std::string> Split(const std::string& string, char delim)
	{
		std::vector<std::string> elements;
		std::size_t start{};
		std::size_t end = string.find(delim);
		while (end != std::string::npos)
		{
			while (string[start] == ' ') ++start;
			elements.push_back(string.substr(start, end - start));
			start = end + 1;
			end = string.find(delim, start);
		}
		while (string[start] == ' ') ++start;
		elements.push_back(string.substr(start));
		return elements;
	}
}