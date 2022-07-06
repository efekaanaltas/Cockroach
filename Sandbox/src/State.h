#pragma once

#include "Components.h"

template<typename T>
class State
{
public:
	virtual void Enter(T obj) = 0;
	virtual void Update(T obj) = 0;
	virtual void Exit(T obj) = 0;
};