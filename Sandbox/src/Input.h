#pragma once

#include "Cockroach.h"

using namespace Cockroach;

class Input
{
public:

	static Input* input;

	struct TrackedKey
	{
		int keycode = -1;
		bool pressed = false;
		bool pressedLastFrame = false;
	};

	const static uint32_t TRACKED_KEY_COUNT = 6;
	TrackedKey trackedKeys[TRACKED_KEY_COUNT] = {
		{ CR_KEY_SPACE, false, false },
		{ CR_KEY_LEFT_SHIFT, false, false },
		{ CR_KEY_W, false, false },
		{ CR_KEY_D, false, false },
		{ CR_KEY_S, false, false },
		{ CR_KEY_A, false, false },
	};

	void Update();
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnKeyReleased(KeyReleasedEvent& e);
	void OnEvent(Event& e);

	bool GetKey(int keycode);
	bool GetKeyDown(int keycode);
	bool GetKeyUp(int keycode);
};