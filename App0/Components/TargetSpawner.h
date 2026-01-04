#pragma once
#include "pch.h"

using namespace StoryboardEngine;
 
class Target;
class Scoreboard;

class TargetSpawner : public SceneComponent
{
private:
	float timeSinceLastSpawn = 0.0f;
	std::vector<SceneReference<Target>> activeTargets;

	SceneReference<Scoreboard> scoreboard;
	float spawnInterval = 2.0f;
	int maxTargets = 10;

public:
	virtual void OnUpdate() override;
	virtual void OnDrawInspector() override;
	virtual void OnDrawDebugInspector() override;

	ADD_SERIALIZATION(SceneComponent, TargetSpawner,
		JSON_VARIABLE(spawnInterval),
		JSON_VARIABLE(maxTargets),
		JSON_VARIABLE(scoreboard))
};

