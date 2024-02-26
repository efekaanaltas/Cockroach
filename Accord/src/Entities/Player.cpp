#include "Player.h"
#include "EntityDef.h"
#include "Game.h"

namespace Entities
{
	Player::Player(int2 position)
		: Dynamic(EntityDefinition(EntityType::Payga, false, position, 8*ONEi), {6,0}, {10,12}, true, true)
	{
		walkingState = new WalkingState;
		jumpingState = new JumpingState(50.0f, 140.0f, 0.0f);
		superjumpingState = new JumpingState(90.0f, 135.0f, 200.0f);
		rolljumpingState = new JumpingState(80.0f, 150.0f, 200.0f);
		walljumpingState = new JumpingState(90.0f, 100.0f, -180.0);
		ledgeJumpingState = new JumpingState(100.0f, 100.0f, 0.0f);
		clingingState = new ClingingState;
		dashingState = new DashingState(150.0f, 0.2f);
		driftingState = new DashingState(150.0f, 100.0f);
		rollingState = new RollingState;

		Ref<Texture2D> texture = Game::baseSpriteSheet;

		idleSheet.framePerSecond = 3;
		idleSheet.Add(Sprite::CreateFromCoords(texture, { 0, 3 }, { 16, 16 }));
		idleSheet.Add(Sprite::CreateFromCoords(texture, { 1, 3 }, { 16, 16 }));

		walkingSheet.framePerSecond = 6;
		walkingSheet.Add(Sprite::CreateFromCoords(texture, { 2, 3 }, { 16, 16 }));
		walkingSheet.Add(Sprite::CreateFromCoords(texture, { 3, 3 }, { 16, 16 }));
		walkingSheet.Add(Sprite::CreateFromCoords(texture, { 4, 3 }, { 16, 16 }));
		walkingSheet.Add(Sprite::CreateFromCoords(texture, { 3, 3 }, { 16, 16 }));

		dashingSheet.Add(Sprite::CreateFromCoords(texture, { 5, 3 }, { 16, 16 }));
		jumpingSheet.Add(Sprite::CreateFromCoords(texture, { 6, 3 }, { 16, 16 }));

		clingingSheet.framePerSecond = 2;
		clingingSheet.Add(Sprite::CreateFromCoords(texture, { 7, 3 }, { 16, 16 }));
		clingingSheet.Add(Sprite::CreateFromCoords(texture, { 8, 3 }, { 16, 16 }));

		fallingSheet.Add(Sprite::CreateFromCoords(texture, { 9, 3 }, { 16, 16 }));

		rollingSheet.framePerSecond = 20;
		rollingSheet.Add(Sprite::CreateFromCoords(texture, { 10, 3 }, { 16,16 }));
		rollingSheet.Add(Sprite::CreateFromCoords(texture, { 11, 3 }, { 16,16 }));
		rollingSheet.Add(Sprite::CreateFromCoords(texture, { 12, 3 }, { 16,16 }));
		rollingSheet.Add(Sprite::CreateFromCoords(texture, { 13, 3 }, { 16,16 }));

		currentSheet = idleSheet;

		currentState = walkingState;
		currentState->Enter(this);

		bufferedJumpInput.Cancel();
		bufferedDashInput.Cancel();

		checkpointPosition = position;

		for (int i = 0; i < DashTrail::count; i++)
		{
			dashTrail[i].sprite = Sprite::CreateFromCoords(texture, { 0,3 }, { 16,16 });
		}
	}

	void Player::Render()
	{
		if (!Game::editMode)
		{
			Renderer::EndScene();
			Renderer::BeginScene(Game::cameraController->camera);
	;		Game::distortionFramebuffer->Bind();
			Renderer::Clear();
			for (int i = 0; i < DashTrail::count; i++)
			{
				
				Renderer::DrawQuad(float3((float2)dashTrail[i].position, 9), 16.0f * ONE, dashTrail[i].sprite, float4(1.0f, 1.0f, 1.0f, pow(dashTrail[i].strength, 3.0f)), float4((ONE-glm::normalize(velocity))/2.0f, 0.0f, 1.0f), dashTrail[i].flipX, false);
			}
			Renderer::EndScene();
			Game::framebuffer->Bind();
			Renderer::BeginScene(Game::cameraController->camera);
		}

		float2 adjustedPosition = (float2)position + (float2(1, 1) - renderSize) * float2(sprite.XSize() / 2, 0);
		float2 adjustedSize = { sprite.XSize() * renderSize.x, sprite.YSize() * renderSize.y };
		Renderer::DrawQuadWithOutline(float3(adjustedPosition.x, adjustedPosition.y, 10), adjustedSize, sprite, color, { overlayColor , overlayWeight }, BLACK, HasFlag(FlipX), HasFlag(FlipY));
	}

