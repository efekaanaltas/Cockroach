#include <Cockroach.h>
#include <Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EntityDef.h"
#include "EditorCursor.h"

#include "Game.h"

#include <filesystem>

CameraController* Game::cameraController = nullptr;
Player* Game::player = nullptr;

Ref<Texture2D> Game::baseSpriteSheet = nullptr;
Ref<Texture2D> Game::background = nullptr;

std::vector<Ref<Room>> Game::rooms;

Game::Game()
	: Application()
{
	Game::background = CreateRef<Texture2D>("assets/textures/BG_RED.png");
	Game::baseSpriteSheet = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");

	const std::filesystem::path roomDir{ "assets/rooms" };
	for (auto& a : std::filesystem::directory_iterator(roomDir))
	{
		std::string name = a.path().string().substr(a.path().string().find_last_of("/\\") + 1);
		rooms.push_back(Room::Load(name));
	}

	Game::cameraController = new CameraController();
	cameraController->type = EntityType::Camera;
	cameraController->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0,0 }, { 1,1 });

	Game::player = new Player({10, 20}, {6,0}, {10,12});
	player->type = EntityType::Payga;
	player->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0, 3 }, { 16, 16 });

	for (auto& room : rooms)
		if (room->Contains(player->WorldHitbox()))
			Room::current = room;
}

void Game::Update(float dt)
{
	cameraController->Update(dt);
	player->Update(dt);
	Room::current->Update(dt);

	if (Input::IsPressed(CR_MOUSE_BUTTON_MIDDLE))
		player->position = EditorCursor::WorldPosition();
	int2 worldPosCur = EditorCursor::WorldPosition();
	EditorCursor::Update(dt);
}

void Game::Render()
{
	Renderer::SetClearColor({ 0.1f, 0.0f, 0.0f, 1 });
	Renderer::Clear();

	Renderer::BeginScene(cameraController->camera);

	if (Input::IsDown(CR_KEY_G)) renderGrid = !renderGrid;
	if (Input::IsDown(CR_KEY_H)) renderHitboxes = !renderHitboxes;
	if (Input::IsDown(CR_KEY_R)) renderAllRooms = !renderAllRooms;

	float zoom = cameraController->zoom;
	float aspect = cameraController->aspectRatio;

	Renderer::DrawQuad(cameraController->positionHighRes - float2(aspect * zoom, zoom), 2.0f * float2(aspect * zoom, zoom), background, {0,0}, {1,1});

	if (renderGrid) RenderGrid();

	bool renderAllVisibleRooms = !Room::current->Contains(player->WorldHitbox());

	for (int i = 0; i < rooms.size(); i++)
	{
		bool roomVisible = rooms[i]->OverlapsWith(cameraController->Bounds(), 0, 0);
		if (rooms[i] == Room::current || renderAllRooms || (renderAllVisibleRooms && roomVisible))
		{
			rooms[i]->Render(Game::baseSpriteSheet);

			if (renderRoomBoundaries)
			{
				Rect bounds = rooms[i]->Bounds();
				Renderer::DrawQuadOutline(bounds.min.x, bounds.max.x, bounds.min.y, bounds.max.y, CR_COLOR_GREEN);
			}
		}
	}

	player->Render();

	if (renderHitboxes) RenderHitboxes();

	for (auto& ent : Room::current->entities)
	{
		if (renderHitboxes && ent->type >= EntityType::TurbineLeft)
		{
			Renderer::DrawQuadOutline(ent->As<Turbine>()->turbineRect.min.x, ent->As<Turbine>()->turbineRect.max.x, ent->As<Turbine>()->turbineRect.min.y, ent->As<Turbine>()->turbineRect.max.y, CR_COLOR_GREEN);
		}
	}

	EditorCursor::Render();

	Cockroach::Renderer::EndScene();

	if(cameraController->editMode)
		ImGuiRender();
}

