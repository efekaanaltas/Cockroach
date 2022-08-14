#pragma once
#include <Cockroach.h>

class Game : public Cockroach::Application
{
public:
	Game();

	virtual void Update(float dt) override;
	virtual void Render() override;
	void ImGuiRender();
	Entity* GetEntityAtPosition(float2 position);
	float2 EntityPlacePosition();

	void RenderGrid();
	void RenderCursor();
	void RenderHitboxes();

	static Ref<CameraController> cameraController;
	static Ref<Player> player;

	static Ref<Texture2D> baseSpriteSheet;

private:
	bool renderGrid = true;
	bool renderHitboxes = true;

	bool isBoxPlacing = false;
	int2 boxPlaceStartPos = { 0.0f, 0.0f };
};