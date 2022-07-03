#pragma once

#include "Cockroach.h"
#include "Components.h"
#include "Input.h"

using namespace Cockroach;

std::vector<Hitbox*> Hitbox::all = std::vector<Hitbox*>();

bool Hitbox::OverlapsWith(Hitbox other, float xForesense, float yForesense)
{
	bool x = (Left() + xForesense < other.Right()) && (Right() + xForesense > other.Left());
	bool y = (Bottom() + yForesense < other.Top()) && (Top() + yForesense > other.Bottom());
	return x && y;
}


Hitbox::Hitbox()
{
	Hitbox::all.push_back(this);
}

bool Hitbox::Contains(Hitbox other)
{
	bool x = (Left() <= other.Left()) && (other.Right() <= Right());
	bool y = (Bottom() <= other.Bottom()) && (other.Top() <= Top());
	return x && y;
}

void ::Player::Update(float dt)
{
	velocity.y -= gravity * dt;

	if (::Input::input->GetKeyDown(CR_KEY_SPACE))
		velocity.y = 100;

	if (Cockroach::Input::IsKeyPressed(CR_KEY_LEFT))
		velocity.x -= 1000 * dt;
	else if (Cockroach::Input::IsKeyPressed(CR_KEY_RIGHT))
		velocity.x += 1000 * dt;
	else
	{
		int signBefore = std::signbit(velocity.x) ? -1 : 1;
		velocity.x -= signBefore * 700 * dt;
		int signAfter = std::signbit(velocity.x) ? -1 : 1;
		if (signBefore != signAfter)
			velocity.x = 0;
	}

	velocity.x = std::signbit(velocity.x) ? std::max(velocity.x, -50.0f) : std::min(velocity.x, 50.0f);

	MoveX(velocity.x * dt);
	int32_t verticalCollisionDirection = MoveY(velocity.y * dt);

	if (verticalCollisionDirection == -1) 
		velocity.y = 0;
}

int32_t Player::MoveX(float amount)
{
	xRemainder += amount;
	int32_t move = (int)xRemainder;

	if (move != 0)
	{
		xRemainder -= move;
		int32_t sign = move > 0 ? 1 : -1;

		while (move != 0)
		{
			Hitbox* collidingHitbox = GetCollidingHitbox(sign, 0);
			if (!collidingHitbox)
			{
				entity->position.x += sign;
				move -= sign;
			}
			else return sign;
		}
	}
	return 0;
}

int32_t Player::MoveY(float amount)
{
	yRemainder += amount;
	int32_t move = (int)yRemainder;

	if (move != 0)
	{
		yRemainder -= move;
		int32_t sign = move > 0 ? 1 : -1;

		while (move != 0)
		{
			Hitbox* collidingHitbox = GetCollidingHitbox(0, sign);
			if (!collidingHitbox)
			{
				entity->position.y += sign;
				move -= sign;
			}
			else return sign;
		}
	}
	return 0;
}

Hitbox* Player::GetCollidingHitbox(int xForesense, int yForesense)
{
	Ref<Hitbox> playerHitbox = entity->GetComponent<Hitbox>();
	for (auto& h : Hitbox::all)
		if (h != entity->GetComponent<Hitbox>().get())
			if (playerHitbox->OverlapsWith(*h, xForesense, yForesense))
				return h;
	return nullptr;
}