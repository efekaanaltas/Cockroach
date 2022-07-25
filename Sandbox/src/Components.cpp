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

bool Hitbox::OverlapsWith(Hitbox other)
{
	if (!enabled || !other.enabled) return false;
	bool x = (Left() <= other.Left()) && (other.Right() <= Right());
	bool y = (Bottom() <= other.Bottom()) && (other.Top() <= Top());
	return x && y;
}

bool Hitbox::OverlapsWith(int2 coord)
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

	Ref<Texture2D> texture = CreateRef<Texture2D>("assets/textures/SpriteSheet.png");
	idleSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 0, 3 }, { 16, 16 }));
	idleSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 1, 3 }, { 16, 16 }));
	walkingSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 2, 3 }, { 16, 16 }));
	walkingSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 3, 3 }, { 16, 16 }));
	dashingSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 4, 3 }, { 16, 16 }));
	dashingSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 5, 3 }, { 16, 16 }));
	dashingSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 6, 3 }, { 16, 16 }));
	jumpingSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 7, 3 }, { 16, 16 }));
	clingingSheet.push_back(	SubTexture2D::CreateFromCoords(texture, { 8, 3 }, { 16, 16 }));
	fallingSheet.push_back(		SubTexture2D::CreateFromCoords(texture, { 9, 3 }, { 16, 16 }));

	animator = entity->GetComponent<Animator>();
	animator->sheet = idleSheet;
	
	currentState = walkingState;
	currentState->Enter(this);

	jumpBufferTimer.remainingTime = 0.0f;
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

	Collision result = Move(velocity.x * dt, (velocityLastFrame.y + velocity.y) * 0.5f * dt);

	OnCollide(result);
}

void Player::OnCollide(Collision collision)
{
	if (collision.horizontal) velocity.x = 0;
	if (collision.vertical)   velocity.y = 0;

	grounded = collision.vertical == -1;

	if (collision.horizontal != 0)
		TrySwitchState(clingingState);
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
	return GetCollision(-1, 0).collided || GetCollision(1, 0).collided;
}

void Player::TrySwitchState(State<Player>* newState)
{
	if (newState == nullptr) return;
	currentState->Exit(this);
	currentState = newState;
	currentState->Enter(this);
}

std::vector<DynamicObject*> DynamicObject::all = std::vector<DynamicObject*>();

DynamicObject::DynamicObject(Entity* entity)
	: Component(entity)
{
	DynamicObject::all.push_back(this);
}

Collision DynamicObject::Move(float dx, float dy)
{
	Collision result;

	xRemainder += dx;
	int xMove = (int)xRemainder;

	if (xMove != 0)
	{
		xRemainder -= xMove;
		int xSign = xMove > 0 ? 1 : -1;

		while (xMove != 0)
		{
			bool xCollision = GetCollision(xSign, 0).collided;
			if (!xCollision)
			{
				entity->position.x += xSign;
				xMove -= xSign;
			}
			else
			{
				result.horizontal = xSign;
				break;
			}
		}
	}

	yRemainder += dy;
	int yMove = (int)yRemainder;
	
	if (yMove != 0)
	{
		yRemainder -= yMove;
		int ySign = yMove > 0 ? 1 : -1;

		while (yMove != 0)
		{
			bool yCollision = GetCollision(0, ySign).collided;
			if (!yCollision)
			{
				entity->position.y += ySign;
				yMove -= ySign;
			}
			else
			{
				result.vertical = ySign;
				break;
			}
		}
	}
	
	if(result.horizontal == 0)
		result.horizontal = GetCollision(std::signbit(xRemainder) ? -1 : 1, 0).horizontal;
	if(result.vertical == 0)
		result.vertical = GetCollision(0, std::signbit(yRemainder) ? -1 : 1).vertical;

	return result;
}

Collision DynamicObject::GetCollision(int xForesense, int yForesense)
{
	Collision result;

	Ref<Hitbox> thisHitbox = entity->GetComponent<Hitbox>();
	
	if (Room::current->CollidesWith(thisHitbox->Left() + xForesense, thisHitbox->Right() + xForesense, thisHitbox->Bottom() + yForesense, thisHitbox->Top() + yForesense))
	{
		result.collided = true;
		if (result.horizontal == 0) result.horizontal = xForesense;
		if (result.vertical == 0) result.vertical = yForesense;
		return result;
	}
	return result;
	/*for (auto& h : Hitbox::all)
		if (h != thisHitbox.get())
			if (thisHitbox->OverlapsWith(*h, xForesense, yForesense))
			{
				result.collided = true;
				result.collidedObject = h;
				return result;
			}*/
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