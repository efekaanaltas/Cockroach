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
		cameraController = Entities::Create({ 30.0f, 30.0f }, Entities::Camera)->GetComponent<CameraController>();

		room = Room::Load("assets/scenes/room1.txt", Entities::Create);
		Room::current = room;
	}

	~Game() {}

	virtual void Update(float dt) override
	{
		if (Room::current != nullptr)
		{
			player->Update(dt);
			room->Update(dt);
			//cameraController->FollowPlayer(player);
		}
		else
		{
			//camera->TransitionTo(nextRoom);
			//if (nextRoom.OverlapsWith(camera.bounds))
				//Room::current = nextRoom;
		}
	}

	Ref<CameraController> cameraController;

	Entity* player;
	Ref<Room> room;
};

class Sandbox : public Cockroach::Application
{
public:
	Sandbox()
		: Application()
	{
		Room::current = Room::Load("assets/scenes/room1.txt", Entities::Create);
		Ref<Texture2D> spriteSheet = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");
	
		Entity* e = new Entity({ 0,0 });
		e->ID = Entities::Camera;
		e->sprite = SubTexture2D::CreateFromCoords(spriteSheet, { 0,0 }, { 1,1 });
		Ref<Hitbox> h = e->AddComponent<Hitbox>();
		h->min = { -160, -90 };
		h->max = { 160, 90 };
		cameraController = e->AddComponent<CameraController>();
	}

	~Sandbox()
	{
	}

	virtual void Update(float dt) override
	{
		cameraController->Update(dt);
		Room::current->Update(dt);

		if (Input::IsDown(CR_KEY_ESCAPE))
			Room::current->PlaceTileBox(Room::Air, { 0,0 }, { 29,29 });
		
		if (Input::IsPressed(CR_MOUSE_BUTTON_LEFT))
		{
			Entity* ent = GetEntityAtPosition(cameraController->camera.ScreenToWorldPosition(Input::MousePosition()));

			if (Input::IsPressed(CR_KEY_LEFT_CONTROL)) // Remove
				Room::current->PlaceTile(Room::Air, EntityPlacePosition());
			else // Place
				Room::current->PlaceTile(Room::TileBasic, EntityPlacePosition());
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
				Room::current->PlaceTileBox(Room::Air, minPos, maxPos);
			else
				Room::current->PlaceTileBox(Room::TileBasic, minPos, maxPos);
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

		static bool renderGrid = true;
		if (Input::IsDown(CR_KEY_G))
			renderGrid = !renderGrid;
		static bool renderHitboxes = true;
		if (Input::IsDown(CR_KEY_H))
			renderHitboxes = !renderHitboxes;

		if(renderGrid)
			RenderGrid();
		Room::current->Render();
		
		if (renderHitboxes)
		{
			for (int i = 0; i < Room::current->entityCount; i++)
			{
				Ref<Hitbox> h = Room::current->entities[i]->GetComponent<Hitbox>();
				if (h && h->enabled)
					DrawQuadOutline(h->Left(), h->Right(), h->Bottom(), h->Top(), { 1.0f, 0.0f, 0.0f, 1.0f });
			}

			for (int i = 0; i < Room::current->width * Room::current->height; i++)
			{
				if (Room::current->tiles[i].type != Room::Air)
				{
					int2 worldPos = Room::current->RoomToWorldPosition(Room::current->IndexToRoomPosition(i));
					DrawQuadOutline(worldPos.x, worldPos.x + 8, worldPos.y, worldPos.y + 8, { 1.0f, 0.0f, 0.0f, 1.0f });
				}
			}
		}

		if (!isBoxPlacing)
			boxPlaceStartPos = EntityPlacePosition();
		int2 boxPlaceEndPos = EntityPlacePosition();
		float2 start = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
		float2 end = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x) + 8.0f, std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) + 8.0f };
		DrawQuadOutline(start.x, end.x, start.y, end.y, { 1.0f, Input::IsPressed(CR_KEY_LEFT_CONTROL) ? 0.0f : 1.0f, 0.0f , 1.0f});

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

		Text("Entity Count: %i", Room::current->entityCount);
		End();

		Application::ImGuiEnd();
	}

	Entity* GetEntityAtPosition(float2 position)
	{
		if (Room::current->entityCount == 0)
			return nullptr;
		for (auto& ent : Room::current->entities)
		{
			Ref<Hitbox> h = ent->GetComponent<Hitbox>();
			if (h && h->OverlapsWith(position))
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
			float xColor = ((i + xFloor) % 8 == 0) ? 0.3f : 0.15f;

			int yFloor = (int)cameraController->camera.GetPosition().y;
			float yColor = (i + yFloor) % 8 == 0 ? 0.3f : 0.15f;

			Cockroach::Renderer::DrawLine({ i + xFloor, -80.0f + yFloor, 0.0f }, { i + xFloor, 79.0f + yFloor, 0.0f }, { xColor, xColor, xColor, 0.5f });
			Cockroach::Renderer::DrawLine({ -80.0f + xFloor, i + yFloor, 0.0f }, { 79.0f + xFloor, i + yFloor, 0.0f }, { yColor, yColor, yColor, 0.5f });
		}
	}
	
private:
	Ref<CameraController> cameraController;

	bool isBoxPlacing = false;
	int2 boxPlaceStartPos = { 0.0f, 0.0f };
};

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Sandbox();
}