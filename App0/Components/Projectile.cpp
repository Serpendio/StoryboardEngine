#include "pch.h"
#include "Projectile.h"
#include "Utils/Time.h"
#include <imgui.h>

void Projectile::OnUpdate()
{
	lifeTime -= StoryboardEngine::Time::GetDeltaTime();
	if (lifeTime <= 0.0f)
	{
		DestroyObject(GetSceneObject());
	}
}

void Projectile::OnCollisionEnter(SceneReference<SceneObject> other)
{
	if (other->name == "Player")
		return;

	DestroyObject(GetSceneObject());
}

void Projectile::OnDrawDebugInspector()
{
	ImGui::TextDisabled("Life Time: %.2f", lifeTime);
}
