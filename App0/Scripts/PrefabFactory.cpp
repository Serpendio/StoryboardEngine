#include "pch.h"
#include "PrefabFactory.h"
#include "Utils/MathUtils.h"
#include "Components/Target.h"
#include "Core/RigidBody.h"
#include "Components/Projectile.h"
#include "Core/ModelRenderer.h"
#include "Core/ResourceManager.h"

SceneReference<RigidBody> PrefabFactory::CreateProjectile(SceneReference<SceneObject> parent, const Vector3& position, const Vector3& direction, float speed)
{
	Vector3 dirNormalized;
	direction.Normalize(dirNormalized);

	auto projectile = parent->AddChild();
	projectile->name = "Projectile";

	auto transform = projectile->GetTransform();
	transform->SetPosition(position);
	transform->SetRotation(Vector3(90, 0, 0));
	transform->SetScale(Vector3::One * 0.05f);

	auto rigidBody = projectile->AddComponent<RigidBody>();
	rigidBody->SetMotionType(JPH::EMotionType::Dynamic);
	rigidBody->SetVelocity(dirNormalized * speed);

	auto meshComp = projectile->AddComponent<ModelRenderer>();
	meshComp->colourTint = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	projectile->AddComponent<Projectile>();

	return rigidBody;
}

SceneReference<Target> PrefabFactory::CreateTarget(SceneReference<SceneObject> parent)
{
	Vector3 spawnPosition = Vector3(0.0f, 0.0f, 5.0f);
	float rotationX = static_cast<float>(rand() % 30) + 200.f; // 20degrees from horizontal
	float rotationY = static_cast<float>(rand() % 360);

	/*Quaternion rotationQuat = Quaternion::CreateFromYawPitchRoll(
		MathUtils::DegToRad(rotationY),
		MathUtils::DegToRad(rotationX),
		0.0f);*/

	Quaternion pitchQuat = Quaternion::CreateFromAxisAngle(Vector3::UnitX, MathUtils::DegToRad(rotationX));
	Quaternion yawQuat = Quaternion::CreateFromAxisAngle(Vector3::UnitY, MathUtils::DegToRad(rotationY));
	Quaternion rotationQuat = pitchQuat * yawQuat;

	spawnPosition = Vector3::Transform(spawnPosition, rotationQuat);

	auto targetBase = parent->AddChild();
	targetBase->name = "TargetBase";

	auto targetTransform = targetBase->GetTransform();
	targetTransform->SetPosition(spawnPosition);
	targetTransform->SetScale(Vector3::One * .8f);
	targetTransform->LookAt(Vector3(0, 0, 0), Vector3::Up);

	auto physBody = targetBase->AddChild();
	physBody->name = "PhysicsBody";

	auto physRig = physBody->AddComponent<RigidBody>();
	physRig->SetMotionType(JPH::EMotionType::Static);

	auto meshBody = targetBase->AddChild();
	meshBody->name = "MeshBody";

	auto meshTransform = meshBody->GetTransform();
	meshTransform->SetPosition(Vector3(0, -1.5f, 0));
	meshTransform->SetRotation(Vector3(0, 90, 0));
	meshTransform->SetScale(Vector3::One * 0.03f);

	auto meshComp = meshBody->AddComponent<ModelRenderer>();
	meshComp->modelResourceID = ResourceManager::GetModelID("Target");
	meshComp->textureResourceID = ResourceManager::GetTextureID("Target");

	auto targetComp = physBody->AddComponent<Target>();

	return targetComp;
}
