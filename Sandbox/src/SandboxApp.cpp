#include <Cockroach.h>
#include <Cockroach/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Components.h"
#include "Entities.h"

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

		for (u32 y = 0; y < room->height; y++)
		{
			for (u32 x = 0; x < room->width; x++)
			{
				if (room->data[x + y * room->width] == Room::Air) continue;
				Entities::CreateEntity({ 8*x, 8*y }, Entities::EntityType::Tile);
			}
		}
	}

	void DrawHitbox(Cockroach::Ref<Hitbox> h)
	{
		Cockroach::Renderer::DrawLine({ h->Left(), h->Bottom(), 0.0f }, { h->Left(), h->Top(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine({ h->Left(), h->Top(), 0.0f }, { h->Right(), h->Top(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine({ h->Right(), h->Top(), 0.0f }, { h->Right(), h->Bottom(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine({ h->Right(), h->Bottom(), 0.0f }, { h->Left(), h->Bottom(), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
	}

	~Sandbox()
	{
	}

	virtual void Update(float dt) override
	{
		cameraController.OnUpdate(dt);
		scene->Update(dt);

		if (Input::IsDown(CR_MOUSE_BUTTON_LEFT))
			Entities::CreateEntity(EntityPlacePosition(), Entities::EntityType::Cockroach);
		else if (Input::IsDown(CR_MOUSE_BUTTON_RIGHT))
		{
			Entities::CreateEntity(EntityPlacePosition(), Entities::EntityType::Tile);
			u32 roomPlacePosition = EntityPlacePosition().x / 8 + EntityPlacePosition().y / 8 * room->width;

			if (0 <= roomPlacePosition && roomPlacePosition < sizeof(char) * room->width * room->height)
				room->data[roomPlacePosition] = 'B';

			room->Save("assets/scenes/room1.txt");
		}
	}

	float2 EntityPlacePosition()
	{
		int2 mousePos = { (int)Cockroach::Input::GetMouseX(), (int)Cockroach::Input::GetMouseY() };
		float2 worldPos = cameraController.camera.ScreenToWorldCoord(mousePos);
		float2 worldCoord = cameraController.camera.ScreenToWorldCoord(mousePos);
		float2 entityCenteredCoord = { worldCoord.x, worldCoord.y};
		return float2(std::floor(entityCenteredCoord.x/8) * 8, std::floor(entityCenteredCoord.y/8) * 8);
	}

	virtual void Render() override
	{
		Cockroach::Renderer::ResetStats();

		Cockroach::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Cockroach::Renderer::Clear();

		Cockroach::Renderer::BeginScene(cameraController.camera);

		RenderGrid();
		scene->Render();

		Cockroach::Renderer::DrawQuad(EntityPlacePosition(), {cursorSprite->XSize(), cursorSprite->YSize()}, cursorSprite);

		for (auto& ent : scene->entities)
		{
			Cockroach::Ref<Hitbox> h = ent->GetComponent<Hitbox>();
			if (h)
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

		auto stats = Cockroach::Renderer::GetStats();
		Text("Draw Calls: %d", stats.DrawCalls);
		Text("Quad Count: %d", stats.QuadCount);
		Text("Vertex Count: %d", stats.GetTotalVertexCount());
		Text("Index Count: %d", stats.GetTotalIndexCount());

		Spacing();

		Text("Entity Count: %i", scene->entities.size());
		End();

		Application::ImGuiEnd();
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
};

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Sandbox();
}