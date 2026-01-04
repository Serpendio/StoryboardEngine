#pragma once
#include "pch.h"

using namespace StoryboardEngine;

class Projectile : public SceneComponent
{
	float lifeTime = 3.0f;

public:
	void OnUpdate() override;
	void OnCollisionEnter(SceneReference<SceneObject> other) override;
	void OnDrawDebugInspector() override;
};

