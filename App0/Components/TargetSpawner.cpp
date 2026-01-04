#include "pch.h"
#include "TargetSpawner.h"
#include "Utils/Time.h"
#include <imgui.h>
#include "Utils/SceneReferenceGUIDrawer.h"
#include "Scripts/PrefabFactory.h"
#include "Components/Target.h"
#include "Components/Scoreboard.h"

void TargetSpawner::OnUpdate()
{
	timeSinceLastSpawn += StoryboardEngine::Time::GetDeltaTime();

	// Clean up any destroyed targets from the activeTargets list
	activeTargets.erase(
		std::remove_if(activeTargets.begin(), activeTargets.end(),
			[](const SceneReference<Target>& targetRef)
			{
				return !targetRef;
			}),
		activeTargets.end());

	if (timeSinceLastSpawn >= spawnInterval && static_cast<int>(activeTargets.size()) < maxTargets)
	{
		auto newTarget = PrefabFactory::CreateTarget(GetSceneObject());
		newTarget->scoreboard = scoreboard;
		activeTargets.push_back(newTarget);
		timeSinceLastSpawn = 0.0f;
	}
}

void TargetSpawner::OnDrawInspector()
{
	SceneReferenceGUIDrawer::DrawSceneReferenceGUI("Scoreboard", scoreboard);
	ImGui::DragFloat("Spawn Interval", &spawnInterval, 0.1f, 0.1f, 60.0f);
	ImGui::DragInt("Max Targets", &maxTargets, 1, 1, 100);
}

void TargetSpawner::OnDrawDebugInspector()
{
	if (ImGui::TreeNode("Targets"))
	{
		ImGui::BeginDisabled();
		for (auto& target : activeTargets)
		{
			SceneReferenceGUIDrawer::DrawSceneReferenceGUI("Target", target);
		}
		ImGui::EndDisabled();
		ImGui::TreePop();
	}
}
