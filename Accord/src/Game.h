#pragma once
#include <Cockroach.h>

#include "Entities/Player.h"

enum EntityType
{
	Payga, Camera, SpikeLeft, SpikeRight, SpikeDown, SpikeUp, Oscillator, TurbineLeft, TurbineRight, TurbineDown, TurbineUp, END
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
	static Player* player;

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