	void Player::Update()
	{
		bool groundedAtStartOfFrame = grounded;
		velocityLastFrame = velocity;

		bufferedJumpInput.Update();
		bufferedDashInput.Update();

		if (grounded) coyoteTimer.Reset();

		TrySwitchState(currentState->Update(this));

		if (lookingForCheckpoint && grounded)
		{
			checkpointPosition = position;
			lookingForCheckpoint = false;
		}

		if (!Room::current->Contains(WorldHitbox()))
			TryChangeRoom();

		if (velocity.x != 0.0f)
			faceDir = velocity.x < 0.0f ? -1 : 1;

		MoveX(velocity.x * dt);
		MoveY((velocityLastFrame.y + velocity.y) * 0.5f * dt);

		grounded = GetCollision(0, -1); //verticalCollision == -1;
		if (grounded && dashRegainTimer.Finished(false) && !canDash) RegainDash();

		if (grounded && !groundedAtStartOfFrame && velocityLastFrame.y < -30.0f) // Use velocityLastFrame because y velocity is set to 0 on ground
			renderSize.y = 0.6f;

		dashSound.SetPosition(position);
		jumpSound.SetPosition(position);

		renderSize.x = clamp(renderSize.x + 2 * dt, 0.0f, 1.0f);
		renderSize.y = clamp(renderSize.y + 2 * dt, 0.0f, 1.0f);

		sprite = currentSheet.CurrentSprite();
		SetFlag(FlipX, faceDir == -1);
		float flashProgress = flashTimer.Progress01();
		overlayColor = WHITE;
		overlayWeight = clamp(-flashProgress * flashProgress + 1, 0.0f, 1.0f);

		for (int i = 0; i < DashTrail::count; i++)
		{
			dashTrail[i].strength = max(dashTrail[i].strength - 1/20.0f, 0.0f);
		}
	}

#pragma warning (disable: 4244) // Lots of int->float conversions, no need for warnings.

	// This function as a whole is just weird isn't it?
	bool Player::OnCollide(Dynamic* other, int horizontal, int vertical)
	{
		bool blockCollisions = other ? other->HasFlag(IsSolid) : true;
		if (blockCollisions)
		{
			// This code seems to be written very early into the entity system.
			// Propeller should have it's own update function instead of this weirdness.
			if (currentState == dashingState && other && other->As<Propeller>())
			{
				if (vertical == -1 && velocity.y == 0) return blockCollisions;
				TrySwitchState(walkingState);
				if(velocity.x != 0)
					faceDir *= -1;
				RegainDash();
				velocity = -float2(horizontal * 300, vertical * 170);
				Game::Freeze(3);
				return blockCollisions;
			}

			if (horizontal)
			{
				int height = 0;
				while (++height < 4)
					if (!GetCollision(faceDir, height))
					{
						MoveY((float)height);
						//MoveX(faceDir);
						break;
					}
				// velocity.x = 0.0f; Enabling this makes walljumps feel stronger but doesn't allow for rolling up 1-high tiles. 
				// I could probably find a way to make both work but I'm not sure how I want walljumping to work yet.
				if (!grounded && velocity.y < 0.0f && InputDirX() == horizontal && InputDirY() != -1)
					TrySwitchState(clingingState);
			}

			if (vertical)
			{
				if (vertical == 1)
				{
					int i = 0;
					while (++i <= 4)
						if (!GetCollision(faceDir * i, 1))
						{
							MoveX(faceDir*i);
							break;
						}
				}
				else
					velocity.y = 0.0f;
			}
		}

		return blockCollisions;
	}

	i32 Player::InputDirX() const
	{
		static int lastDir = 0;

		bool left = Input::IsPressed(InputAction::Left);
		bool right = Input::IsPressed(InputAction::Right);

		if (left && right)
			return -lastDir;
		else return lastDir = (left ? -1 : (right ? 1 : 0));
	}

	i32 Player::InputDirY() const
	{
		bool down = Input::IsPressed(InputAction::Down);
		bool up = Input::IsPressed(InputAction::Up);
		return down ? -1 : (up ? 1 : 0);
	}

	int Player::WallDir()
	{
		if (GetCollision(faceDir, 0))	return faceDir;
		if (GetCollision(-faceDir, 0))	return -faceDir;
		return 0;
	}

	void Player::TrySwitchState(State<Player>* newState)
	{
		if (newState == nullptr) return;
		currentState->Exit(this);
		currentState = newState;
		currentState->Enter(this);
	}

	void Player::TryChangeRoom()
	{
		for (auto& room : Game::rooms)
			if (room != Room::current && room->OverlapsWith(WorldHitbox(), 0, 0))
			{
				Room::current = room;
				Room::current->Reset();
				Rect bounds = room->Bounds();
				position = glm::clamp(position, bounds.min - hitbox.min, bounds.max - hitbox.max);
				Game::cameraController->StartTransition();
				lookingForCheckpoint = true;
				RegainDash(currentDashType);
				return;
			}
		if (!Room::current->Contains(WorldHitbox()))
			Die();
	}

	void Player::RegainDash(DashType dashType)
	{
		if(!canDash) flashTimer.Reset();
		currentDashType = dashType;
		canDash = true;
	}

	void Player::Die()
	{
		position = checkpointPosition;
	}

	void Player::CreateDashTrail()
	{
		dashTrail[(++lastDashTrailIndex%DashTrail::count)] = DashTrail(sprite, position, faceDir == -1);
	}
}
