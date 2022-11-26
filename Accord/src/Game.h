#pragma once
#include <Cockroach.h>

#include "Entities/Player.h"
#include "Entities/Particles.h"

using namespace Entities;

enum EntityType
{
	Payga, Camera, Particles, SpikeLeft, SpikeRight, SpikeDown, SpikeUp, Oscillator, TurbineLeft, TurbineRight, TurbineDown, TurbineUp, EssenceRed, Igniter, END
};

class Game : public Cockroach::Application
{
public:
	Game();

	virtual void Update(float dt) override;
	virtual void Render() override;
	void ImGuiRender();

	static void RenderGrid();
	void RenderCursor();
	static void RenderHitboxes();

	static CameraController* cameraController;
	static Entities::Player* player;
	static Entities::Particles* particles;

	static Ref<Texture2D> baseSpriteSheet;
	static Ref<Texture2D> background;

	static std::vector<Ref<Room>> rooms;
private:
	bool renderGrid = true;
	bool renderHitboxes = true;
	bool renderAllRooms = false;
	bool renderRoomBoundaries = false;

	bool isBoxPlacing = false;
	int2 boxPlaceStartPos = { 0.0f, 0.0f };
};