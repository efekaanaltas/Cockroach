#include <Cockroach.h>
#include <Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EntityDef.h"
#include "EditorCursor.h"

#include "Game.h"

#include <filesystem>
#include <fstream>

CameraController* Game::cameraController = nullptr;
Player* Game::player = nullptr;
Entities::Particles* Game::particles = nullptr;

Ref<Framebuffer> Game::framebuffer = nullptr;
Ref<Texture2D> Game::baseSpriteSheet = nullptr;
Ref<Texture2D> Game::background = nullptr;

std::vector<Sprite> Game::entitySprites;
std::vector<Sprite> Game::decorationSprites;

std::vector<Ref<Room>> Game::rooms;

Timer Game::freezeTimer = Timer(0.0f);

bool Game::editMode = true;

Game::Game()
	: Application()
{
	Game::framebuffer = CreateRef<Framebuffer>(320, 180);
	Game::background = CreateRef<Texture2D>("assets/textures/BG_RED.png");
	Game::baseSpriteSheet = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");

	//for (int i = EntityType::SpikeLeft; i < EntityType::END; i++)
	//{
	//	Entity* e = CreateEntity(ZEROi, ONEi, i);
	//	if (entitySprites.size() < i + 1)
	//		entitySprites.resize(i + 1);
	//	entitySprites[i] = e->sprite;
	//}
	//SaveSprites();

	LoadSprites();

	const std::filesystem::path roomDir{ "assets/rooms" };
	for (auto& a : std::filesystem::directory_iterator(roomDir))
	{
		std::string name = a.path().string().substr(a.path().string().find_last_of("/\\") + 1);
		rooms.push_back(Room::Load(name));
	}

	Game::cameraController = new CameraController();
	cameraController->type = EntityType::Camera;
	cameraController->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0,0 }, { 1,1 });

	Game::player = new Entities::Player({-72, 32}, {6,0}, {10,12});
	player->type = EntityType::Payga;
	player->sprite = Sprite::CreateFromCoords(baseSpriteSheet, { 0, 3 }, { 16, 16 });

	Game::particles = new Entities::Particles();
	particles->type = EntityType::Particles;

	for (auto& room : rooms)
		if (room->Contains(player->WorldHitbox()))
			Room::current = room;
}

void Game::Update(float dt)
{
	if (!freezeTimer.Finished())
	{
		freezeTimer.Tick(1.0f);
		return;
	}

	cameraController->Update(dt);
	if (!editMode)
	{
		player->Update(dt);
		particles->Update(dt);
	}
	Room::current->Update(dt);

	EditorCursor::Update(dt);

	if (Input::IsDown(CR_KEY_TAB))
	{
		cameraController->SetZoom(90.0f);
		editMode = !editMode;
	}

	static bool muted = false;

	if (Input::IsDown(CR_KEY_F))
		Application::GetWindow().SetWindowMode(!Application::GetWindow().fullscreen);
	if (Input::IsDown(CR_KEY_M))
		Audio::ToggleSound(muted = !muted);
}

#pragma warning (disable: 4244) // No need for DrawQuad... warnings

void Game::Render()
{
	if (!editMode)
	{
		framebuffer->Bind();
		glViewport(0, 0, 320, 180);
	}

	Renderer::SetClearColor({ 0.1f, 0.0f, 0.0f, 1 });
	Renderer::Clear();
	glEnable(GL_DEPTH_TEST);

	Renderer::BeginScene(cameraController->camera);

	if (Input::IsDown(CR_KEY_G)) renderGrid = !renderGrid;
	if (Input::IsDown(CR_KEY_H)) renderHitboxes = !renderHitboxes;
	if (Input::IsDown(CR_KEY_R)) renderAllRooms = !renderAllRooms;

	float zoom = cameraController->zoom;
	float aspect = cameraController->aspectRatio;

	Sprite backgroundSprite = Sprite(background, { 0,0 }, { 1,1 });
	Renderer::DrawQuad(float3(cameraController->positionHighRes, -90.0f)-float3(aspect*zoom,zoom,0), 2.0f * float2(aspect*zoom,zoom), backgroundSprite, { 0,0,0,0 }, false, false); // Turn this into a dedicated function: RenderBackground(Sprite background)?
	
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
				Renderer::DrawQuadOutline(bounds.min.x, bounds.max.x, bounds.min.y, bounds.max.y, GREEN);
			}
		}
	}

	player->Render();
	particles->Render();

	if (renderHitboxes) RenderHitboxes();

	for (auto& ent : Room::current->entities)
	{
		if (renderHitboxes && ent->type >= EntityType::TurbineLeft && ent->type <= EntityType::TurbineUp)
		{
			Renderer::DrawQuadOutline(ent->As<Turbine>()->turbineRect.min.x, ent->As<Turbine>()->turbineRect.max.x, ent->As<Turbine>()->turbineRect.min.y, ent->As<Turbine>()->turbineRect.max.y, GREEN);
		}
	}

	EditorCursor::Render();

	Renderer::EndScene();
	if (!editMode)
	{
		glViewport(0, 0, Application::Get().GetWindow().width, Application::Get().GetWindow().height );
		framebuffer->Unbind();
		Renderer::BlitToScreen(framebuffer);
	}

