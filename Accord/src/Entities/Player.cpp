#include "Player.h"

#include "Game.h"

Player::Player(int2 position, int2 hitboxMin, int2 hitboxMax)
	: Dynamic(position, hitboxMin, hitboxMax)
{
	walkingState = new WalkingState;
	jumpingState = new JumpingState(50.0f, 130.0f, 0.0f);
	superjumpingState = new JumpingState(50.0f, 110.0f, 250.0f);
	walljumpingState = new JumpingState(50.0f, 80.0f, -140.0);
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

	jumpBufferTimer.remainingTime = 0.0f;
	gravityHaltTimer.remainingTime = 0.0f;
}

void Player::Update(float dt)
{
	velocityLastFrame = velocity;

	if (Input::IsDown(CR_KEY_SPACE)) jumpBufferTimer.Reset();
	else							 jumpBufferTimer.Tick(1.0f);

	if (grounded) coyoteTimer.Reset();
	else		  coyoteTimer.Tick(1.0f);

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

	int horizontalCollision = MoveX(velocity.x * dt);
	int verticalCollision = MoveY((velocityLastFrame.y + velocity.y) * 0.5f * dt);
	grounded = verticalCollision == -1;

	sprite = currentSheet.CurrentSprite();
	sprite.flipX = faceDir == -1;
	sprite.overlayColor = CR_COLOR_RED;
	float flashProgress = flashTimer.Progress01();
	sprite.overlayWeight = std::clamp(-flashProgress * flashProgress + 1, 0.0f, 1.0f);
}

bool Player::OnCollide(Dynamic* other, int horizontal, int vertical)
{
	if (horizontal)
	{
		velocity.x = 0.0f;
		if (!grounded && velocity.y < 0.0f && InputDirX() == horizontal)
			TrySwitchState(clingingState);
	}

	if (vertical)
	{
		velocity.y = 0.0f;
		grounded = vertical == -1;
	}

	if (grounded) canDash = true;

	return !other->As<Trigger>();
}

void Player::OnTrigger(Trigger* trigger)
{
	switch (trigger->type)
	{
	case EntityType::SpikeLeft:
	case EntityType::SpikeRight:
	case EntityType::SpikeDown:
	case EntityType::SpikeUp:
	{
		position = { 0,20 };
		break;
	}
	}
}

i32 Player::InputDirX() const
{
	// Perhaps some kind of overwrite system where if both inputs are pressed, the latest pressed one takes precedence?
	i8 inputDir = 0;
	if (Input::IsPressed(CR_KEY_LEFT))  inputDir = -1;
	if (Input::IsPressed(CR_KEY_RIGHT)) inputDir = +1;
	return inputDir;
}

i32 Player::InputDirY() const
{
	i8 inputDir = 0;
	if (Input::IsPressed(CR_KEY_DOWN))  inputDir = -1;
	if (Input::IsPressed(CR_KEY_UP))	inputDir = +1;
	return inputDir;
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