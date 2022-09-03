#include "crpch.h"
#include "State.h"

#include "EntityDef.h"
#include "Entities/Player.h"

Sheet GetWalkingStateSheet(Player* player)
{
	if (!player->grounded)
	{
		if (player->velocity.y < 0.0f)		return player->fallingSheet;
		if (player->velocity.y >= 0.0f)		return player->jumpingSheet;
	}
	else if (player->velocity.x != 0.0f)	return player->walkingSheet;
											return player->idleSheet;
}

void WalkingState::Enter(Player* player)
{
	player->coyoteTimer.remainingTime = 0.0f;
	player->jumpBufferTimer.remainingTime = 0.0f;
}

State<Player>* WalkingState::Update(Player* player, float dt)
{
	player->currentSheet = GetWalkingStateSheet(player);

	if (Input::IsDown(CR_KEY_SPACE) && !player->grounded && player->WallDir())
	{
		player->faceDir = player->WallDir();
		return player->walljumpingState;
	}

	if (!player->coyoteTimer.Finished() && !player->jumpBufferTimer.Finished())
		return player->jumpingState;

	if (Input::IsDown(CR_KEY_LEFT_SHIFT) && player->canDash)
		return player->dashingState;

	player->velocity.y -= gravity * dt;
	player->velocity.y = std::max(player->velocity.y, -maxFallSpeed);

	if (player->InputDirX() != 0 && (std::abs(player->velocity.x) <= maxWalkSpeed))
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

	player->grounded = false;

	player->velocity.x += horizontalBoost * player->faceDir;
	player->velocity.y = maxJumpSpeed;

	player->currentSheet = player->jumpingSheet;
	Audio::Play("assets/audio/Jump.wav");
}

State<Player>* JumpingState::Update(Player* player, float dt)
{
	player->coyoteTimer.remainingTime = 0;

	if (Input::IsUp(CR_KEY_SPACE))
		player->velocity.y = std::min(player->velocity.y, minJumpSpeed);
	else if (player->velocity.y <= 0)
		return player->walkingState;

	return WalkingState::Update(player, dt);
}

void ClingingState::Enter(Player* player)
{
	player->velocity.y = 0;

	player->currentSheet = player->clingingSheet;
}

State<Player>* ClingingState::Update(Player* player, float dt)
{
	if (!player->WallDir())
		return player->walkingState;

	if (Input::IsDown(CR_KEY_LEFT_SHIFT) && player->canDash)
		return player->dashingState;

	if (!player->jumpBufferTimer.Finished())
		return player->walljumpingState;

	player->velocity.y -= reducedGravity * dt;
	player->velocity.y = std::clamp(player->velocity.y, -fallSpeed, 0.0f);

	if (player->InputDirX() == -player->faceDir)
		return player->walkingState;

	return nullptr;
}

void DashingState::Enter(Player* player)
{
	player->canDash = false;
	dashTimer.Reset();

	player->velocity.y = 0;

	bool down = Input::IsPressed(CR_KEY_DOWN);
	bool up = Input::IsPressed(CR_KEY_UP);
	dashDir = { 0.0f, 0.0f };

	if (player->InputDirX() != 0 || player->InputDirY() != 0)
		dashDir = glm::normalize(float2(player->InputDirX(), player->InputDirY()));
	else dashDir = { player->faceDir, 0.0f };

	player->currentSheet = player->dashingSheet;
	Audio::Play("assets/audio/Dash.wav");

	Application::Get().freezeTimer = Timer(3.0f);
}

State<Player>* DashingState::Update(Player* player, float dt)
{
	if (Input::IsDown(CR_KEY_SPACE) && player->GetCollision(0, -1))
		return player->superjumpingState;

	if (dashTimer.Finished())
		return player->walkingState;
	else
	{
		dashTimer.Tick(dt);
		player->velocity = dashSpeed * dashDir;

		if (player->GetCollision(player->faceDir, 0))
		{
			int height = 0;
			while (++height < 5)
				if (!player->GetCollision(player->faceDir, height))
				{
					player->MoveY(height);
					break;
				}
		}

		if (player->GetCollision(0, -3))
		{
			int height = 0;
			while (--height > -4)
				if (player->GetCollision(0, height) && height < -1)
				{
					player->MoveY(height+1);
					break;
				}
		}
	}

	return nullptr;
}

void DashingState::Exit(Player* player)
{
	player->velocity = { 0.0f, 0.0f };
}