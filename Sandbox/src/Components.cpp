#pragma once

#include "Cockroach.h"
#include "Components.h"

#include "State.h"

using namespace Cockroach;

std::vector<Hitbox*> Hitbox::all = std::vector<Hitbox*>();

Hitbox::Hitbox(Entity* entity)
	: Component(entity)
{
	Hitbox::all.push_back(this);
}

bool Hitbox::OverlapsWith(Hitbox other, int xForesense, int yForesense)
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

Player::Player(Entity* entity)
	: DynamicObject(entity)
{
	walkingState =		new WalkingState;
	jumpingState =		new JumpingState(100.0f, 160.0f, 0.0f);
	superjumpingState = new JumpingState(100.0f, 150.0f, 150.0f);
	walljumpingState =  new JumpingState(100.0f, 130.0f, -140.0);
	climbingState =		new JumpingState(100.0f, 160.0f, 0.0f);
	clingingState =		new ClingingState;
	dashingState =		new DashingState;

	std::string sheetPath = "assets/textures/SpriteSheet.png";
	idleSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 0, 3 }, { 16, 16 }));
	idleSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 1, 3 }, { 16, 16 }));
	walkingSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 2, 3 }, { 16, 16 }));
	walkingSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 3, 3 }, { 16, 16 }));
	dashingSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 4, 3 }, { 16, 16 }));
	dashingSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 5, 3 }, { 16, 16 }));
	dashingSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 6, 3 }, { 16, 16 }));
	jumpingSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 7, 3 }, { 16, 16 }));
	clingingSheet.push_back(	Scene::current->GetSubTexture(sheetPath, { 8, 3 }, { 16, 16 }));
	fallingSheet.push_back(		Scene::current->GetSubTexture(sheetPath, { 9, 3 }, { 16, 16 }));

	animator = entity->GetComponent<Animator>();
	animator->sheet = idleSheet;
	
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
	entity->sprite->flipX = faceDir == -1;

	i8 horizontalCollisions = MoveX(velocity.x * dt);
	i8 verticalCollisions = MoveY((velocityLastFrame.y + velocity.y) * 0.5f * dt); // Verlet integrated vertical motion

	grounded = verticalCollisions == -1;

	if (horizontalCollisions != 0)
		TrySwitchState(clingingState);
}

void Player::OnCollide(Ref<DynamicObject> other, bool horizontalCollision)
{
	if (horizontalCollision) velocity.x = 0;
	else                     velocity.y = 0;
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

StaticObject::StaticObject(Entity* entity)
	: Component(entity)
{
	StaticObject::all.push_back(this);
}

std::vector<DynamicObject*> DynamicObject::all = std::vector<DynamicObject*>();

DynamicObject::DynamicObject(Entity* entity)
	: Component(entity)
{
	DynamicObject::all.push_back(this);
}

int DynamicObject::MoveX(float amount)
{
	xRemainder += amount;
	int move = (int)xRemainder;

	if (move != 0)
	{
		xRemainder -= move;
		int sign = move > 0 ? 1 : -1;

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
				OnCollide(collidingHitbox->entity->GetComponent<DynamicObject>(), true);
				return sign;
			}
		}

		return GetCollidingHitbox(sign, 0) ? sign : 0;
	}
}

int DynamicObject::MoveY(float amount)
{
	yRemainder += amount;
	int move = (int)yRemainder;

	int sign = move > 0 ? 1 : -1;
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
			OnCollide(collidingHitbox->entity->GetComponent<DynamicObject>(), false);
			return sign;
		}
	}

	return GetCollidingHitbox(0, sign) ? sign : 0;

}

Hitbox* DynamicObject::GetCollidingHitbox(int xForesense, int yForesense)
{
	Ref<Hitbox> thisHitbox = entity->GetComponent<Hitbox>();
	for (auto& h : Hitbox::all)
		if (h != thisHitbox.get())
			if (thisHitbox->OverlapsWith(*h, xForesense, yForesense))
				return h;
	return nullptr;
}

void Animator::Update(float dt)
{
	int index = std::fmodf(Application::Get().frameCount * framePerSecond / 60.0f, (float)sheet.size());
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
	zoom = std::clamp(zoom, 5.0f, 100.0f);

	float newAspect = (float)Application::Get().GetWindow().Width() / (float)Application::Get().GetWindow().Height();
	if (aspectRatio != newAspect)
		aspectRatio = newAspect;

	camera.SetZoom(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
}