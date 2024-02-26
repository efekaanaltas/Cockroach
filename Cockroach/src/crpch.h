#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <optional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "Core/Log.h"

#include <glm/glm.hpp>

#define local_persist static
#define global		  static

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t
#define float2 glm::vec2
#define float3 glm::vec3
#define float4 glm::vec4
#define int2 glm::ivec2
#define int3 glm::ivec3
#define mat3 glm::mat3
#define mat4 glm::mat4

template<typename T>
using vector = std::vector<T>;

using string = std::string;
using stringstream = std::stringstream;

#define PI glm::pi<float>()

#define ONE			float2(1.0f, 1.0f)
#define ZERO		float2(0.0f, 0.0f)
#define LEFT		float2(-1.0f, 0.0f)
#define RIGHT		float2(+1.0f, 0.0f)
#define DOWN		float2(0.0f, -1.0f)
#define UP			float2(0.0f, +1.0f)

#define ONEi		int2(1, 1)
#define ZEROi		int2(0, 0)
#define LEFTi		int2(-1, 0)
#define RIGHTi		int2(1, 0)
#define DOWNi		int2(0, -1)
#define UPi			int2(0, 1)

#define WHITE		float4(1.0f, 1.0f, 1.0f, 1.0f)
#define BLACK		float4(0.0f, 0.0f, 0.0f, 1.0f)
#define CLEAR		float4(0.0f, 0.0f, 0.0f, 0.0f)
#define RED			float4(1.0f, 0.0f, 0.0f, 1.0f)
#define GREEN		float4(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE		float4(0.0f, 0.0f, 1.0f, 1.0f)
#define YELLOW		float4(1.0f, 1.0f, 0.0f, 1.0f)
#define CYAN		float4(0.0f, 1.0f, 1.0f, 1.0f)
#define MAGENTA		float4(1.0f, 0.0f, 1.0f, 1.0f)

#ifdef CR_PLATFORM_WINDOWS
	#include <Windows.h>
#endif