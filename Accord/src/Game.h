#pragma once
#include <Cockroach.h>
#include "EntityDef.h"
#include "Entities/Player.h"
#include "Entities/Particles.h"
#include "PlayerData.h"

using namespace Entities;

class Game : public Cockroach::Application
{
public:
	Game();
	virtual ~Game() override;

	virtual void Update() override;
	virtual void Render() override;
	void ImGuiRender();
	void ExampleGameUI();
	void ShowExampleAppSimpleOverlay();

	void ShowSpriteEditor(bool* open);

	static void RenderGrid();
	static void RenderHitboxes();

	void SaveSprites();
	void LoadSprites();

	void BeginKeyRebind(InputAction action);

	static PlayerData data;

	static CameraController* cameraController;
	static Entities::Player* player;
	static Entities::Particles* particles;

	static Ref<Framebuffer> framebuffer;
	static Ref<Framebuffer> distortionFramebuffer;
	static Ref<Texture2D> baseSpriteSheet;
	static Ref<Texture2D> tilemapSheet;
	static Ref<Texture2D> background;

	static vector<Sprite> entitySprites;
	static vector<Sprite> decorationSprites;

	static vector<Ref<Room>> rooms;

	static void Freeze(int frames);

	static bool editMode;
private:
	Timer freezeTimer = Timer(0.0f, frames, true);
	Timer saveTimer = Timer(60.0f, seconds, true);

	bool renderGrid = false;
	bool renderHitboxes = false;
	bool renderAllRooms = false;
	bool renderRoomBoundaries = false;
	bool showInfo = true;
};