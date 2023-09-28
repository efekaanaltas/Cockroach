#include "EditorCursor.h"
#include "Game.h"

EditorCursor::BrushMode EditorCursor::brushMode = BrushMode::Tile;
Cockroach::Entity* EditorCursor::selectedEntity = nullptr;
Room::TileType EditorCursor::tileType = Room::TileBasic;
int EditorCursor::entityType = EntityType::SpikeLeft;
int EditorCursor::decorationType = 0;
bool EditorCursor::isBoxPlacing = false;
int2 EditorCursor::boxPlaceStartPos = { 0.0f, 0.0f };

void EditorCursor::Update()
{
	if (Input::IsPressed(CR_MOUSE_BUTTON_MIDDLE))
	{
		Game::player->position = EditorCursor::WorldPosition();
		Game::player->TryChangeRoom();
	}

	if (Input::IsPressed(CR_MOUSE_BUTTON_LEFT))
	{
		switch (brushMode)
		{
		case BrushMode::Tile:
		{
			Room::current->PlaceTile(Input::IsPressed(CR_KEY_LEFT_CONTROL) ? Room::Air : tileType, WorldPositionSnapped());
			break;
		}
		case BrushMode::Entity:
		{
			Cockroach::Entity* entityOverCursor = nullptr;
			for (auto& ent : Room::current->entities)
				if (Dynamic* dyn = ent->As<Dynamic>())
					if (dyn->WorldHitbox().OverlapsWith(Rect(WorldPositionSnapped(), WorldPositionSnapped() + int2(8, 8)), 0, 0))
					{
						entityOverCursor = ent;
						selectedEntity = ent;
					}
			if (!Input::IsPressed(CR_KEY_LEFT_CONTROL) && !entityOverCursor)
				Room::current->AddEntity(CreateEntity(EntityDefinition(entityType, false, WorldPositionSnapped(), {1,1})));
			else if (Input::IsPressed(CR_KEY_LEFT_CONTROL) && entityOverCursor)
				Room::current->RemoveEntity(entityOverCursor);
			break;
		}
		}
	}
	
	int z = (Input::IsDown(CR_MOUSE_BUTTON_LEFT) ? 1 : (Input::IsDown(CR_MOUSE_BUTTON_RIGHT) ? -1 : 0));
	if (z != 0 && brushMode == Decoration)
	{
		Cockroach::Entity* decorationOverCursor = nullptr;
		for (auto& ent : Room::current->entities)
			if (Entities::Decoration* deco = ent->As<Entities::Decoration>())
				if (deco->WorldHitbox().OverlapsWith(Rect(WorldPosition(), WorldPosition() + 8 * ONEi), 0, 0))
				{
					decorationOverCursor = ent;
					selectedEntity = ent;
				}
		if (Input::IsPressed(CR_KEY_LEFT_CONTROL) && decorationOverCursor)
			Room::current->RemoveEntity(decorationOverCursor);
		else if (!Input::IsPressed(CR_KEY_LEFT_CONTROL))
		{
			EntityDefinition decorationDef{ decorationType, true, WorldPosition(), 8 * ONEi };
			decorationDef.z = z;
			Room::current->AddEntity(CreateEntity(decorationDef));
		}
	}

	if (Input::IsDown(CR_MOUSE_BUTTON_RIGHT))
	{
		isBoxPlacing = true;
		boxPlaceStartPos = WorldPositionSnapped();
	}

	else if (Input::IsUp(CR_MOUSE_BUTTON_RIGHT))
	{
		isBoxPlacing = false;
		int2 boxPlaceEndPos = WorldPositionSnapped();
		int2 minPos = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
		int2 maxPos = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x), std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) };
		switch (brushMode)
		{
		case BrushMode::Tile:
		{
			Room::current->PlaceTileBox(Input::IsPressed(CR_KEY_LEFT_CONTROL) ? Room::Air : tileType, minPos, maxPos);
			break;
		}
		case BrushMode::Entity:
		{
			Cockroach::Entity* entityOverCursor = nullptr;
			for (auto& ent : Room::current->entities)
				if (Dynamic* dyn = ent->As<Dynamic>())
					if (dyn->WorldHitbox().OverlapsWith(Rect(WorldPositionSnapped(), WorldPositionSnapped() + int2(8, 8)), 0, 0))
					{
						entityOverCursor = ent;
						selectedEntity = ent;
					}
			if (!Input::IsPressed(CR_KEY_LEFT_CONTROL) && !entityOverCursor)
				Room::current->AddEntity(CreateEntity(EntityDefinition(entityType, false, minPos, { maxPos.x-minPos.x+8,maxPos.y-minPos.y+8 })));
			else if (Input::IsPressed(CR_KEY_LEFT_CONTROL) && entityOverCursor)
				Room::current->RemoveEntity(entityOverCursor);
			break;
		}
		case BrushMode::Room:
		{
			Game::rooms.push_back(CreateRef<Cockroach::Room>(std::to_string(Game::rooms.size() - 1), (maxPos.x - minPos.x)/8+1, (maxPos.y - minPos.y)/8+1, minPos.x/8, minPos.y/8));
			Game::rooms[Game::rooms.size() - 1]->UpdateTileUVAll();
			Game::rooms[Game::rooms.size() - 1]->Save();
			break;
		}
		}
	}
	}

void EditorCursor::Render()
{
	if (!isBoxPlacing)
		boxPlaceStartPos = brushMode == Decoration ? WorldPosition() : WorldPositionSnapped();
	int2 boxPlaceEndPos = brushMode == Decoration ? WorldPosition() : WorldPositionSnapped();
	float2 start = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
	float2 end = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x) + 8.0f, std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) + 8.0f };
	Renderer::DrawQuadOutline(start.x, end.x, start.y, end.y, Input::IsPressed(CR_KEY_LEFT_CONTROL) ? RED : YELLOW);
}

int2 EditorCursor::WorldPosition()
{
	return Game::cameraController->camera.ScreenToWorldPosition(Input::MousePosition());
}

int2 EditorCursor::WorldPositionSnapped()
{
	float2 worldCoord = Game::cameraController->camera.ScreenToWorldPosition(Input::MousePosition());
	return float2(std::floor(worldCoord.x / 8) * 8, std::floor(worldCoord.y / 8) * 8);
}