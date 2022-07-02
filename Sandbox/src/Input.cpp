#include "crpch.h"
#include "Input.h"

::Input* ::Input::input = new ::Input();

void ::Input::Update()
{
	for (uint32_t i = 0; i < TRACKED_KEY_COUNT; i++)
		trackedKeys[i].pressedLastFrame = trackedKeys[i].pressed;
}

bool ::Input::OnKeyPressed(Cockroach::KeyPressedEvent& e)
{
	if (e.GetRepeatCount() != 0) return false;
	for (uint32_t i = 0; i < TRACKED_KEY_COUNT; i++)
		if (trackedKeys[i].keycode == e.GetKeyCode())
			trackedKeys[i].pressed = true;
	return false;
}

bool ::Input::OnKeyReleased(Cockroach::KeyReleasedEvent& e)
{
	for (uint32_t i = 0; i < TRACKED_KEY_COUNT; i++)
		if (trackedKeys[i].keycode == e.GetKeyCode())
			trackedKeys[i].pressed = false;
	return false;
}

void ::Input::OnEvent(Cockroach::Event& e)
{
	Cockroach::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cockroach::KeyPressedEvent>(CR_BIND_EVENT_FN(Input::OnKeyPressed));
	dispatcher.Dispatch<Cockroach::KeyReleasedEvent>(CR_BIND_EVENT_FN(Input::OnKeyReleased));
}

bool ::Input::GetKey(int keycode)
{
	for (uint32_t i = 0; i < TRACKED_KEY_COUNT; i++)
		if (trackedKeys[i].keycode == keycode)
			return trackedKeys[i].pressed;
	CR_CORE_ASSERT(false, "Keycode is not recognized.");
	return false;
}

bool ::Input::GetKeyDown(int keycode)
{
	for (uint32_t i = 0; i < TRACKED_KEY_COUNT; i++)
		if (trackedKeys[i].keycode == keycode)
			return trackedKeys[i].pressed && !trackedKeys[i].pressedLastFrame;
	CR_CORE_ASSERT(false, "Keycode is not recognized.");
	return false;
}

bool ::Input::GetKeyUp(int keycode)
{
	for (uint32_t i = 0; i < TRACKED_KEY_COUNT; i++)
		if (trackedKeys[i].keycode == keycode)
			return !trackedKeys[i].pressed && trackedKeys[i].pressedLastFrame;
	CR_CORE_ASSERT(false, "Keycode is not recognized.");
	return false;
}
