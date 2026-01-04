#pragma once
#include "pch.h"
#include "Core/Rigidbody.h"

using namespace StoryboardEngine;

class Gun;

class FirstPersonController : public SceneComponent
{
	SceneReference<RigidBody> rigidBody;

	SceneReference<Gun> gun;
	float lookSensitivity = 45.f;
	float moveSpeed = 10.f;
	float jumpSpeed = 5.f;
public:
	virtual void OnAwake() override;
	virtual void OnUpdate() override;
	virtual void OnDrawInspector() override;
	virtual void OnDrawDebugInspector() override;

	ADD_SERIALIZATION(SceneComponent, FirstPersonController,
		JSON_VARIABLE(lookSensitivity),
		JSON_VARIABLE(moveSpeed),
		JSON_VARIABLE(jumpSpeed),
		JSON_VARIABLE(gun))
};

