#pragma once
#include <Cockroach.h>

#include "Entities/Player.h"
#include "Entities/Particles.h"

using namespace Entities;

// As far as I'm aware, C++ doesn't have a simple way to get the name of an enum value, and having to manually update a string array when adding a new entity is cumbersome.
// The code below generates a string list for the EntityType enum. It may look kind of confusing, but it's the best I could do.
inline std::vector<std::string> DeclareEntityTypeNames(const std::string& entityTypeEnumDefinition)
{
	std::vector<std::string> entityTypeNames;

	char delim = ',';
	std::size_t start{};
	std::size_t end = entityTypeEnumDefinition.find(delim);
	while (end != std::string::npos)
	{
		while (entityTypeEnumDefinition[start] == ' ') ++start;
		entityTypeNames.push_back(entityTypeEnumDefinition.substr(start, end - start));
		start = end + 1;
		end = entityTypeEnumDefinition.find(delim, start);
	}
	while (entityTypeEnumDefinition[start] == ' ') ++start;
	entityTypeNames.push_back(entityTypeEnumDefinition.substr(start));
	return entityTypeNames;
}

#define DECLARE_ENTITY_TYPE_ENUM(ENUM_NAME, ...)  \
	enum ENUM_NAME{ __VA_ARGS__ }; \
	static std::vector<std::string> entityTypeNames = DeclareEntityTypeNames( #__VA_ARGS__ );

DECLARE_ENTITY_TYPE_ENUM(EntityType, Payga, Camera, Particles, SpikeLeft, SpikeRight, SpikeDown, SpikeUp, Oscillator, TurbineLeft, TurbineRight, TurbineDown, TurbineUp, EssenceRed, Igniter, Propeller, END);

class Game : public Cockroach::Application
{
public:
	Game();

	virtual void Update(float dt) override;
	virtual void Render() override;
	void ImGuiRender();

	static void RenderGrid();
	static void RenderHitboxes();

	static CameraController* cameraController;
	static Entities::Player* player;
	static Entities::Particles* particles;

	static Ref<Texture2D> baseSpriteSheet;
	static Ref<Texture2D> background;

	static std::vector<Ref<Room>> rooms;

	static void Freeze(int frames);

private:
	static Timer freezeTimer;

	bool renderGrid = true;
	bool renderHitboxes = true;
	bool renderAllRooms = false;
	bool renderRoomBoundaries = false;
};