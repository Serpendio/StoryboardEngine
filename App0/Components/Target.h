#pragma once
#include "pch.h"

using namespace StoryboardEngine;

class Scoreboard;

class Target : public SceneComponent
{
	float lifeTime = 5.0f;

public:
	SceneReference<Scoreboard> scoreboard;

public:
	virtual void OnUpdate() override;
	virtual void OnDrawDebugInspector() override;
	virtual void OnCollisionEnter(SceneReference<SceneObject> other) override;
};

