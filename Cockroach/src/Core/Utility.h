#pragma once

#include <sstream>

namespace Cockroach
{
	template<typename T>
	inline T lerp(T a, T b, float t)
	{
		return a + t * (b - a);
	}

	template<glm::length_t L, typename T, glm::qualifier Q>
	inline glm::vec<L, T, Q> lerp(glm::vec<L, T, Q> a, glm::vec<L, T, Q> b, float t) 
	{
		glm::vec<L, T, Q> result;
		for (int i = 0; i < L; i++)
			result[i] = lerp(a[i], b[i], t);
		return result;
	}

	static float random(float min, float max)
	{
		static int seed = 0;
		++seed;
		float noise = sin(dot({ seed + 3.486f, seed + 26.846f }, float2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f;
		noise = fmod(abs(noise), 1.0f) * (max - min) + min;
		return noise;
	}

	static int random(int min, int max)
	{
		static int seed = 0;
		++seed;
		float noise = sin(dot({ seed + 3.486f, seed + 26.846f }, float2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f;
		noise = fmod(abs(noise), 1.0f) * (max - min + 1) + min;
		return (int)noise;
	}

	template<typename T>
	inline T min(T a, T b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	inline T max(T a, T b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	inline T clamp(T v, T min, T max)
	{
		if (v < min) return min;
		if (v > max) return max;
		else return v;
	}

	template<typename T>
	inline T clamp01(T v)
	{
		return clamp(v, (T)0.0f, (T)1.0f);
	}

	template<typename T>
	inline T sign(T v)
	{
		if (v == 0.0f) return (T)+0.0f;
		if (v <  0.0f) return (T)-1.0f;
		if (v >	 0.0f) return (T)+1.0f;
	}

	template<typename T>
	inline T abs(T v)
	{
		return v < 0.0f ? -v : v;
	}

	vector<string> Split(const string& string, char delim = ',');

	template<typename T>
	inline string GenerateProperty(string propertyName, T value)
	{
		return propertyName + ": " + std::to_string(value) + ", ";
	}

	inline bool HasProperty(const std::stringstream& stream, const string& propertyName)
	{
		return stream.str().find(propertyName) != string::npos;
	}

	template<typename T>
	T GetProperty(std::stringstream& stream, const string& propertyName)
	{
		T value;
		stream.seekg(stream.str().find(propertyName) + propertyName.length() + 1);
		stream >> value;
		return value;
	}
}