#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Cockroach
{
	class COCKROACH_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define CR_CORE_TRACE(...)  ::Cockroach::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CR_CORE_INFO(...)   ::Cockroach::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CR_CORE_WARN(...)   ::Cockroach::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CR_CORE_ERROR(...)  ::Cockroach::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CR_CORE_FATAL(...)  ::Cockroach::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define CR_TRACE(...)       ::Cockroach::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CR_INFO(...)        ::Cockroach::Log::GetClientLogger()->info(__VA_ARGS__)
#define CR_WARN(...)        ::Cockroach::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CR_ERROR(...)       ::Cockroach::Log::GetClientLogger()->error(__VA_ARGS__)
#define CR_FATAL(...)       ::Cockroach::Log::GetClientLogger()->fatal(__VA_ARGS__)
