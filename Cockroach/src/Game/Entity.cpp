#include "crpch.h"
#include "Entity.h"

#include "../Core/Utility.h"
#include "Renderer/Renderer.h"

#include "imgui.h"

namespace Cockroach
{
	int Entity::lastID = 0;

	Entity::Entity()
		: Entity({ 0,0 })
	{}

	Entity::Entity(const int2& position)
	{
		this->position = position;
		ID = lastID++;
	}

	void Entity::Render()
	{
		Renderer::DrawQuad(float3(position, z), { sprite.XSize(), sprite.YSize() }, sprite, { overlayColor, overlayWeight }, flipX, flipY);
	}

	void Entity::RenderInspectorUI()
	{
		using namespace ImGui;
		Begin("Inspector");
		InputInt("X", &position.x, 8, 1);
		InputInt("Y", &position.y, 8, 1);
		InputInt("W", &size.x, 8, 8);
		InputInt("H", &size.y, 8, 8);
		End();
	}

	std::string Entity::GenerateDefinitionString()
	{
		return GenerateProperty("E", type)
			 + GenerateProperty("X", position.x) + GenerateProperty("Y", position.y)
			 + GenerateProperty("W", size.x) + GenerateProperty("H", size.y) + "\n";
	}

	EntityDefinition::EntityDefinition(std::stringstream& definition)
	{
		this->isDecoration = HasProperty(definition, "D");
		this->type = isDecoration ? GetProperty<int>(definition, "D") : GetProperty<int>(definition, "E");
		this->position = { GetProperty<int>(definition, "X"), GetProperty<int>(definition, "Y") };
		this->size = { GetProperty<int>(definition, "W"), GetProperty<int>(definition, "H")};

		if (HasProperty(definition, "Z"))
			this->z = GetProperty<int>(definition, "Z");
		if(HasProperty(definition, "X1"))
			this->altPosition = int2(GetProperty<int>(definition, "X1"), GetProperty<int>(definition, "Y1"));
	}

	EntityDefinition::EntityDefinition(int type, bool isDecoration, int2 position, int2 size)
	{
		this->type = type;
		this->isDecoration = isDecoration;
		this->position = position;
		this->size = size;
	}
}