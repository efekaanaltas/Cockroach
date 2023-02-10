#pragma once

#include <sstream>

namespace Cockroach
{
	template<typename T>
	inline T lerp(T a, T b, float t)
	{
		return a + t * (b - a);
	}

	std::vector<std::string> Split(const std::string& string, char delim = ',');

	template<typename T>
	inline std::string GenerateProperty(std::string propertyName, T value)
	{
		return propertyName + ": " + std::to_string(value) + ", ";
	}

	inline bool HasProperty(std::stringstream& stream, const std::string& propertyName)
	{
		return stream.str().find(propertyName) != std::string::npos;
	}

	template<typename T>
	T GetProperty(std::stringstream& stream, const std::string& propertyName)
	{
		T value;
		stream.seekg(stream.str().find(propertyName) + propertyName.length() + 1);
		stream >> value;
		return value;
	}
}