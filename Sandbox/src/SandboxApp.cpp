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
		: Application(), cameraController(16.0f / 9.0f)
	{
		player = Entities::CreateEntity({ 0.0f, 0.0f }, Entities::Cockroach);

		room = Room::Load("assets/scenes/room1.txt");
		for (i32 y = 0; y < room->height; y++)
		{
			for (i32 x = 0; x < room->width; x++)
			{
				if (room->data[x + y * room->width].type == Room::Air) continue;
				Entities::CreateEntity({ 8 * x, 8 * y }, Entities::EntityType::Tile);
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

	CameraController cameraController;

	Ref<Entity> player;
	Ref<Room> room;
};

class Sandbox : public Cockroach::Application
{
public:
	Sandbox()
		: Application(), cameraController(16.0f / 9.0f)
	{
		scene = Cockroach::CreateRef<Cockroach::Scene>();
		scene->Load();

		texture = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");
		cursorSprite = scene->GetSubTexture("assets/textures/SpriteSheet.png", { 0, 0 }, { 8, 8 });

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
		cameraController.OnUpdate(dt);
		scene->Update(dt);

		if (Input::IsDown(CR_KEY_ESCAPE))
			room->PlaceTileBox(Room::Air, { 0,0 }, { 1000,1000 });

		if (Input::IsPressed(CR_KEY_LEFT_SHIFT) && (Input::IsPressed(CR_MOUSE_BUTTON_RIGHT) || Input::IsPressed(CR_MOUSE_BUTTON_MIDDLE)))
		{
			if (Input::IsDown(CR_KEY_LEFT_SHIFT) || Input::IsDown(CR_MOUSE_BUTTON_RIGHT) || Input::IsDown(CR_MOUSE_BUTTON_MIDDLE))
			{
				isBoxPlacing = true;
				boxPlaceStartPos = EntityPlacePosition();
			}
		}

		if (isBoxPlacing)
		{
			if (Input::IsUp(CR_KEY_LEFT_SHIFT) || Input::IsUp(CR_MOUSE_BUTTON_RIGHT))
			{
				int2 boxPlaceEndPos = EntityPlacePosition();
				int2 minPos = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
				int2 maxPos = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x), std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) };

				for(i32 y = minPos.y; y <= maxPos.y; y+=8)
					for (i32 x = minPos.x; x <= maxPos.x; x+=8)
					{
						Ref<Entity> ent = GetEntityAtPosition(cameraController.camera.ScreenToWorldPosition({ x,y }));

						if (!ent)
						{
							Entities::CreateEntity({ x,y }, Entities::EntityType::Tile);
						}
					}
				room->PlaceTileBox(Room::TileBasic, minPos, maxPos);
				isBoxPlacing = false;
			}
			if (Input::IsUp(CR_KEY_LEFT_SHIFT) || Input::IsUp(CR_MOUSE_BUTTON_MIDDLE))
			{
				int2 boxPlaceEndPos = EntityPlacePosition();
				int2 minPos = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
				int2 maxPos = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x), std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) };

				for (i32 y = minPos.y; y <= maxPos.y; y += 8)
					for (i32 x = minPos.x; x <= maxPos.x; x += 8)
					{
						Ref<Entity> ent = GetEntityAtPosition({x,y});
						if (ent)
						{
							ent->sprite = scene->GetSubTexture("assets/textures/SpriteSheet.png", { 0,0 }, { 1,1 });
							ent->GetComponent<Hitbox>()->enabled = false;
						}
					}
				room->PlaceTileBox(Room::Air, minPos, maxPos);
				isBoxPlacing = false;
			}
		}
		else
		{
			if (Input::IsDown(CR_MOUSE_BUTTON_LEFT))
				Entities::CreateEntity(EntityPlacePosition(), Entities::EntityType::Cockroach);
			else if (Input::IsPressed(CR_MOUSE_BUTTON_RIGHT))
			{
				Ref<Entity> ent = GetEntityAtPosition(cameraController.camera.ScreenToWorldPosition(Input::MousePosition()));

				if (!ent)
				{
					Entities::CreateEntity(EntityPlacePosition(), Entities::EntityType::Tile);
					room->PlaceTile(Room::TileBasic, EntityPlacePosition());
				}
			}
			else if (Input::IsPressed(CR_MOUSE_BUTTON_MIDDLE))
			{
				Ref<Entity> ent = GetEntityAtPosition(cameraController.camera.ScreenToWorldPosition(Input::MousePosition()));
				if (ent)
				{
					ent->sprite = scene->GetSubTexture("assets/textures/SpriteSheet.png", { 0,0 }, { 1,1 });
					ent->GetComponent<Hitbox>()->enabled = false;
					room->PlaceTile(Room::Air, EntityPlacePosition());
				}
			}
		}

	}

	virtual void Render() override
	{
		Renderer::ResetStats();

		Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Renderer::Clear();

		Renderer::BeginScene(cameraController.camera);

		RenderGrid();
		scene->Render();
		room->Render();

		if (!isBoxPlacing)
			Renderer::DrawQuad(EntityPlacePosition(), { cursorSprite->XSize(), cursorSprite->YSize() }, cursorSprite);
		else
		{
			int2 boxPlaceEndPos = EntityPlacePosition();
			float2 start = { std::min(boxPlaceStartPos.x, boxPlaceEndPos.x), std::min(boxPlaceStartPos.y, boxPlaceEndPos.y) };
			float2 end = { std::max(boxPlaceStartPos.x, boxPlaceEndPos.x) + 8.0f, std::max(boxPlaceStartPos.y, boxPlaceEndPos.y) + 8.0f };
			Renderer::DrawLine({ start.x, start.y, 0.0f },	{ start.x, end.y, 0.0f },	{ 1.0f, 1.0f, 0.0f, 1.0f });
			Renderer::DrawLine({ start.x, end.y, 0.0f },	{ end.x, end.y, 0.0f},		{ 1.0f, 1.0f, 0.0f, 1.0f });
			Renderer::DrawLine({ end.x, end.y, 0.0f },		{ end.x, start.y, 0.0f },	{ 1.0f, 1.0f, 0.0f, 1.0f });
			Renderer::DrawLine({ end.x, start.y, 0.0f },	{ start.x, start.y, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f });
		}

		for (auto& ent : scene->entities)
		{
			Ref<Hitbox> h = ent->GetComponent<Hitbox>();
			if (h && h->enabled)
				DrawHitbox(h);
		}

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
		float2 worldCoord = cameraController.camera.ScreenToWorldPosition(Input::MousePosition());
		return float2(std::floor(worldCoord.x / 8) * 8, std::floor(worldCoord.y / 8) * 8);
	}

	void DrawHitbox(Cockroach::Ref<Hitbox> h)
	{
		Cockroach::Renderer::DrawLine({ h->Left(), h->Bottom(), 0.0f }, { h->Left(), h->Top(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine({ h->Left(), h->Top(), 0.0f }, { h->Right(), h->Top(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine({ h->Right(), h->Top(), 0.0f }, { h->Right(), h->Bottom(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine({ h->Right(), h->Bottom(), 0.0f }, { h->Left(), h->Bottom(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
	}

	void RenderGrid()
	{
		for (int i = -80; i < 80; i++)
		{
			int xFloor = (int)cameraController.camera.GetPosition().x;
			float xColor = ((i + xFloor) % 8 == 0) ? 0.3f : 0.2f;

			int yFloor = (int)cameraController.camera.GetPosition().y;
			float yColor = (i + yFloor) % 8 == 0 ? 0.3f : 0.2f;

			Cockroach::Renderer::DrawLine({ i + xFloor, -80.0f + yFloor, 0.0f }, { i + xFloor, 79.0f + yFloor, 0.0f }, { xColor, xColor, xColor, 0.5f });
			Cockroach::Renderer::DrawLine({ -80.0f + xFloor, i + yFloor, 0.0f }, { 79.0f + xFloor, i + yFloor, 0.0f }, { yColor, yColor, yColor, 0.5f });
		}
	}
	
private:
	CameraController cameraController;
	Ref<Texture2D> texture;
	Ref<SubTexture2D> cursorSprite;
	Ref<Scene> scene;
	Ref<Room> room;

	bool isBoxPlacing = false;
	int2 boxPlaceStartPos = { 0.0f, 0.0f };
};

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Sandbox();
}