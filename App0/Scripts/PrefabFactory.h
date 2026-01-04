#pragma once
#include "pch.h"
#include "Core/RigidBody.h"

using namespace StoryboardEngine;

class Target;

namespace PrefabFactory
{
	SceneReference<RigidBody> CreateProjectile(SceneReference<SceneObject> parent, const Vector3& position, const Vector3& direction, float speed);
	SceneReference<Target> CreateTarget(SceneReference<SceneObject> parent);
};

