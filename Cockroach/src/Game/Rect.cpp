#include "crpch.h"
#include "Rect.h"

namespace Cockroach
{
	Rect::Rect(int2 min, int2 max)
		: min(min), max(max)
	{
	}

	bool Rect::OverlapsWith(Rect other, int xForesense, int yForesense) const
	{
		bool x = (min.x + xForesense < other.max.x) && (max.x + xForesense > other.min.x);
		bool y = (min.y + yForesense < other.max.y) && (max.y + yForesense > other.min.y);
		return x && y;
	}

	bool Rect::Contains(Rect other, int xForesense, int yForesense) const
	{
		bool x = (min.x <= other.min.x && other.max.x <= max.x);
		bool y = (min.y <= other.min.y && other.max.y <= max.y);
		return x && y;
	}

	bool Rect::Contains(int2 coord) const
	{
		bool x = min.x <= coord.x && coord.x <= max.x;
		bool y = min.y <= coord.y && coord.y <= max.y;
		return x && y;
	}

	int2 Rect::Center() const
	{
		return { (min.x + max.x) / 2, (min.y + max.y) / 2 };
	}

	Rect Rect::Extend(int amount)
	{
		return Rect(min - amount*ONEi, max + amount*ONEi);
	}
}