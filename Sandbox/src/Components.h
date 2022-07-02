#pragma once

#include "Cockroach.h"

using namespace Cockroach;

class Hitbox : public Component
{
public:
	static std::vector<Hitbox*> all;

	Hitbox();
	~Hitbox() {}
	virtual void Update(float dt) override {}

	glm::ivec2 min = { 0, 0 };
	glm::ivec2 max = { 8, 8 };

	glm::ivec2 Hitbox::Position() const { return entity->position; }

	int32_t Hitbox::Left() const { return Position().x + min.x; }
	int32_t Hitbox::Right() const { return Position().x + max.x; }
	int32_t Hitbox::Bottom() const { return Position().y + min.y; }
	int32_t Hitbox::Top() const { return Position().y + max.y; }

	bool OverlapsWith(Hitbox other, float xForesense = 0, float yForesense = 0); // TODO: Const reference of Hitbox can't call functions such as Left(), why?
	bool Contains(Hitbox other);
};

class Player : public Component
{
public:
	Player() {}
	~Player() {}
	virtual void Update(float dt) override;

	float xRemainder = 0.0f, yRemainder = 0.0f;
	float gravity = 100.0f;
	glm::vec2 velocity = { 0.0f, 0.0f };

	void MoveX(float amount);
	void MoveY(float amount);
	Hitbox* GetCollidingHitbox(int xForesense, int yForesense);
};