void Game::ImGuiRender()
{
	using namespace ImGui;
	Application::ImGuiBegin();

	ImGuiIO io = ImGui::GetIO();

	Begin("Info");
	Text("%.3f ms (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	Text("Pos: %i %i", player->position.x, player->position.y);
	Text("Vel: %.1f, %.1f", player->velocity.x, player->velocity.y);

	Checkbox("Hitboxes", &renderHitboxes);
	Checkbox("Grid", &renderGrid);
	Checkbox("Rooms", &renderAllRooms);
	Checkbox("Room Boundaries", &renderRoomBoundaries);

	End();

	Begin("Current Room");
	
	Text(Room::current->name.c_str());

	char buf[255]{};
	InputText("Name", buf, sizeof(buf));

	int pos[2] = { Room::current->position.x, Room::current->position.y };
	DragInt2("Pos", pos);
	if (Room::current->position.x != pos[0] || Room::current->position.y != pos[1])
		Room::current->position = { pos[0], pos[1] };

	int size[2] = { Room::current->width, Room::current->height };
	DragInt2("Size", size);
	if (Room::current->width != size[0] || Room::current->height != size[1])
		Room::current->Resize(size[0], size[1]);

	if (Button("Save"))
		Room::current->Save();

	End();

	Begin("Brush Settings");

	int* brushModeIntPtr = (int*)&EditorCursor::brushMode;
	const char* elems_names[3] = { "Tile", "Entity", "Room" };
	const char* elem_name = elems_names[*brushModeIntPtr];
	SliderInt("Mode", brushModeIntPtr, 0, 2, elem_name);

	if (EditorCursor::brushMode == EditorCursor::Tile)
	{
		bool backgroundTile = EditorCursor::tileType == Room::BackgroundBasic;
		Checkbox("Background Tile", &backgroundTile);
		EditorCursor::tileType = backgroundTile ? Room::BackgroundBasic : Room::TileBasic;
	}
	if (EditorCursor::brushMode == EditorCursor::Entity)
		SliderInt("Entity Type", &EditorCursor::entityType, EntityType::Camera+1, EntityType::END-1);
		
	End();

	Application::ImGuiEnd();
}

void Game::RenderGrid()
{
	for (int i = -80; i < 80; i++)
	{
		int xFloor = (int)cameraController->camera.GetPosition().x;
		float xColor = ((i + xFloor) % 8 == 0) ? 0.3f : 0.15f;

		int yFloor = (int)cameraController->camera.GetPosition().y;
		float yColor = (i + yFloor) % 8 == 0 ? 0.3f : 0.15f;

		Cockroach::Renderer::DrawLine({ i + xFloor, -80.0f + yFloor, 0.0f }, { i + xFloor, 79.0f + yFloor, 0.0f }, { xColor, xColor, xColor, 0.5f });
		Cockroach::Renderer::DrawLine({ -80.0f + xFloor, i + yFloor, 0.0f }, { 79.0f + xFloor, i + yFloor, 0.0f }, { yColor, yColor, yColor, 0.5f });
	}
}

void Game::RenderHitboxes()
{
	Renderer::DrawQuadOutline((float)player->Left(), (float)player->Right(), (float)player->Bottom(), (float)player->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });

	for (auto& ent : Room::current->entities)
	{
		Dynamic* dyn = ent->As<Dynamic>();
		if (dyn)
			Renderer::DrawQuadOutline((float)dyn->Left(), (float)dyn->Right(), (float)dyn->Bottom(), (float)dyn->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });
	}

	for (int i = 0; i < Room::current->width * Room::current->height; i++)
	{
		if (Room::current->tiles[i].type == Room::TileBasic)
		{
			float2 worldPos = Room::current->RoomToWorldPosition(Room::current->IndexToRoomPosition(i));
			Renderer::DrawQuadOutline(worldPos.x, worldPos.x + 8.0f, worldPos.y, worldPos.y + 8.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
		}
	}
}

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Game();
}