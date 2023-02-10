#include "Player.h"

#include "Game.h"

namespace Entities
{
	Player::Player(int2 position, int2 hitboxMin, int2 hitboxMax)
		: Dynamic(position, hitboxMin, hitboxMax)
	{
		walkingState = new WalkingState;
		jumpingState = new JumpingState(50.0f, 140.0f, 0.0f);
		superjumpingState = new JumpingState(50.0f, 120.0f, 180.0f);
		walljumpingState = new JumpingState(50.0f, 80.0f, -160.0);
		ledgeJumpingState = new JumpingState(80.0f, 80.0f, 0.0f);
		clingingState = new ClingingState;
		dashingState = new DashingState;

		Ref<Texture2D> texture = Game::baseSpriteSheet;

		idleSheet.framePerSecond = 3;
		idleSheet.Add(Sprite::CreateFromCoords(texture, { 0, 3 }, { 16, 16 }));
		idleSheet.Add(Sprite::CreateFromCoords(texture, { 1, 3 }, { 16, 16 }));

		walkingSheet.framePerSecond = 10;
		walkingSheet.Add(Sprite::CreateFromCoords(texture, { 2, 3 }, { 16, 16 }));
		walkingSheet.Add(Sprite::CreateFromCoords(texture, { 3, 3 }, { 16, 16 }));
		walkingSheet.Add(Sprite::CreateFromCoords(texture, { 4, 3 }, { 16, 16 }));

		dashingSheet.Add(Sprite::CreateFromCoords(texture, { 5, 3 }, { 16, 16 }));
		jumpingSheet.Add(Sprite::CreateFromCoords(texture, { 6, 3 }, { 16, 16 }));
		clingingSheet.Add(Sprite::CreateFromCoords(texture, { 7, 3 }, { 16, 16 }));
		fallingSheet.Add(Sprite::CreateFromCoords(texture, { 8, 3 }, { 16, 16 }));

		currentSheet = idleSheet;

		currentState = walkingState;
		currentState->Enter(this);

		bufferedJumpInput.Cancel();
		bufferedDashInput.Cancel();
		gravityHaltTimer.remainingTime = 0.0f;
	}

	void Player::Render()
	{
		float2 adjustedPosition = (float2)position + (float2(1, 1) - renderSize) * float2(sprite.XSize() / 2, 0);
		float2 adjustedSize = { sprite.XSize() * renderSize.x, sprite.YSize() * renderSize.y };
		Renderer::DrawQuadWithOutline(float3(adjustedPosition.x, adjustedPosition.y, 10), adjustedSize, sprite, { overlayColor ,overlayWeight }, BLACK, flipX, flipY);
	}

	void Player::Update(float dt)
	{
		bool groundedAtStartOfFrame = grounded;
		velocityLastFrame = velocity;

		bufferedJumpInput.Update();
		bufferedDashInput.Update();

		if (grounded) coyoteTimer.Reset();
		else		  coyoteTimer.Tick(1.0f);

		dashRegainTimer.Tick(1.0f);
		gravityHaltTimer.Tick(1.0f);
		flashTimer.Tick(1.0f);

		TrySwitchState(currentState->Update(this, dt));

		if(!Room::current->Contains(WorldHitbox()))
			for(auto& room : Game::rooms)
				if (room != Room::current && room->OverlapsWith(WorldHitbox(), 0, 0))
				{
					Room::current = room;
					Rect bounds = room->Bounds();
					position = glm::clamp(position, Room::current->Bounds().min - hitbox.min, Room::current->Bounds().max - hitbox.max);
					break;
				}

		if (velocity.x != 0.0f)
			faceDir = velocity.x < 0.0f ? -1 : 1;

		MoveX(velocity.x * dt);
		MoveY((velocityLastFrame.y + velocity.y) * 0.5f * dt);

		grounded = GetCollision(0, -1); //verticalCollision == -1;
		if (grounded && dashRegainTimer.Finished()) RegainDash();

		if (grounded && !groundedAtStartOfFrame)
			renderSize.y = 0.6f;

		renderSize.x = std::clamp(renderSize.x + 2 * dt, 0.0f, 1.0f);
		renderSize.y = std::clamp(renderSize.y + 2 * dt, 0.0f, 1.0f);

		sprite = currentSheet.CurrentSprite();
		flipX = faceDir == -1;
		float flashProgress = flashTimer.Progress01();
		overlayColor = WHITE;
		overlayWeight = std::clamp(-flashProgress * flashProgress + 1, 0.0f, 1.0f);
	}

	bool Player::OnCollide(Dynamic* other, int horizontal, int vertical)
	{
		bool blockCollisions = other ? other->blockOnCollision : true;
		if (blockCollisions)
		{
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
				velocity.x = 0.0f;
				if (!grounded && velocity.y < 0.0f && InputDirX() == horizontal)
					TrySwitchState(clingingState);
			}

			if (vertical)
				velocity.y = 0.0f;
		}

		return blockCollisions;
	}

	i32 Player::InputDirX() const
	{
		static int lastDir = 0;

		bool left = Input::IsPressed(CR_KEY_A);
		bool right = Input::IsPressed(CR_KEY_D);

		if (left && right)
			return -lastDir;
		else return lastDir = (left ? -1 : (right ? 1 : 0));
	}

	i32 Player::InputDirY() const
	{
		bool down = Input::IsPressed(CR_KEY_S);
		bool up = Input::IsPressed(CR_KEY_W);
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

	void Player::RegainDash()
	{
		if(!canDash) flashTimer.Reset();
		canDash = true;
	}

	void Player::Die()
	{
		position = { -72,32 };
	}
}
