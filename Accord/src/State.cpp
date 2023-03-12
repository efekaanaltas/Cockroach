#include "crpch.h"
#include "State.h"

#include "EntityDef.h"
#include "Entities/Player.h"
#include "Entities/Particles.h"
#include "Game.h"

using namespace Entities;

namespace Entities
{
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
		player->bufferedJumpInput.Cancel();
	}

	State<Player>* WalkingState::Update(Player* player, float dt)
	{
		player->currentSheet = GetWalkingStateSheet(player);

		if (Input::IsDown(CR_KEY_SPACE) && !player->grounded && player->WallDir())
		{
			player->faceDir = player->WallDir();

			if (player->InputDirY() != -1)
			{
				int height = 0;
				while (++height < 8)
					if (!player->GetCollision(player->faceDir, height))
					{
						return player->ledgeJumpingState;
					}
			}

			return player->walljumpingState;
		}

		if (!player->coyoteTimer.Finished() && player->bufferedJumpInput.Active())
			return player->jumpingState;

		if (player->bufferedDashInput.Active() && player->canDash)
			return player->dashingState;

		if (!player->grounded)
		{
			if (player->gravityHaltTimer.Finished())
				player->velocity.y -= gravity * dt;
			else
				player->velocity.y -= gravity * 0.2f * dt;
		}
	
		player->velocity.y = std::max(player->velocity.y, -maxFallSpeed);

		if (player->InputDirX() != 0 && (std::abs(player->velocity.x) <= maxWalkSpeed))
		{
			player->velocity.x += player->InputDirX() * acceleration * dt;
			player->velocity.x = std::clamp(player->velocity.x, -maxWalkSpeed, maxWalkSpeed);
		}
		else
		{
			player->velocity.x -= player->faceDir * (player->grounded ? deceleration : airDeceleration) * dt;
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
		player->renderSize.x = 0.6f;
		Audio::Play("assets/audio/Jump.wav");

		for (int i = 0; i < 30; i++)
		{
			Game::particles->particles.push_back(Particle
			(
				Game::player->position + RIGHTi*8, { 3,0 },
				{ -Game::player->velocity.x / 10.0f, 12.0f }, { 4.0f, 3.0f },
				0.7f, 0.3f, 
				WHITE, CLEAR)
			);
		}
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

		if (player->bufferedDashInput.Active() && player->canDash)
			return player->dashingState;

		if (player->bufferedJumpInput.Active())
		{
			if (player->InputDirY() != -1)
			{
				int height = 0;
				while (++height < 8)
					if (!player->GetCollision(player->faceDir, height))
					{
						return player->ledgeJumpingState;
					}
			}
			return player->walljumpingState;
		}

		player->velocity.y -= reducedGravity * dt;
		player->velocity.y = std::clamp(player->velocity.y, -fallSpeed/2.0f, 0.0f);

		if (player->InputDirX() == -player->faceDir || player->InputDirY() == -1 || player->grounded)
			return player->walkingState;

		return nullptr;
	}

	void DashingState::Enter(Player* player)
	{
		player->canDash = false;
		player->grounded = false;
		player->bufferedDashInput.Cancel();
		player->dashRegainTimer.Reset();
		dashTimer.Reset();

		player->velocity.y = 0;
		player->velocityLastFrame.y = 0;

		dashDir = ZERO;
		if (player->InputDirX() != 0 || player->InputDirY() != 0)
			dashDir = glm::normalize(float2(player->InputDirX(), player->InputDirY()));
		else dashDir = { player->faceDir, 0.0f };

		player->currentSheet = player->dashingSheet;
		Audio::Play("assets/audio/Dash.wav");

		Game::Freeze(3);
	}

	State<Player>* DashingState::Update(Player* player, float dt)
	{
		if (player->bufferedJumpInput.Active() && player->GetCollision(0, -1))
		{
			if (player->faceDir != player->InputDirX() && player->InputDirX() != 0)
			{
				player->velocity.x *= -1;
				player->faceDir *= -1;
			}
			return player->superjumpingState;
		}

		if(dashTimer.Finished() && dashDir.x != 0 && dashDir.y < 0)
			return player->rollingState;

		if (dashTimer.Finished())
			return player->walkingState;
		else
		{

			for (int i = 0; i < 5; i++)
			{
				Game::particles->particles.push_back(Particle
				(
					Game::player->position + ONEi*8, ONE*4.0f,
					DOWN * 2.0f, { 0.4f, 5.0f },
					0.3f, 0.2f,
					RED, BLUE*0.3f)
				);
			}

			dashTimer.Tick(dt);
			player->velocity = dashSpeed * dashDir;

			if (dashDir.y != 0) return nullptr;

			if (player->GetCollision(player->faceDir, 0))
			{
				int height = 0;
				while (++height < 5)
					if (!player->GetCollision(player->faceDir, height))
					{
						player->MoveY((float)height);
						break;
					}
			}

			if (player->GetCollision(0, -3))
			{
				int height = 0;
				while (--height > -4)
					if (player->GetCollision(0, height) && height < -1)
					{
						player->MoveY((float)height+1);
						break;
					}
			}
		}

		return nullptr;
	}

	void DashingState::Exit(Player* player)
	{
		if (dashDir.y > 0.0f)
		{
			player->gravityHaltTimer.Reset();
			player->velocity.y = 0.0f;
		}
	}

	void RollingState::Enter(Player* player)
	{
		player->currentSheet = player->rollingSheet;
		player->hitbox.min = { 6,  0 };
		player->hitbox.max = { 10, 8 };
		player->velocity.x = 100.0f * (player->InputDirX() != 0 ? player->InputDirX() : player->faceDir);
	}

	State<Player>* RollingState::Update(Player* player, float dt)
	{
		player->velocity.x += player->InputDirX() * (player->grounded ? groundRollAcceleration : airRollAcceleration) * dt;
		player->velocity.x = std::clamp(player->velocity.x, -maxRollSpeed, maxRollSpeed);
		
		if(!player->grounded)
			player->velocity.y -= player->walkingState->gravity * dt;

		if (player->GetCollision(player->faceDir, 0))
		{
			int height = 0;
			while (++height <= 8)
				if (!player->GetCollision(player->faceDir*2, height))
				{
					player->MoveY(2);
					break;
				}
		}

		if (std::abs(player->velocity.x) < 10.0f)
			return player->walkingState;
		if (player->bufferedJumpInput.Active() && !player->coyoteTimer.Finished())
			return player->rolljumpingState;
		if (player->bufferedDashInput.Active() && player->canDash)
			return player->dashingState;
		if (player->WallDir() != 0 && player->InputDirY() != -1)
			return player->clingingState;

		return nullptr;
	}

	void RollingState::Exit(Player* player)
	{
		player->hitbox.min = { 6, 0 };
		player->hitbox.max = { 10,12 };
	}
}
