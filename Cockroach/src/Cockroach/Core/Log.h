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
		static std::shared_ptr<spdlog::logger> coreLogger;
		static std::shared_ptr<spdlog::logger> clientLogger;
	};
}

// Core log macros
#define CR_CORE_TRACE(...)     ::Cockroach::Log::coreLogger->trace(__VA_ARGS__)
#define CR_CORE_INFO(...)      ::Cockroach::Log::coreLogger->info(__VA_ARGS__)
#define CR_CORE_WARN(...)      ::Cockroach::Log::coreLogger->warn(__VA_ARGS__)
#define CR_CORE_ERROR(...)     ::Cockroach::Log::coreLogger->error(__VA_ARGS__)
#define CR_CORE_CRITICAL(...)  ::Cockroach::Log::s_CoreLogger->critical(__VA_ARGS__)

// Client log macros
#define CR_TRACE(...)		   ::Cockroach::Log::clientLogger->trace(__VA_ARGS__)
#define CR_INFO(...)		   ::Cockroach::Log::clientLogger->info(__VA_ARGS__)
#define CR_WARN(...)		   ::Cockroach::Log::clientLogger->warn(__VA_ARGS__)
#define CR_ERROR(...)		   ::Cockroach::Log::clientLogger->error(__VA_ARGS__)
#define CR_CRITICAL(...)	   ::Cockroach::Log::s_ClientLogger->critical(__VA_ARGS__)
