#pragma once

#include <sstream>

namespace Cockroach
{
	template<typename T>
	inline T lerp(T a, T b, float t)
	{
		return a + t * (b - a);
	}

	//template<>
	//inline int2 lerp(int2 a, int2 b, float t)
	//{
	//	return { lerp(a.x, b.x, t), lerp(a.y, b.y, t) };
	//}

	template<glm::length_t L, typename T, glm::qualifier Q>
	inline glm::vec<L, T, Q> lerp(glm::vec<L, T, Q> a, glm::vec<L, T, Q> b, float t) 
	{
		glm::vec<L, T, Q> result;
		for (int i = 0; i < L; i++)
			result[i] = lerp(a[i], b[i], t);
		return result;
	}


	std::vector<std::string> Split(const std::string& string, char delim = ',');

	template<typename T>
	inline std::string GenerateProperty(std::string propertyName, T value)
	{
		return propertyName + ": " + std::to_string(value) + ", ";
	}

	inline bool HasProperty(const std::stringstream& stream, const std::string& propertyName)
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