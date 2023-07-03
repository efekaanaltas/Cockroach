#pragma once
#include <Cockroach.h>

#include "Entities/Player.h"
#include "Entities/Particles.h"
#include "PlayerData.h"

using namespace Entities;

// As far as I'm aware, C++ doesn't have a simple way to get the name of an enum value, and having to manually update a string array when adding a new entity is cumbersome.
// We use a macro that generates the corresponding string list given the enum definition.
#define DECLARE_ENTITY_TYPE_ENUM(ENUM_NAME, ...)  \
	enum ENUM_NAME{ __VA_ARGS__ }; \
	static std::vector<std::string> entityTypeNames = Cockroach::Split( #__VA_ARGS__ );

DECLARE_ENTITY_TYPE_ENUM(EntityType, Payga, Camera, Particles, SpikeLeft, SpikeRight, SpikeDown, SpikeUp, Oscillator, TurbineLeft, TurbineRight, TurbineDown, TurbineUp, Essence, Igniter, Propeller, MovingPlatform, Attractor, Checkpoint, END);

class Game : public Cockroach::Application
{
public:
	Game();

	virtual void Update(float dt) override;
	virtual void Render() override;
	void ImGuiRender();

	void ShowSpriteEditor(bool* open);

	static void RenderGrid();
	static void RenderHitboxes();

	void SaveSprites();
	void LoadSprites();

	static PlayerData data;

	static CameraController* cameraController;
	static Entities::Player* player;
	static Entities::Particles* particles;

	static Ref<Framebuffer> framebuffer;
	static Ref<Texture2D> baseSpriteSheet;
	static Ref<Texture2D> tilemapSheet;
	static Ref<Texture2D> background;

	static std::vector<Sprite> entitySprites;
	static std::vector<Sprite> decorationSprites;

	static std::vector<Ref<Room>> rooms;

	static void Freeze(int frames);

	static bool editMode;
private:
	static Timer freezeTimer;

	bool renderGrid = false;
	bool renderHitboxes = false;
	bool renderAllRooms = false;
	bool renderRoomBoundaries = false;
};