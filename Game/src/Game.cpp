#include <Cockroach.h>
#include <Cockroach/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Components.h"
#include "Entities.h"

#include "Game.h"

Ref<CameraController> Game::cameraController = nullptr;
Ref<Player> Game::player = nullptr;

Ref<Texture2D> Game::baseSpriteSheet = nullptr;

Game::Game()
	: Application()
{
	Game::baseSpriteSheet = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");
	Room::current = Room::Load("assets/scenes/room1.txt", Entities::Create);

	Entity* e = new Entity({ 0,0 });
	e->ID = Entities::Camera;
	e->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0,0 }, { 1,1 });
	cameraController = e->AddComponent<CameraController>();

	Entity* pe = new Entity({ 10,10 });
	pe->ID = Entities::Cockroach;
	pe->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0, 3 }, { 16, 16 });
	pe->AddComponent<Animator>();
	player = pe->AddComponent<Player>();
	player->hitbox = Rect({ 6,0 }, { 10,8 });
}

void Game::Update(float dt)
{
	cameraController->entity->Update(dt);
	player->entity->Update(dt);
	Room::current->Update(dt);

	if (Input::IsDown(CR_KEY_ESCAPE))
	{
		Entities::Create(EntityPlacePosition(), Entities::PushBlock);
		Room::current->Save("assets/scenes/room1.txt");
	};

	if (Input::IsPressed(CR_MOUSE_BUTTON_LEFT))
		Room::current->PlaceTile(Input::IsPressed(CR_KEY_LEFT_CONTROL) ? Room::Air : Room::TileBasic, EntityPlacePosition());

	else if (Input::IsDown(CR_MOUSE_BUTTON_RIGHT))
	{
		isBoxPlacing = true;
		boxPlaceStartPos = EntityPlacePosition();
	}

	else if (Input::IsUp(CR_MOUSE_BUTTON_RIGHT))
	{
		isBoxPlacing = false;
		int2 boxPlaceEndPos = EntityPlacePosition();
		int2 minPos = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
		int2 maxPos = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x), std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) };
		Room::current->PlaceTileBox(Input::IsPressed(CR_KEY_LEFT_CONTROL) ? Room::Air : Room::TileBasic, minPos, maxPos);
	}
}

void Game::Render()
{
	Renderer::ResetStats();

	Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Renderer::Clear();

	Renderer::BeginScene(cameraController->camera);

	if (Input::IsDown(CR_KEY_G))
		renderGrid = !renderGrid;
	if (Input::IsDown(CR_KEY_H))
		renderHitboxes = !renderHitboxes;

	if (renderGrid)
		RenderGrid();

	Room::current->Render(Game::baseSpriteSheet);
	player->entity->Render();

	if (renderHitboxes)
		RenderHitboxes();

	RenderCursor();

	Cockroach::Renderer::EndScene();

	ImGuiRender();
}

void Game::ImGuiRender()
{
	using namespace ImGui;
	Application::ImGuiBegin();

	ImGuiIO io = ImGui::GetIO();
	Begin("Frame Info");
	Text("%.3f ms (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	Text("Entity Count: %i", Room::current->entityCount);
	Text("Player Position: %i %i", player->entity->position.x, player->entity->position.y);
	Text("Player Velocity: %.1f, %.1f", player->velocity.x, player->velocity.y);
	End();

	Application::ImGuiEnd();
}

Entity* Game::GetEntityAtPosition(float2 position)
{
	if (Room::current->entityCount == 0)
		return nullptr;
	for (int i = 0; i < Room::current->entityCount; i++)
	{
		Ref<DynamicObject> dyn = Room::current->entities[i].GetComponent<DynamicObject>();
		if(dyn && dyn->hitbox.Contains(position))
			return &Room::current->entities[i];
	}
	return false;
}

float2 Game::EntityPlacePosition()
{
	float2 worldCoord = cameraController->camera.ScreenToWorldPosition(Input::MousePosition());
	return float2(std::floor(worldCoord.x / 8) * 8, std::floor(worldCoord.y / 8) * 8);
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

void Game::RenderCursor()
{
	if (!isBoxPlacing)
		boxPlaceStartPos = EntityPlacePosition();
	int2 boxPlaceEndPos = EntityPlacePosition();
	float2 start = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
	float2 end = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x) + 8.0f, std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) + 8.0f };
	Renderer::DrawQuadOutline(start.x, end.x, start.y, end.y, { 1.0f, Input::IsPressed(CR_KEY_LEFT_CONTROL) ? 0.0f : 1.0f, 0.0f , 1.0f });
}

void Game::RenderHitboxes()
{
	Ref<DynamicObject> dyn = player->entity->GetComponent<DynamicObject>();
	Renderer::DrawQuadOutline(dyn->Left(), dyn->Right(), dyn->Bottom(), dyn->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });

	for (int i = 0; i < Room::current->entityCount; i++)
	{
		Ref<DynamicObject> dyn = Room::current->entities[i].GetComponent<DynamicObject>();
		if (dyn)
			Renderer::DrawQuadOutline(dyn->Left(), dyn->Right(), dyn->Bottom(), dyn->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });
	}

	for (int i = 0; i < Room::current->width * Room::current->height; i++)
	{
		if (Room::current->tiles[i].type != Room::Air)
		{
			int2 worldPos = Room::current->RoomToWorldPosition(Room::current->IndexToRoomPosition(i));
			Renderer::DrawQuadOutline(worldPos.x, worldPos.x + 8, worldPos.y, worldPos.y + 8, { 1.0f, 0.0f, 0.0f, 1.0f });
		}
	}
}

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Game();
}