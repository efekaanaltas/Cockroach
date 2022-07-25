#include "crpch.h"
#include "State.h"

#include "Components.h"

Sheet GetWalkingStateSheet(Player* player)
{
	if (!player->grounded && player->velocity.y < 0.0f)  return player->fallingSheet;
	else if (player->velocity.x != 0.0f)				 return player->walkingSheet;
	else												 return player->idleSheet;
}

void WalkingState::Enter(Player* player)
{
	player->coyoteTimer.remainingTime = 0.0f;
}

State<Player>* WalkingState::Update(Player* player, float dt)
{
	player->animator->sheet = GetWalkingStateSheet(player);

	if (Input::IsDown(CR_KEY_SPACE) && !player->grounded && player->NextToWall())
				return player->walljumpingState;

	if (!player->coyoteTimer.Finished() && !player->jumpBufferTimer.Finished())
		return player->jumpingState;

	if (Input::IsDown(CR_KEY_LEFT_SHIFT))
		return player->dashingState;

	player->velocity.y -= gravity * dt;
	player->velocity.y = std::max(player->velocity.y, -maxFallSpeed);

	if (player->InputDirX() != 0)
	{
		player->velocity.x += player->InputDirX() * acceleration * dt;
		player->velocity.x = std::clamp(player->velocity.x, -maxWalkSpeed, maxWalkSpeed);
	}
	else
	{
		player->velocity.x -= player->faceDir * deceleration * dt;
		if (player->faceDir * player->velocity.x < 0.0f) player->velocity.x = 0.0f;
	}

	return nullptr;
}

void JumpingState::Enter(Player* player)
{
	WalkingState::Enter(player);

	player->velocity.x += horizontalBoost * player->faceDir;
	player->velocity.y = maxJumpSpeed;

	player->animator->sheet = player->jumpingSheet;
}

State<Player>* JumpingState::Update(Player* player, float dt)
{
	if (Input::IsUp(CR_KEY_SPACE))
		player->velocity.y = std::min(player->velocity.y, minJumpSpeed);
	else if (player->velocity.y <= 0)
		return player->walkingState;

	return WalkingState::Update(player, dt);
}

void ClingingState::Enter(Player* player)
{
	player->velocity.y = 0;

	player->animator->sheet = player->clingingSheet;
}

State<Player>* ClingingState::Update(Player* player, float dt)
{
	if (!player->NextToWall())
		return player->walkingState;

	if (Input::IsDown(CR_KEY_LEFT_SHIFT))
		return player->dashingState;

	if (!player->jumpBufferTimer.Finished())
	{
		if (Input::IsPressed(CR_KEY_UP)) return player->climbingState;
		else							 return player->walljumpingState;
	}

	if (Input::IsPressed(CR_KEY_UP)) player->velocity.y = 0.0f;
	else
	{
		player->velocity.y -= 500.0f * dt;
		player->velocity.y = std::clamp(player->velocity.y, -fallSpeed, 0.0f);
	}

	if (player->InputDirX() == -player->faceDir)
		return player->walkingState;

	return nullptr;
}

void DashingState::Enter(Player* player)
{
	dashTimer.Reset();

	player->velocity.y = 0;

	bool down = Input::IsPressed(CR_KEY_DOWN);
	bool up = Input::IsPressed(CR_KEY_UP);
	dashDir = { 0.0f, 0.0f };

	if (player->InputDirX() != 0 || player->InputDirY() != 0)
		dashDir = glm::normalize(float2(player->InputDirX(), player->InputDirY()));
	else dashDir = { player->faceDir, 0.0f };

	player->animator->sheet = player->dashingSheet;
}

State<Player>* DashingState::Update(Player* player, float dt)
{
	if (Input::IsDown(CR_KEY_SPACE) && player->GetCollision(0, -1).collided)
		return player->superjumpingState;

	if (dashTimer.Finished())
		return player->walkingState;
	else
	{
		dashTimer.Tick(dt);
		player->velocity = dashSpeed * dashDir;
	}


	return nullptr;
}

void DashingState::Exit(Player* player)
{
	player->velocity = { 0.0f,0.0f };
}