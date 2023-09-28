#pragma once

#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/Utility.h"
#include "Core/Input.h"

// ---Renderer-----------------------------
#include "Renderer/Renderer.h"

#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexArray.h"

#include "Renderer/Camera.h"
// ----------------------------------------

// ---Audio--------------------------------
#include "Audio/Audio.h"
#include "Audio/Sound.h"
// ----------------------------------------

// ---Game---------------------------------
#include "Game/Room.h"
#include "Game/Entity.h"
#include "Game/Timer.h"
#include "Game/Rect.h"
#include "Game/Tween.h"
// ----------------------------------------

#define time Application::Get().time_
#define timeUnscaled Application::Get().timeUnscaled_
#define dt Application::Get().dt_
#define dtUnscaled Application::Get().dtUnscaled_
#define frameCount Application::Get().frameCount_