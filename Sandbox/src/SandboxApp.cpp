#include <Cockroach.h>
#include <Cockroach/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Input.h"
#include "Components.h"
#include "Entities.h"

class Sandbox : public Cockroach::Application
{
public:
	Sandbox()
		: Application(), m_CameraController(16.0f / 9.0f), m_Transform(0.0f)
	{
		m_Scene = Cockroach::CreateRef<Cockroach::Scene>();
		m_Scene->Load();

		cursorSprite = m_Scene->GetSubTexture("assets/textures/SpriteSheet.png", { 0, 0 }, { 8, 8 });
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
		m_CameraController.OnUpdate(dt);
		m_Scene->Update(dt);
		::Input::input->Update();
	}

	glm::vec2 EntityPlacePosition()
	{
		glm::ivec2 mousePos = { (int)Cockroach::Input::GetMouseX(), (int)Cockroach::Input::GetMouseY() };
		glm::vec2 worldPos = m_CameraController.camera.ScreenToWorldCoord(mousePos);
		glm::vec2 worldCoord = m_CameraController.camera.ScreenToWorldCoord(mousePos);
		glm::vec2 entityCenteredCoord = { worldCoord.x, worldCoord.y};
		return glm::vec2((int)entityCenteredCoord.x/8 * 8, (int)entityCenteredCoord.y/8 * 8);
	}

	virtual void Render() override
	{
		Cockroach::Renderer::ResetStats();

		Cockroach::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Cockroach::Renderer::Clear();

		Cockroach::Renderer::BeginScene(m_CameraController.camera);

		RenderGrid();
		m_Scene->Render();

		Cockroach::Renderer::DrawQuad(EntityPlacePosition(), {cursorSprite->XSize(), cursorSprite->YSize()}, cursorSprite);

		for (auto& ent : m_Scene->entities)
		{
			Cockroach::Ref<Hitbox> h = ent->GetComponent<Hitbox>();
			if (h)
				DrawHitbox(h);
		}

		Cockroach::Renderer::EndScene();

		ImGuiRender();
	}

	virtual void OnEvent(Cockroach::Event& e) override
	{
		m_CameraController.OnEvent(e);
		::Input::input->OnEvent(e);
		Cockroach::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Cockroach::MouseButtonPressedEvent>(CR_BIND_EVENT_FN(Sandbox::OnMouseButtonPressed));
	}

	bool OnMouseButtonPressed(Cockroach::MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == CR_MOUSE_BUTTON_LEFT)
			Entities::Cockroach(EntityPlacePosition());
		if (e.GetMouseButton() == CR_MOUSE_BUTTON_RIGHT)
			Entities::Tile(EntityPlacePosition());

		return false;
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

		Text("Entity Count: %i", m_Scene->entities.size());
		End();

		Application::ImGuiEnd();
	}

	void RenderGrid()
	{
		for (int i = -80; i < 80; i++)
		{
			int xFloor = (int)m_CameraController.camera.GetPosition().x;
			float xColor = ((i + xFloor) % 8 == 0) ? 0.3f : 0.2f;

			int yFloor = (int)m_CameraController.camera.GetPosition().y;
			float yColor = (i + yFloor) % 8 == 0 ? 0.3f : 0.2f;

			Cockroach::Renderer::DrawLine({ i + xFloor, -80.0f + yFloor, 0.0f }, { i + xFloor, 79.0f + yFloor, 0.0f }, { xColor, xColor, xColor, 0.5f });
			Cockroach::Renderer::DrawLine({ -80.0f + xFloor, i + yFloor, 0.0f }, { 79.0f + xFloor, i + yFloor, 0.0f }, { yColor, yColor, yColor, 0.5f });
		}
	}
	
private:
	Cockroach::CameraController m_CameraController;
	glm::vec3 m_Transform;
	Cockroach::Ref<Cockroach::Texture2D> m_Texture;
	Cockroach::Ref<Cockroach::SubTexture2D> cursorSprite;
	Cockroach::Ref<Cockroach::Scene> m_Scene;
};

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Sandbox();
}