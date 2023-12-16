#include "crpch.h"
#include "Utility.h"

namespace Cockroach
{
	vector<std::string> Split(const std::string& string, char delim)
	{
		vector<std::string> elements;
		size_t start{};
		size_t end = string.find(delim);
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