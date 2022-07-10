#include "crpch.h"
#include "State.h"

#include "Components.h"

State<Player>* WalkingState::Update(Player* player, float dt)
{
	if (Input::IsDown(CR_KEY_SPACE))
		jumpBufferTimer.Reset();
	if (Input::IsDown(CR_KEY_LEFT_SHIFT))
		return (State<Player>*)player->dashingState;
	if (player->grounded && !jumpBufferTimer.Finished())
		return (State<Player>*)player->jumpingState;

	player->velocity.y -= gravity * dt;
	player->velocity.y = std::max(player->velocity.y, -maxFallSpeed);

	if (player->InputDir() != 0)
	{
		player->velocity.x += player->InputDir() * acceleration * dt;
		player->velocity.x = std::clamp(player->velocity.x, -maxWalkSpeed, maxWalkSpeed);
	}
	else
	{
		player->velocity.x -= player->faceDir * deceleration * dt;
		player->velocity.x = std::max(player->faceDir * player->velocity.x, 0.0f);
	}

	if (player->grounded)
		groundedTimer.Reset();

	jumpBufferTimer.Tick(1.0f);
	groundedTimer.Tick(1.0f);

	return nullptr;
}

void JumpingState::Enter(Player* player)
{
	player->velocity.y = maxJumpSpeed;
}

State<Player>* JumpingState::Update(Player* player, float dt)
{
	player->velocity.y -= 120.0f * dt;

	if (Input::IsUp(CR_KEY_SPACE))
		player->velocity.y = std::min(player->velocity.y, minJumpSpeed);
	else if (player->velocity.y <= 0)
		return (State<Player>*)player->walkingState;

	return nullptr;
}

State<Player>* ClingingState::Update(Player* player, float dt)
{
	if (Input::IsDown(CR_KEY_LEFT_SHIFT))
	{
		player->faceDir *= -1;
		return (State<Player>*)player->dashingState;
	}

	if (Input::IsDown(CR_KEY_SPACE)) // TODO: Use jump buffer
	{
		if (Input::IsPressed(CR_KEY_UP)) return (State<Player>*)player->climbingState; // Climbing
		else return (State<Player>*)player->walljumpingState; // Walljump
	}

	if (!Input::IsPressed(CR_KEY_DOWN))
		player->velocity.y = -fallSpeed;

	if (player->InputDir() != player->faceDir)
		return (State<Player>*)player->walkingState;

	return nullptr;
}

void DashingState::Enter(Player* player)
{
	dashTimer.Reset();

	player->velocity.y = 0;

	if (player->InputDir() != 0)
		player->faceDir = player->InputDir();
}

State<Player>* DashingState::Update(Player* player, float dt)
{
	if (Input::IsDown(CR_KEY_SPACE) && player->grounded)
		return (State<Player>*)player->superjumpingState;

	if (dashTimer.Finished())
		return (State<Player>*)player->walkingState;
	else
	{
		player->velocity.x = dashSpeed * player->faceDir;

		// TODO: Add edge correction (DashingState.cs could help)
	}

	dashTimer.Tick(dt);

	return nullptr;
}
