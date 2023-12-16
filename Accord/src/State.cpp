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

	State<Player>* GetDashingState(Player* player)
	{
		switch (player->currentDashType)
		{
		case Dash:	return player->dashingState;
		case Drift: return player->driftingState;
		}
		return player->dashingState;
	}

	void WalkingState::Enter(Player* player)
	{
		player->coyoteTimer.ForceFinish();
		player->bufferedJumpInput.Cancel();
	}

	State<Player>* WalkingState::Update(Player* player)
	{
		player->currentSheet = GetWalkingStateSheet(player);

		if (Input::IsDown(CR_KEY_SPACE) && !player->grounded && player->WallDir())
		{
			player->faceDir = player->WallDir();

			if (player->InputDirY() != -1)
			{
				int height = 0;
				while (++height < 8)
					if (!player->GetCollision(player->faceDir, height) && player->InputDirX() == player->WallDir())
					{
						return player->ledgeJumpingState;
					}
			}

			return player->walljumpingState;
		}

		if (!player->coyoteTimer.Finished() && player->bufferedJumpInput.Active())
			return player->jumpingState;

		if (player->bufferedDashInput.Active() && player->canDash)
			return GetDashingState(player);

		if (!player->grounded)
		{
			if (player->gravityHaltTimer.Finished())
				player->velocity.y -= gravity * dt;
			else
				player->velocity.y -= gravity * 0.2f * dt;
		}
	
		player->velocity.y = max(player->velocity.y, -maxFallSpeed);

		if (player->InputDirX() != 0 && (abs(player->velocity.x) <= maxWalkSpeed))
		{
			player->velocity.x += player->InputDirX() * acceleration * dt;
			player->velocity.x = clamp(player->velocity.x, -maxWalkSpeed, maxWalkSpeed);
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
		player->jumpSound.Start();

		if (player->carrier)
			player->velocity.x *= 2;

		for (int i = 0; i < 30; i++)
		{
			Game::particles->Add(Particle
			(
				Game::player->position + RIGHTi*5, Game::player->position + RIGHTi * 11,
				{ 4.0f-Game::player->velocity.x / 10.0f, 9.0f }, { -4.0f-Game::player->velocity.x / 10.0f, 15.0f },
				0.4f, 1.0f, 
				WHITE, WHITE)
			);
		}
	}

	State<Player>* JumpingState::Update(Player* player)
	{
		player->coyoteTimer.ForceFinish();

		if (Input::IsUp(CR_KEY_SPACE))
			player->velocity.y = min(player->velocity.y, minJumpSpeed);
		else if (player->velocity.y <= 0)
			return player->walkingState;

		return WalkingState::Update(player);
	}

	void ClingingState::Enter(Player* player)
	{
		impactSpeed = abs(player->velocity.x);
		player->velocity.x = 0;
		player->velocity.y = 0;

		player->currentSheet = player->clingingSheet;
	}

	State<Player>* ClingingState::Update(Player* player)
	{
		if (!player->WallDir())
			return player->walkingState;

		if (player->bufferedDashInput.Active() && player->canDash)
			return GetDashingState(player);

		if (player->bufferedJumpInput.Active())
		{
			if (player->InputDirY() != -1)
			{
				if (impactSpeed > player->walkingState->maxWalkSpeed)
				{
					impactSpeed = 0.0f;
					return player->ledgeJumpingState;
				}

				int height = 0;
				while (++height < 8)
					if (!player->GetCollision(player->faceDir, height) && player->InputDirX() == player->WallDir())
					{
						return player->ledgeJumpingState;
					}
			}
			return player->walljumpingState;
		}

		if (player->InputDirX() == player->faceDir)
			player->velocity.y = 0.0f;
		else
		{
			player->velocity.y -= reducedGravity * dt;
			player->velocity.y = clamp(player->velocity.y, -fallSpeed/2.0f, 0.0f);
		}

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
		player->dashSound.Start();

		Game::Freeze(3);
	}

	State<Player>* DashingState::Update(Player* player)
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
			player->velocity = dashSpeed * dashDir;

			if ((player->currentDashType == Drift) && Input::IsDown(CR_KEY_LEFT_SHIFT))
				return player->walkingState;

			player->CreateDashTrail();

			for (int i = 0; i < 5; i++)
			{
				Game::particles->Add(Particle
				(
					Game::player->position + ONEi*4, Game::player->position + ONEi*12,
					DOWN * 2.0f - float2(0.4f, 5.0f), DOWN * 2.0f + float2(0.4f, 5.0f),
					0.1f, 0.5f,
					RED, CLEAR)
				);
			}

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
		player->currentDashType = Dash;
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

	State<Player>* RollingState::Update(Player* player)
	{
		for (int i = 0; i < (player->grounded ? 20 : 5); i++)
		{
			Game::particles->Add(Particle
			(
				Game::player->position + int2(5 + player->faceDir * 2, 0), Game::player->position + int2(11 + player->faceDir * 2, 0),
				{ -7.0f-Game::player->velocity.x / 10.0f, 15.0f }, { 7.0f-Game::player->velocity.x / 10.0f, 35.0f },
				0.05f, 0.15f,
				WHITE, WHITE)
			);
		}

		player->CreateDashTrail();

		player->renderSize.y = 1.0f - 0.3f*(abs(player->velocity.x)/maxRollSpeed);

		player->velocity.x += player->InputDirX() * (player->grounded ? groundRollAcceleration : airRollAcceleration) * dt;
		player->velocity.x = clamp(player->velocity.x, -maxRollSpeed, maxRollSpeed);
		
		if (!player->grounded)
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

		if (abs(player->velocity.x) < 10.0f)
			return player->walkingState;
		if (player->bufferedJumpInput.Active() && !player->coyoteTimer.Finished())
			return player->rolljumpingState;
		if (player->bufferedDashInput.Active() && player->canDash)
			return GetDashingState(player);
		if (player->WallDir() != 0 && player->InputDirY() != -1)
			return player->clingingState;

		return nullptr;
	}

	void RollingState::Exit(Player* player)
	{
		player->hitbox.min = { 6, 0 };
		player->hitbox.max = { 10,12 };
		player->renderSize.y = 0.5f;
	}
}
