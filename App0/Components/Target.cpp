#include "pch.h"
#include "Target.h"
#include "Components/Scoreboard.h"
#include "Utils/Time.h"
#include <imgui.h>
#include "Components/Projectile.h"

void Target::OnUpdate()
{
	lifeTime -= StoryboardEngine::Time::GetDeltaTime();
	if (lifeTime <= 0.0f)
	{
		DestroyObject(GetSceneObject()->GetParent());
	}
}

void Target::OnDrawDebugInspector()
{
	ImGui::TextDisabled("Life Time: %.2f", lifeTime);
}

void Target::OnCollisionEnter(SceneReference<SceneObject> other)
{
	if (!other->GetComponent<Projectile>())
		return;

	// Destroy self on collision
	if (scoreboard)
	{
		scoreboard->AddScore(1);
	}

	DestroyObject(GetSceneObject()->GetParent());
}
