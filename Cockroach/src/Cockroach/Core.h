#pragma once

#ifdef CR_PLATFORM_WINDOWS
	#ifdef CR_BUILD_DLL
		#define COCKROACH_API __declspec(dllexport)
	#else
		#define COCKROACH_API __declspec(dllimport)
	#endif
#else
#error Cockroach only supports Windows!
#endif