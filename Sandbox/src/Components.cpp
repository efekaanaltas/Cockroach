#pragma once

#include "Cockroach.h"
#include "Components.h"

#include "State.h"

using namespace Cockroach;

std::vector<Hitbox*> Hitbox::all = std::vector<Hitbox*>();

Hitbox::Hitbox()
{
	Hitbox::all.push_back(this);
}

bool Hitbox::OverlapsWith(Hitbox other, float xForesense, float yForesense)
{
	if (!enabled || !other.enabled) return false;
	bool x = (Left() + xForesense < other.Right()) && (Right() + xForesense > other.Left());
	bool y = (Bottom() + yForesense < other.Top()) && (Top() + yForesense > other.Bottom());
	return x && y;
}

bool Hitbox::Contains(Hitbox other)
{
	if (!enabled || !other.enabled) return false;
	bool x = (Left() <= other.Left()) && (other.Right() <= Right());
	bool y = (Bottom() <= other.Bottom()) && (other.Top() <= Top());
	return x && y;
}

bool Hitbox::Contains(int2 coord)
{
	if (!enabled) return false;
	bool x = Left() <= coord.x && coord.x <= Right();
	bool y = Bottom() <= coord.y && coord.y <= Top();
	return x && y;
}

Player::Player()
{
	walkingState =		new WalkingState;
	jumpingState =		new JumpingState(100.0f, 160.0f, 0.0f);
	superjumpingState = new JumpingState(100.0f, 150.0f, 150.0f);
	walljumpingState =  new JumpingState(100.0f, 130.0f, -140.0);
	climbingState =		new JumpingState(100.0f, 160.0f, 0.0f);
	clingingState =		new ClingingState;
	dashingState =		new DashingState;
	
	currentState = walkingState;
	currentState->Enter(this);
}

void ::Player::Update(float dt)
{
	velocityLastFrame = velocity;

	if (Input::IsDown(CR_KEY_SPACE)) jumpBufferTimer.Reset();
	else							 jumpBufferTimer.Tick(1.0f);

	if (grounded) coyoteTimer.Reset();
	else		  coyoteTimer.Tick(1.0f);

	TrySwitchState(currentState->Update(this, dt));

	if (velocity.x != 0.0f) // Use InputDirX() instead?
		faceDir = velocity.x < 0.0f ? -1 : 1;
	i8 horizontalCollisions = MoveX(velocity.x * dt);
	i8 verticalCollisions = MoveY((velocityLastFrame.y + velocity.y) * 0.5f * dt); // Verlet integrated vertical motion

	grounded = verticalCollisions == -1;

	if (horizontalCollisions != 0)
		TrySwitchState(clingingState);
}

i8 Player::MoveX(float amount)
{
	xRemainder += amount;
	i32 move = (i32)xRemainder;

	if (move != 0)
	{
		xRemainder -= move;
		i8 sign = move > 0 ? 1 : -1;

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
				return sign;
			}
		}

		return 0;
	}
}

i8 Player::MoveY(float amount)
{
	yRemainder += amount;
	i32 move = (i32)yRemainder;

	i8 sign = move > 0 ? 1 : -1;
	yRemainder -= move;

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
			return sign;
		}
	}

	if (GetCollidingHitbox(0, sign)) return sign;
	else return 0;
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

i8 Player::InputDirX() const
{
	// Perhaps some kind of overwrite system where if both inputs are pressed, the latest pressed one takes precedence?
	i8 inputDir = 0;
	if (Input::IsPressed(CR_KEY_LEFT))  inputDir = -1;
	if (Input::IsPressed(CR_KEY_RIGHT)) inputDir = +1;
	return inputDir;
}

i8 Player::InputDirY() const
{
	i8 inputDir = 0;
	if (Input::IsPressed(CR_KEY_DOWN))  inputDir = -1;
	if (Input::IsPressed(CR_KEY_UP))	inputDir = +1;
	return inputDir;
}

bool Player::NextToWall()
{
	return GetCollidingHitbox(-1, 0) || GetCollidingHitbox(1, 0);
}

void Player::TrySwitchState(State<Player>* newState)
{
	if (newState == nullptr) return;
	currentState->Exit(this);
	currentState = newState;
	currentState->Enter(this);
}

std::vector<StaticObject*> StaticObject::all = std::vector<StaticObject*>();

StaticObject::StaticObject()
{
	StaticObject::all.push_back(this);
}
