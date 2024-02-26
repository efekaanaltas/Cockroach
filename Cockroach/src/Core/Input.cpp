#include "crpch.h"
#include "Input.h"

#include "Core/Application.h"
#include "GLFW/glfw3.h"

namespace Cockroach
{
	std::unordered_map<u16, Input::InputState> Input::stateMap =
	{
		{ CR_MOUSE_BUTTON_LEFT,		{} },
		{ CR_MOUSE_BUTTON_RIGHT,	{} },
		{ CR_MOUSE_BUTTON_MIDDLE,	{} },
		{ CR_KEY_LEFT_SHIFT,		{} },
		{ CR_KEY_LEFT_CONTROL,		{} },
		{ CR_KEY_ESCAPE,			{} },
		{ CR_KEY_UP,				{} },
		{ CR_KEY_LEFT,				{} },
		{ CR_KEY_DOWN,				{} },
		{ CR_KEY_RIGHT,				{} },
		{ CR_KEY_SPACE,				{} },
		{ CR_KEY_Q,					{} },
		{ CR_KEY_W,					{} },
		{ CR_KEY_E,					{} },
		{ CR_KEY_R,					{} },
		{ CR_KEY_T,					{} },
		{ CR_KEY_Y,					{} },
		{ CR_KEY_U,					{} },
		{ CR_KEY_I,					{} },
		{ CR_KEY_O,					{} },
		{ CR_KEY_P,					{} },
		{ CR_KEY_A,					{} },
		{ CR_KEY_S,					{} },
		{ CR_KEY_D,					{} },
		{ CR_KEY_F,					{} },
		{ CR_KEY_G,					{} },
		{ CR_KEY_H,					{} },
		{ CR_KEY_J,					{} },
		{ CR_KEY_K,					{} },
		{ CR_KEY_L,					{} },
		{ CR_KEY_Z,					{} },
		{ CR_KEY_X,					{} },
		{ CR_KEY_C,					{} },
		{ CR_KEY_V,					{} },
		{ CR_KEY_B,					{} },
		{ CR_KEY_N,					{} },
		{ CR_KEY_M,					{} },
	};

	u16 Input::actionMap[] = 
	{
		CR_KEY_A,
		CR_KEY_D,
		CR_KEY_W,
		CR_KEY_S,
		CR_KEY_SPACE,
		CR_KEY_LEFT_SHIFT
	};

	float Input::scroll = 0.0f;

	void Input::Update()
	{
		Input::scroll = 0.0f;
		for (auto& [key, value] : Input::stateMap)
			value.pressedLastFrame = value.pressed;
	}

	bool Input::IsPressed(u16 code) { return stateMap[code].pressed; }
	bool Input::IsDown(u16 code)	{ return stateMap[code].pressed && !stateMap[code].pressedLastFrame; }
	bool Input::IsUp(u16 code)		{ return !stateMap[code].pressed && stateMap[code].pressedLastFrame; }

	bool Input::IsPressed(InputAction action)	{ return IsPressed(actionMap[action]); }
	bool Input::IsDown(InputAction action)		{ return IsDown(actionMap[action]); }
	bool Input::IsUp(InputAction action)		{ return IsUp(actionMap[action]); }

	float2 Input::MousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GLFWWindow);
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}
}