#include <Cockroach.h>
#include <Cockroach/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Components.h"
#include "Entities.h"

class Game : public Application
{
public:
	Game()
		: Application()
	{
		player = Entities::Create({ 0.0f, 0.0f }, Entities::Cockroach);
		cameraController = Entities::Create({ 0.0f, 0.0f }, Entities::Camera)->GetComponent<CameraController>();

		room = Room::Load("assets/scenes/room1.txt");
		for (i32 y = 0; y < room->height; y++)
		{
			for (i32 x = 0; x < room->width; x++)
			{
				if (room->data[x + y * room->width].type == Room::Air) continue;
				Entities::Create({ 8 * x, 8 * y }, Entities::EntityType::Tile);
			}
		}
	}

	~Game() {}

	virtual void Update(float dt) override
	{
		if (Room::current != nullptr)
		{
			player->Update(dt);
			//room->Update(dt);
			//cameraController->FollowPlayer(player);
		}
		else
		{
			//camera->TransitionTo(nextRoom);
			//if (nextRoom.Contains(camera.bounds))
				//Room::current = nextRoom;
		}
	}

	Ref<CameraController> cameraController;

	Ref<Entity> player;
	Ref<Room> room;
};

class Sandbox : public Cockroach::Application
{
public:
	Sandbox()
		: Application()
	{
		scene = Cockroach::CreateRef<Cockroach::Scene>();
		scene->Load();

		cameraController = Entities::Create({ 0.0f, 0.0f }, Entities::Camera)->GetComponent<CameraController>();

		room = Room::Load("assets/scenes/room1.txt");
		for (i32 y = 0; y < room->height; y++)
		{
			for (i32 x = 0; x < room->width; x++)
			{
				if (room->data[x + y * room->width].type == Room::Air) continue;
				Ref<Entity> ent = scene->AddEntity({ x*8, y*8 });
				ent->sprite = scene->GetSubTexture("assets/textures/SpriteSheet.png", { 0,0 }, { 1,1 });
				Ref<Hitbox> h = ent->AddComponent<Hitbox>();
				h->min = { 0,0 };
				h->max = { 8,8 };
			}
		}
	}

	~Sandbox()
	{
	}

	virtual void Update(float dt) override
	{
		cameraController->Update(dt);
		scene->Update(dt);

		if (Input::IsDown(CR_KEY_ESCAPE))
			room->PlaceTileBox(Room::Air, { 0,0 }, { 1000,1000 });
		
		if (Input::IsPressed(CR_MOUSE_BUTTON_LEFT))
		{
			Ref<Entity> ent = GetEntityAtPosition(cameraController->camera.ScreenToWorldPosition(Input::MousePosition()));

			if (Input::IsPressed(CR_KEY_LEFT_CONTROL)) // Remove
				room->PlaceTile(Room::Air, EntityPlacePosition());
			else // Place
				room->PlaceTile(Room::TileBasic, EntityPlacePosition());
		}

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
			if (Input::IsPressed(CR_KEY_LEFT_CONTROL))
				room->PlaceTileBox(Room::Air, minPos, maxPos);
			else
				room->PlaceTileBox(Room::TileBasic, minPos, maxPos);
		}

		else if (Input::IsDown(CR_MOUSE_BUTTON_MIDDLE))
				Entities::Create(EntityPlacePosition(), Entities::EntityType::Cockroach);

	}

	virtual void Render() override
	{
		Renderer::ResetStats();

		Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Renderer::Clear();

		Renderer::BeginScene(cameraController->camera);

		RenderGrid();
		scene->Render();
		room->Render();

		for (auto& ent : scene->entities)
		{
			Ref<Hitbox> h = ent->GetComponent<Hitbox>();
			if (h && h->enabled)
				DrawQuadOutline(h->Left(), h->Right(), h->Bottom(), h->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });
		}

		if (!isBoxPlacing)
			boxPlaceStartPos = EntityPlacePosition();
		int2 boxPlaceEndPos = EntityPlacePosition();
		float2 start = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
		float2 end = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x) + 8.0f, std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) + 8.0f };
		DrawQuadOutline(start.x, end.x, start.y, end.y, { 1.0f, 1.0f, 0.0f ,1.0f });

		Cockroach::Renderer::EndScene();

		ImGuiRender();
	}

	void ImGuiRender()
	{
		using namespace ImGui;
		Application::ImGuiBegin();

		ImGuiIO io = ImGui::GetIO();
		Begin("Frame Info");
		Text("%.3f ms (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		Text("Entity Count: %i", scene->entities.size());
		End();

		Application::ImGuiEnd();
	}

	Ref<Entity> GetEntityAtPosition(float2 position)
	{
		if (scene->entities.size() == 0)
			return nullptr;
		for (auto& ent : scene->entities)
		{
			Ref<Hitbox> h = ent->GetComponent<Hitbox>();
			if (h && h->Contains(position))
				return ent;
		}
		return false;
	}

	float2 EntityPlacePosition()
	{
		float2 worldCoord = cameraController->camera.ScreenToWorldPosition(Input::MousePosition());
		return float2(std::floor(worldCoord.x / 8) * 8, std::floor(worldCoord.y / 8) * 8);
	}

	void DrawQuadOutline(float x0, float x1, float y0, float y1, float4 color)
	{
		Renderer::DrawLine({ x0, y0, 0.0f }, { x0, y1, 0.0f }, color);
		Renderer::DrawLine({ x0, y1, 0.0f }, { x1, y1, 0.0f }, color);
		Renderer::DrawLine({ x1, y1, 0.0f }, { x1, y0, 0.0f }, color);
		Renderer::DrawLine({ x1, y0, 0.0f }, { x0, y0, 0.0f }, color);
	}

	void RenderGrid()
	{
		for (int i = -80; i < 80; i++)
		{
			int xFloor = (int)cameraController->camera.GetPosition().x;
			float xColor = ((i + xFloor) % 8 == 0) ? 0.3f : 0.2f;

			int yFloor = (int)cameraController->camera.GetPosition().y;
			float yColor = (i + yFloor) % 8 == 0 ? 0.3f : 0.2f;

			Cockroach::Renderer::DrawLine({ i + xFloor, -80.0f + yFloor, 0.0f }, { i + xFloor, 79.0f + yFloor, 0.0f }, { xColor, xColor, xColor, 0.5f });
			Cockroach::Renderer::DrawLine({ -80.0f + xFloor, i + yFloor, 0.0f }, { 79.0f + xFloor, i + yFloor, 0.0f }, { yColor, yColor, yColor, 0.5f });
		}
	}
	
private:
	Ref<CameraController> cameraController;
	Ref<Scene> scene;
	Ref<Room> room;

	bool isBoxPlacing = false;
	int2 boxPlaceStartPos = { 0.0f, 0.0f };
};

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Sandbox();
}