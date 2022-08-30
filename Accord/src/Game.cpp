#include <Cockroach.h>
#include <Cockroach/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Components.h"
#include "Entities.h"
#include "EditorCursor.h"

#include "Game.h"

#include <filesystem>

Ref<CameraController> Game::cameraController = nullptr;
Ref<Player> Game::player = nullptr;

Ref<Texture2D> Game::baseSpriteSheet = nullptr;

std::vector<Ref<Room>> Game::rooms;

Game::Game()
	: Application()
{
	Game::baseSpriteSheet = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");

	const std::filesystem::path roomDir{ "assets/rooms" };
	for (auto& a : std::filesystem::directory_iterator(roomDir))
	{
		std::string name = a.path().string().substr(a.path().string().find_last_of("/\\") + 1);
		rooms.push_back(Room::Load(name, Entities::Create));
	}

	Room::current = rooms[0];

	Entity* e = new Entity({ 0,0 });
	e->type = Entities::Camera;
	e->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0,0 }, { 1,1 });
	cameraController = e->AddComponent<CameraController>();

	Entity* pe = new Entity({ 10,10 });
	pe->type = Entities::Payga;
	pe->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0, 3 }, { 16, 16 });
	pe->AddComponent<Animator>();
	player = pe->AddComponent<Player>();
	player->hitbox = Rect({ 6,0 }, { 10,12 });
	player->layer = Light;
}

void Game::Update(float dt)
{
	cameraController->entity->Update(dt);
	player->entity->Update(dt);
	Room::current->Update(dt);

	static bool currentlyTransitioning = false; // Temporary

	bool roomContainsPlayer = Room::current->Contains(player->WorldHitbox());
	currentlyTransitioning = !roomContainsPlayer;

	if (currentlyTransitioning)
	{
		for (int i = 0; i < rooms.size(); i++)
		{
			if (rooms[i] == Room::current) continue;
			if (rooms[i]->Contains(player->WorldHitbox()))
			{
				Room::current = rooms[i];
				currentlyTransitioning = false;
				CR_TRACE("New current room: {0}", i);
			}
		}
	}

	for (int i = CR_KEY_KP_1; i <= CR_KEY_KP_3; i++)
		if(Input::IsDown(i))
			cameraController->SetZoom(std::powf(10.0f, (float)(i-CR_KEY_KP_1+1)));

	if (Input::IsPressed(CR_MOUSE_BUTTON_MIDDLE))
		player->entity->position = EditorCursor::WorldPosition();

	EditorCursor::Update(dt);
}

void Game::Render()
{
	Renderer::ResetStats();

	Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Renderer::Clear();

	Renderer::BeginScene(cameraController->camera);

	if (Input::IsDown(CR_KEY_G)) renderGrid = !renderGrid;
	if (Input::IsDown(CR_KEY_H)) renderHitboxes = !renderHitboxes;
	if (Input::IsDown(CR_KEY_R)) renderAllRooms = !renderAllRooms;

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
				float2 worldPosition = rooms[i]->RoomToWorldPosition({ 0,0 });
				float2 extents = rooms[i]->RoomToWorldPosition({ rooms[i]->width, rooms[i]->height });

				Renderer::DrawQuadOutline(worldPosition.x, extents.x, worldPosition.y, extents.y, { 0.0f, 1.0f, 0.0f, 1.0f });
			}
		}
	}

	player->entity->Render();

	if (renderHitboxes) RenderHitboxes();

	EditorCursor::Render();

	Cockroach::Renderer::EndScene();

	ImGuiRender();
}

void Game::ImGuiRender()
{
	using namespace ImGui;
	Application::ImGuiBegin();

	ImGuiIO io = ImGui::GetIO();
	Begin("Info");
	Text("%.3f ms (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	Text("Pos: %i %i", player->entity->position.x, player->entity->position.y);
	Text("Vel: %.1f, %.1f", player->velocity.x, player->velocity.y);

	Checkbox("Render Hitboxes", &renderHitboxes);
	Checkbox("Render Grid", &renderGrid);
	Checkbox("Render All Rooms", &renderAllRooms);
	Checkbox("Render Room Boundaries", &renderRoomBoundaries);

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

	SliderInt("Brush Mode", (int*) & EditorCursor::brushMode, 0, 2);
		
	End();

	Application::ImGuiEnd();
}

Entity* Game::GetEntityAtPosition(float2 position)
{
	for (int i = 0; i < Room::current->entities.size(); i++)
	{
		Ref<DynamicObject> dyn = Room::current->entities[i].GetComponent<DynamicObject>();
		if(dyn && dyn->hitbox.Contains(position))
			return &Room::current->entities[i];
	}
	return nullptr;
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
	Ref<DynamicObject> dyn = player->entity->GetComponent<DynamicObject>();
	Renderer::DrawQuadOutline((float)dyn->Left(), (float)dyn->Right(), (float)dyn->Bottom(), (float)dyn->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });

	for (int i = 0; i < Room::current->entities.size(); i++)
	{
		Ref<DynamicObject> dyn = Room::current->entities[i].GetComponent<DynamicObject>();
		if (dyn)
			Renderer::DrawQuadOutline((float)dyn->Left(), (float)dyn->Right(), (float)dyn->Bottom(), (float)dyn->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });
	}

	for (int i = 0; i < Room::current->width * Room::current->height; i++)
	{
		if (Room::current->tiles[i].type != Room::Air)
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