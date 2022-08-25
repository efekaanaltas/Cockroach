#pragma once

#include "Cockroach.h"
#include "Components.h"

#include "State.h"

#include "Game.h"

using namespace Cockroach;

Player::Player(Entity* entity)
	: DynamicObject(entity)
{
	walkingState =		new WalkingState;
	jumpingState =		new JumpingState(100.0f, 160.0f, 0.0f);
	superjumpingState = new JumpingState(100.0f, 150.0f, 250.0f);
	walljumpingState =  new JumpingState(100.0f, 130.0f, -140.0);
	clingingState =		new ClingingState;
	dashingState =		new DashingState;

	Ref<Texture2D> texture = Game::baseSpriteSheet;

	idleSheet.framePerSecond = 3;
	idleSheet.Add(		Sprite::CreateFromCoords(texture, { 0, 3 }, { 16, 16 }));
	idleSheet.Add(		Sprite::CreateFromCoords(texture, { 1, 3 }, { 16, 16 }));

	walkingSheet.framePerSecond = 10;
	walkingSheet.Add(	Sprite::CreateFromCoords(texture, { 2, 3 }, { 16, 16 }));
	walkingSheet.Add(	Sprite::CreateFromCoords(texture, { 3, 3 }, { 16, 16 }));
	walkingSheet.Add(	Sprite::CreateFromCoords(texture, { 4, 3 }, { 16, 16 }));

	dashingSheet.Add(	Sprite::CreateFromCoords(texture, { 5, 3 }, { 16, 16 }));
	jumpingSheet.Add(	Sprite::CreateFromCoords(texture, { 6, 3 }, { 16, 16 }));
	clingingSheet.Add(	Sprite::CreateFromCoords(texture, { 7, 3 }, { 16, 16 }));
	fallingSheet.Add(	Sprite::CreateFromCoords(texture, { 8, 3 }, { 16, 16 }));

	animator = entity->GetComponent<Animator>();
	animator->sheet = idleSheet;
	
	currentState = walkingState;
	currentState->Enter(this);

	jumpBufferTimer.remainingTime = 0.0f;
}

void Player::Update(float dt)
{
	velocityLastFrame = velocity;

	if (Input::IsDown(CR_KEY_SPACE)) jumpBufferTimer.Reset();
	else							 jumpBufferTimer.Tick(1.0f);

	if (grounded) coyoteTimer.Reset();
	else		  coyoteTimer.Tick(1.0f);

	TrySwitchState(currentState->Update(this, dt));

	if (velocity.x != 0.0f) // Use InputDirX() instead?
		faceDir = velocity.x < 0.0f ? -1 : 1;
	entity->sprite.flipX = faceDir == -1;

	int horizontalCollision = MoveX(velocity.x * dt);
	int verticalCollision = MoveY((velocityLastFrame.y + velocity.y) * 0.5f * dt);

	grounded = verticalCollision == -1;
}

bool Player::OnCollide(Ref<DynamicObject> other, int horizontal, int vertical)
{
	if (std::dynamic_pointer_cast<Hazard>(other))
		entity->position = { 0,0 };

	if (horizontal)
	{
		velocity.x = 0.0f;
		if(!grounded && velocity.y < 0.0f && InputDirX() == horizontal)
			TrySwitchState(clingingState);
	}

	if (vertical)
	{
		velocity.y = 0.0f;
	}

	grounded = vertical == -1;
	if(grounded) canDash = true;

	return true;
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

DynamicObject::DynamicObject(Entity* entity)
	: Component(entity)
{
}

int DynamicObject::MoveX(float amount)
{
	xRemainder += amount;
	int move = (int)xRemainder;
	int sign = move > 0 ? 1 : -1;

	if (move != 0)
	{
		xRemainder -= move;

		while (move != 0)
		{
			Ref<DynamicObject> collidingHitbox = GetEntityCollision(sign, 0);
			bool tilemapCollision = GetTilemapCollision(sign, 0);
			if (!collidingHitbox && !tilemapCollision)
			{
				entity->position.x += sign;
				move -= sign;
			}
			else if (OnCollide(collidingHitbox, sign, 0))
					return sign;
		}
	}
	return GetCollision(sign, 0) ? sign : 0;
}

int DynamicObject::MoveY(float amount)
{
	yRemainder += amount;
	int move = (int)yRemainder;
	int sign = move > 0 ? 1 : -1;
	
	if (move != 0)
	{
		yRemainder -= move;

		while (move != 0)
		{
			Ref<DynamicObject> collidingHitbox = GetEntityCollision(0, sign, Heavy);
			bool tilemapCollision = GetTilemapCollision(0, sign);
			if (!collidingHitbox && !tilemapCollision)
			{
				entity->position.y += sign;
				move -= sign;
			}
			else if (OnCollide(collidingHitbox, 0, sign))
				return sign;
		}
	}
	return GetCollision(0, sign) ? sign : 0;
}

Ref<DynamicObject> DynamicObject::GetEntityCollision(int xForesense, int yForesense, CollisionLayer layer)
{	
	for (int i = 0; i < Room::current->entities.size(); i++)
	{
		if (Room::current->entities[i] != *entity)
		{
			Ref<DynamicObject> dyn = Room::current->entities[i].GetComponent<DynamicObject>();
			if (OverlapsWith(dyn, xForesense, yForesense) && (layer & All || layer == dyn->layer))
				return dyn;
		}
	}
	// Also check for collisions with player
	if (Game::player->entity != entity && OverlapsWith(Game::player, xForesense, yForesense) && (layer == All || layer == Game::player->layer))
		return Game::player;
	return nullptr;
}

bool DynamicObject::GetTilemapCollision(int xForesense, int yForesense)
{
	return Room::current->CollidesWith(WorldHitbox(), xForesense, yForesense);
}

bool DynamicObject::GetCollision(int xForesense, int yForesense, CollisionLayer layer)
{
	return GetTilemapCollision(xForesense, yForesense) || GetEntityCollision(xForesense, yForesense, layer);
}

void Animator::Update(float dt)
{
	int index = (int)std::fmodf(Application::Get().frameCount * sheet.framePerSecond / 60.0f, (float)sheet.Size());
	entity->sprite = sheet[index];
}

CameraController::CameraController(Entity* entity)
	: Component(entity), camera(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom)
{
}

void CameraController::Update(float dt)
{
	if (Input::IsPressed(CR_KEY_A))
		positionHighRes.x -= speed * dt;
	if (Input::IsPressed(CR_KEY_D))
		positionHighRes.x += speed * dt;
	if (Input::IsPressed(CR_KEY_S))
		positionHighRes.y -= speed * dt;
	if (Input::IsPressed(CR_KEY_W))
		positionHighRes.y += speed * dt;

	entity->position = int2(positionHighRes.x, positionHighRes.y);

	camera.SetPosition({positionHighRes.x, positionHighRes.y, 0.0f});

	speed = zoom; // Change speed according to zoom level

	zoom -= Input::scroll * 0.1f * zoom;
	zoom = std::clamp(zoom, 5.0f, 1000.0f);

	float newAspect = (float)Application::Get().GetWindow().Width() / (float)Application::Get().GetWindow().Height();
	if (aspectRatio != newAspect)
		aspectRatio = newAspect;

	camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
}

void CameraController::SetZoom(float zoom)
{
	this->zoom = zoom;
	camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
}