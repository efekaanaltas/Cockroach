#pragma once

#include "Cockroach.h"
#include "Components.h"

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

	if (Input::IsDown(CR_KEY_SPACE))
		velocity.y = 100;

	if (Input::IsPressed(CR_KEY_LEFT))
		velocity.x -= 1000 * dt;
	else if (Input::IsPressed(CR_KEY_RIGHT))
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
	MoveY(velocity.y * dt);
}

void Player::MoveX(float amount)
{
	xRemainder += amount;
	i32 move = (i32)xRemainder;

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
			else
			{
				velocity.x = 0;
				break;
			}
		}
	}
}

void Player::MoveY(float amount)
{
	yRemainder += amount;
	i32 move = (i32)yRemainder;

	if (move != 0)
	{
		yRemainder -= move;
		i32 sign = move > 0 ? 1 : -1;

		while (move != 0)
		{
			Hitbox* collidingHitbox = GetCollidingHitbox(0, sign);
			if (!collidingHitbox)
			{
				entity->position.y += sign;
				move -= sign;
			}
			else
			{
				velocity.y = 0;
				break;
			}
		}
	}
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

std::vector<StaticObject*> StaticObject::all = std::vector<StaticObject*>();

StaticObject::StaticObject()
{
	StaticObject::all.push_back(this);
}
