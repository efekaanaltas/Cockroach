#include <Cockroach.h>
#include <Cockroach/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Input.h"
#include "Components.h"

class Sandbox : public Cockroach::Application
{
public:
	Sandbox()
		: Application(), m_CameraController(16.0f / 9.0f), m_Transform(0.0f)
	{
		m_Texture = Cockroach::CreateRef<Cockroach::Texture2D>("assets/textures/SpriteSheet.png");
		m_SubTex = Cockroach::SubTexture2D::CreateFromCoords(m_Texture, { 0, 3 }, { 16, 16 });

		m_Scene = Cockroach::CreateRef<Cockroach::Scene>();
		m_Scene->Load();
	}

	void DrawHitbox(Cockroach::Ref<Hitbox> h)
	{
		glm::vec3 bottomLeft = { h->Left(), h->Bottom(), 0.0f};
		glm::vec3 bottomRight = { h->Right(), h->Bottom(), 0.0f };
		glm::vec3 topLeft = { h->Left(), h->Top(), 0.0f };
		glm::vec3 topRight = { h->Right(), h->Top(), 0.0f };
		Cockroach::Renderer::DrawLine(bottomLeft, topLeft, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine(topLeft, topRight, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine(topRight, bottomRight, { 1.0f, 0.0f, 0.0f, 1.0f });
		Cockroach::Renderer::DrawLine(bottomRight, bottomLeft, { 1.0f, 0.0f, 0.0f, 1.0f });
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
		glm::vec2 entityCenteredCoord = { worldCoord.x - m_SubTex->XSize() / 2.0f, worldCoord.y - m_SubTex->YSize() / 2.0f };
		glm::vec2 placePos = { (int)entityCenteredCoord.x, (int)entityCenteredCoord.y };
		return glm::vec2((int)placePos.x/8 * 8, (int)placePos.y/8 * 8);
	}

	virtual void Render() override
	{
		Cockroach::Renderer::ResetStats();

		Cockroach::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Cockroach::Renderer::Clear();

		Cockroach::Renderer::BeginScene(m_CameraController.camera);

		RenderGrid();
		m_Scene->Render();

		Cockroach::Renderer::DrawQuad(EntityPlacePosition(), {m_SubTex->XSize(), m_SubTex->YSize()}, m_SubTex);

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
		//dispatcher.Dispatch<KeyPressedEvent>(std::bind(::Input::OnEvent, this, std::placeholders::_1));
	}

	bool OnMouseButtonPressed(Cockroach::MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == CR_MOUSE_BUTTON_LEFT)
		{
			Cockroach::Ref<Cockroach::Entity> ent = m_Scene->AddEntity(EntityPlacePosition());
			ent->sprite = m_SubTex;
			Cockroach::Ref<Hitbox> hitbox = ent->AddComponent<Hitbox>();
			hitbox->min = { 6, 0 };
			hitbox->max = { 10, 10 };
			ent->AddComponent<::Player>();
		}
		if (e.GetMouseButton() == CR_MOUSE_BUTTON_RIGHT)
		{
			Cockroach::Ref<Cockroach::Entity> ent = m_Scene->AddEntity(EntityPlacePosition());
			ent->sprite = Cockroach::SubTexture2D::CreateFromCoords(m_Texture, { 11,2 }, { 8,8 });
			ent->AddComponent<Hitbox>();
		}

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

		for (uint32_t i = 0; i < ::Input::TRACKED_KEY_COUNT; i++)
			Text("%i: %i, %i", ::Input::input->trackedKeys[i].keycode, ::Input::input->trackedKeys[i].pressed, ::Input::input->trackedKeys[i].pressedLastFrame);

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
	Cockroach::Ref<Cockroach::SubTexture2D> m_SubTex;
	Cockroach::Ref<Cockroach::Scene> m_Scene;
	Cockroach::Ref<Hitbox> m_Hitbox;
};

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Sandbox();
}