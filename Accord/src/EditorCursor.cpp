#include "EditorCursor.h"
using namespace Cockroach;
#include "Game.h"

EditorCursor::BrushMode EditorCursor::brushMode = BrushMode::Tile;
int EditorCursor::entityType = 2;
bool EditorCursor::isBoxPlacing = false;
int2 EditorCursor::boxPlaceStartPos = { 0.0f, 0.0f };

void EditorCursor::Update(float dt)
{
	if (Input::IsPressed(CR_MOUSE_BUTTON_LEFT))
	{
		switch (brushMode)
		{
		case BrushMode::Tile:
		{
			Room::current->PlaceTile(Input::IsPressed(CR_KEY_LEFT_CONTROL) ? Room::Air : Room::TileBasic, WorldPosition());
			break;
		}
		case BrushMode::Entity:
		{
			Cockroach::Entity* entityOverCursor = nullptr;
			for (auto& ent : Room::current->entities)
				if (Dynamic* dyn = ent->As<Dynamic>())
					if (dyn->WorldHitbox().OverlapsWith(Rect(WorldPosition(), WorldPosition() + int2(8, 8)), 0, 0))
						entityOverCursor = ent;
			if (!Input::IsPressed(CR_KEY_LEFT_CONTROL) && !entityOverCursor)
				CreateEntity(WorldPosition(), entityType);
			else if (Input::IsPressed(CR_KEY_LEFT_CONTROL) && entityOverCursor)
				Room::current->RemoveEntity(entityOverCursor);
			break;
		}
		}

	}

	if (brushMode == BrushMode::Entity) return;

	else if (Input::IsDown(CR_MOUSE_BUTTON_RIGHT))
	{
		isBoxPlacing = true;
		boxPlaceStartPos = WorldPosition();
	}

	else if (Input::IsUp(CR_MOUSE_BUTTON_RIGHT))
	{
		isBoxPlacing = false;
		int2 boxPlaceEndPos = WorldPosition();
		int2 minPos = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
		int2 maxPos = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x), std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) };
		switch (brushMode)
		{
		case BrushMode::Tile:
		{
			Room::current->PlaceTileBox(Input::IsPressed(CR_KEY_LEFT_CONTROL) ? Room::Air : Room::TileBasic, minPos, maxPos);
			break;
		}
		case BrushMode::Room:
		{
			Game::rooms.push_back(CreateRef<Cockroach::Room>(std::to_string(Game::rooms.size() - 1), (maxPos.x - minPos.x)/8+1, (maxPos.y - minPos.y)/8+1, minPos.x/8, minPos.y/8));
			Game::rooms[Game::rooms.size() - 1]->Save();
			break;
		}
		}
	}
	}

void EditorCursor::Render()
{
	if (!isBoxPlacing)
		boxPlaceStartPos = WorldPosition();
	int2 boxPlaceEndPos = WorldPosition();
	float2 start = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
	float2 end = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x) + 8.0f, std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) + 8.0f };
	Renderer::DrawQuadOutline(start.x, end.x, start.y, end.y, Input::IsPressed(CR_KEY_LEFT_CONTROL) ? CR_COLOR_RED : CR_COLOR_YELLOW);
}

int2 EditorCursor::WorldPosition()
{
	float2 worldCoord = Game::cameraController->camera.ScreenToWorldPosition(Input::MousePosition());
	return float2(std::floor(worldCoord.x / 8) * 8, std::floor(worldCoord.y / 8) * 8);
}