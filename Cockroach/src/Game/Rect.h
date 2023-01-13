#pragma once

#include <glm/glm.hpp>

namespace Cockroach
{
	class Rect
	{
	public:
		Rect() {}
		Rect(int2 min, int2 max);

		int2 min = { 0, 0 };
		int2 max = { 8, 8 };

		bool OverlapsWith(Rect other, int xForesense = 0, int yForesense = 0) const;

		bool Contains(Rect other, int xForesense = 0, int yForesense = 0) const;
		bool Contains(int2 coord) const;
	};
}