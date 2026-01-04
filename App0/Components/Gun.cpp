#include "pch.h"
#include "Gun.h"
#include "Scripts/PrefabFactory.h"

void Gun::Fire()
{
	PrefabFactory::CreateProjectile(
		GetSceneObject()->GetScene()->GetRoot(),
		GetTransform()->GetGlobalPosition(),
		GetTransform()->GetGlobalForward(),
		25.0f);
}
