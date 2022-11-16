#pragma once

#include <glm/glm.hpp>

namespace Cockroach
{
	class Rect
	{
	public:
		Rect() {}
		Rect(glm::ivec2 min, glm::ivec2 max);

		glm::ivec2 min = { 0, 0 };
		glm::ivec2 max = { 8, 8 };

		bool OverlapsWith(Rect other, int xForesense = 0, int yForesense = 0) const;

		bool Contains(Rect other, int xForesense = 0, int yForesense = 0) const;
		bool Contains(glm::ivec2 coord) const;
	};
}