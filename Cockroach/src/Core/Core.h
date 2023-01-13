#pragma once

#include <memory>

#ifdef CR_PLATFORM_WINDOWS
#if CR_DYNAMIC_LINK
	#ifdef CR_BUILD_DLL
		#define COCKROACH_API __declspec(dllexport)
	#else
		#define COCKROACH_API __declspec(dllimport)
	#endif
#else
	#define COCKROACH_API
#endif
#else
#error Cockroach only supports Windows!
#endif

#ifdef CR_DEBUG
	#define CR_ENABLE_ASSERTS
#endif

#ifdef CR_ENABLE_ASSERTS
	#define CR_ASSERT(x, ...) { if(!(x)) { CR_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define CR_CORE_ASSERT(x, ...) { if(!(x)) { CR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define CR_ASSERT(x, ...)
	#define CR_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

namespace Cockroach
{
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	T lerp(T a, T b, float t)
	{
		return a + t * (b - a);
	}
}
