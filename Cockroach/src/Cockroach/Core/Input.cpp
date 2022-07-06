#include "crpch.h"
#include "Input.h"

#include "Cockroach/Core/Application.h"
#include "GLFW/glfw3.h"

namespace Cockroach
{
	std::unordered_map<u16, Input::InputState> Input::stateMap =
	{
		{ CR_MOUSE_BUTTON_LEFT,		InputState() },
		{ CR_MOUSE_BUTTON_RIGHT,	InputState() },
		{ CR_MOUSE_BUTTON_MIDDLE,	InputState() },
		{ CR_KEY_LEFT_SHIFT,		InputState() },
		{ CR_KEY_LEFT_CONTROL,		InputState() },
		{ CR_KEY_ESCAPE,			InputState() },
		{ CR_KEY_W,					InputState() },
		{ CR_KEY_A,					InputState() },
		{ CR_KEY_S,					InputState() },
		{ CR_KEY_D,					InputState() },
		{ CR_KEY_UP,				InputState() },
		{ CR_KEY_LEFT,				InputState() },
		{ CR_KEY_DOWN,				InputState() },
		{ CR_KEY_RIGHT,				InputState() },
		{ CR_KEY_SPACE,				InputState() },
	};

	float Input::scroll = 0.0f;

	void Input::Update()
	{
		for (auto& [key, value] : Input::stateMap)
			value.pressedLastFrame = value.pressed;
	}

	bool Input::IsPressed(u16 code) { return stateMap[code].pressed; }
	bool Input::IsDown(u16 code)	{ return stateMap[code].pressed && !stateMap[code].pressedLastFrame; }
	bool Input::IsUp(u16 code)		{ return !stateMap[code].pressed && stateMap[code].pressedLastFrame; }

	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GLFWWindow);
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX() { return GetMousePosition().first; }
	float Input::GetMouseY() { return GetMousePosition().second; }
}