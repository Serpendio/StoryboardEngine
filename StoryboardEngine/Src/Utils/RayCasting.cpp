#include "pch.h"
#include "Utils/RayCasting.h"
#include "Utils/ApplicationUtils.h"
#include "Core/CameraComponent.h"
#include "Core/SceneTransform.h"

void StoryboardEngine::Utils::RayCollector::AddHit(const ResultType& inResult)
{
	// Only store the hit if it's the closest one so far (but also we're immediately done after the first hit, as that should be the closest anyway I think)
    if (!hasHit || inResult.mFraction < hitData.mFraction)
    {
        hitData = inResult;
        hasHit = true;
        ForceEarlyOut();
    }
}

void StoryboardEngine::Utils::RayCollectorAll::AddHit(const ResultType& inResult)
{
	hits.push_back(inResult);
}

JPH::RayCast StoryboardEngine::Utils::ScreenPointToRay(const SceneReference<CameraComponent>& camera, const Vector2& screenPoint, float rayLength)
{
    if (!camera)
        return {};

    const Vector2 ndc = { screenPoint.x / ApplicationUtils::GetScreenSize().x * 2.0f - 1.0f, 1.0f - screenPoint.y / ApplicationUtils::GetScreenSize().y * 2.0f };
    const Vector4 clip = { ndc.x, ndc.y, 0.0f, 1.0f };

    const Matrix projInv = CameraComponent::GetProjectionMatrix().Invert(); // cache if possible
    const Matrix viewInv = camera->GetViewMatrix().Invert(); // cache if possible

    // Transform to view space then rotate to world direction
    const Vector4 viewPos = DirectX::XMVector4Transform(clip, projInv);
    const Vector3 dirView = DirectX::XMVector3Normalize(Vector3(viewPos) / viewPos.w);
    const Vector3 dirWorld = DirectX::XMVector3TransformNormal(dirView, viewInv);

    return JPH::RayCast(JPH::Vec3(camera->GetTransform()->GetGlobalPosition()), JPH::Vec3(dirWorld * rayLength));
}