#if CR_DEBUG
	if(editMode)
		ImGuiRender();
#endif
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
	static char nameBuffer[255];
	if (strcmp(nameBuffer, Room::current->name.c_str()))
		strcpy_s(nameBuffer, 255, Room::current->name.c_str());

	InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer));
	
	if (strcmp(nameBuffer, Room::current->name.c_str()))
		Room::current->Rename(nameBuffer);

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
	if (Button("Save All"))
		for (auto& room : rooms)
			room->Save();

	End();

	Begin("Brush Settings");

	int* brushModeIntPtr = (int*)&EditorCursor::brushMode;
	const char* elems_names[3] = { "Tile", "Entity", "Room" };
	const char* elem_name = elems_names[*brushModeIntPtr];
	SliderInt("Mode", brushModeIntPtr, 0, 2, elem_name);

	if (EditorCursor::brushMode == EditorCursor::Tile)
	{
		EditorCursor::tileType = Room::TileBasic;
	}
	if (EditorCursor::brushMode == EditorCursor::Entity)
		for (int i = EntityType::Particles + 1; i < EntityType::END; i++)
			if(ImGui::Button(entityTypeNames[i].c_str(), {200,20}))
				EditorCursor::entityType = i;
		
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

		Cockroach::Renderer::DrawLine({ i + xFloor, -80.0f + yFloor, -1.0f }, { i + xFloor, 79.0f + yFloor, -1.0f }, { xColor, xColor, xColor, 0.5f });
		Cockroach::Renderer::DrawLine({ -80.0f + xFloor, i + yFloor, -1.0f }, { 79.0f + xFloor, i + yFloor, -1.0f }, { yColor, yColor, yColor, 0.5f });
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
		if (Room::current->tiles[i] == Room::TileBasic)
		{
			float2 worldPos = Room::current->RoomToWorldPosition(Room::current->IndexToRoomPosition(i));
			Renderer::DrawQuadOutline(worldPos.x, worldPos.x + 8.0f, worldPos.y, worldPos.y + 8.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
		}
	}
}

void Game::Freeze(int frames)
{
	freezeTimer = Timer(frames);
}

void Game::SaveSprites()
{
	std::string filepath = "assets/textures/sprites.txt";
	std::fstream out(filepath, std::ios::out | std::ios::binary | std::ios::trunc);

	if (out)
	{
		for (int i = 0; i < entitySprites.size(); i++)
		{
			out << GenerateProperty("E", i);
			out << GenerateProperty("X0", entitySprites[i].min.x);
			out << GenerateProperty("Y0", entitySprites[i].min.y);
			out << GenerateProperty("X1", entitySprites[i].max.x);
			out << GenerateProperty("Y1", entitySprites[i].max.y);
			out << '\n';
		}
		for (int i = 0; i < decorationSprites.size(); i++)
		{
			out << GenerateProperty("D", i);
			out << GenerateProperty("X0", entitySprites[i].min.x);
			out << GenerateProperty("Y0", entitySprites[i].min.y);
			out << GenerateProperty("X1", entitySprites[i].max.x);
			out << GenerateProperty("Y1", entitySprites[i].max.y);
			out << '\n';
		}
	}
	else CR_CORE_ERROR("Could not open file '{0}'", filepath);
	out.close();
}

void Game::LoadSprites()
{
	std::string filepath = "assets/textures/sprites.txt";
	std::fstream in(filepath, std::ios::in || std::ios::binary);

	if (in)
	{
		std::string line;

		while (std::getline(in, line))
		{
			std::stringstream stream(line);
			int type;

			float2 min = { GetProperty<float>(stream, "X0"), GetProperty<float>(stream, "Y0") };
			float2 max = { GetProperty<float>(stream, "X1"), GetProperty<float>(stream, "Y1") };
			Sprite sprite = Sprite(Game::baseSpriteSheet, min, max);

			if (HasProperty(stream, "E"))
			{
				type = GetProperty<int>(stream, "E");
				if (entitySprites.size() < type + 1) 
					entitySprites.resize(type + 1);
				entitySprites[type] = sprite;
			}
			else if(HasProperty(stream, "D"))
			{
				type = GetProperty<int>(stream, "D");
				if (entitySprites.size() < type + 1) 
					entitySprites.resize(type + 1);
				decorationSprites[type] = sprite;
			}
		}
	}
	else CR_CORE_ERROR("Could not open file '{0}'", filepath);
	in.close();
}

Cockroach::Application* Cockroach::CreateApplication()
{
	return new Game();
}