#pragma once

#include "Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Cockroach
{
	class Log
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger> coreLogger;
		static std::shared_ptr<spdlog::logger> clientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector) { return os << glm::to_string(vector); }

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix) { return os << glm::to_string(matrix); }

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::qua<T, Q>& quaternion) { return os << glm::to_string(quaternion); }

#define CR_CORE_TRACE(...)     ::Cockroach::Log::coreLogger->trace(__VA_ARGS__)
#define CR_CORE_INFO(...)      ::Cockroach::Log::coreLogger->info(__VA_ARGS__)
#define CR_CORE_WARN(...)      ::Cockroach::Log::coreLogger->warn(__VA_ARGS__)
#define CR_CORE_ERROR(...)     ::Cockroach::Log::coreLogger->error(__VA_ARGS__)
#define CR_CORE_CRITICAL(...)  ::Cockroach::Log::coreLogger->critical(__VA_ARGS__)

#define CR_TRACE(...)		   ::Cockroach::Log::clientLogger->trace(__VA_ARGS__)
#define CR_INFO(...)		   ::Cockroach::Log::clientLogger->info(__VA_ARGS__)
#define CR_WARN(...)		   ::Cockroach::Log::clientLogger->warn(__VA_ARGS__)
#define CR_ERROR(...)		   ::Cockroach::Log::clientLogger->error(__VA_ARGS__)
#define CR_CRITICAL(...)	   ::Cockroach::Log::clientLogger->critical(__VA_ARGS__)
