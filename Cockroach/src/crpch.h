#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Cockroach/Core/Log.h"

#include <glm/glm.hpp>

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

#define CR_COLOR_WHITE		float4(1.0f, 1.0f, 1.0f, 1.0f)
#define CR_COLOR_BLACK		float4(0.0f, 0.0f, 0.0f, 1.0f)
#define CR_COLOR_CLEAR		float4(0.0f, 0.0f, 0.0f, 0.0f)
#define CR_COLOR_RED		float4(1.0f, 0.0f, 0.0f, 1.0f)
#define CR_COLOR_GREEN		float4(0.0f, 1.0f, 0.0f, 1.0f)
#define CR_COLOR_BLUE		float4(0.0f, 0.0f, 1.0f, 1.0f)
#define CR_COLOR_YELLOW		float4(1.0f, 1.0f, 0.0f, 1.0f)
#define CR_COLOR_CYAN		float4(0.0f, 1.0f, 1.0f, 1.0f)
#define CR_COLOR_MAGENTA	float4(1.0f, 0.0f, 1.0f, 1.0f)

#ifdef CR_PLATFORM_WINDOWS
	#include <Windows.h>
#endif