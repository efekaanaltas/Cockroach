#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Cockroach
{
	class Log
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define CR_CORE_TRACE(...)  ::Cockroach::Log::s_CoreLogger->trace(__VA_ARGS__)
#define CR_CORE_INFO(...)   ::Cockroach::Log::s_CoreLogger->info(__VA_ARGS__)
#define CR_CORE_WARN(...)   ::Cockroach::Log::s_CoreLogger->warn(__VA_ARGS__)
#define CR_CORE_ERROR(...)  ::Cockroach::Log::s_CoreLogger->error(__VA_ARGS__)
#define CR_CORE_CRITICAL(...)  ::Cockroach::Log::s_CoreLogger->critical(__VA_ARGS__)

// Client log macros
#define CR_TRACE(...)       ::Cockroach::Log::s_ClientLogger->trace(__VA_ARGS__)
#define CR_INFO(...)        ::Cockroach::Log::s_ClientLogger->info(__VA_ARGS__)
#define CR_WARN(...)        ::Cockroach::Log::s_ClientLogger->warn(__VA_ARGS__)
#define CR_ERROR(...)       ::Cockroach::Log::s_ClientLogger->error(__VA_ARGS__)
#define CR_CRITICAL(...)       ::Cockroach::Log::s_ClientLogger->critical(__VA_ARGS__)
