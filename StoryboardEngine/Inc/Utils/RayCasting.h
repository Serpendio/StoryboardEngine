#pragma once
#include <vector>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include "Jolt/Physics/Collision/RayCast.h"
#include "Utils/SceneReference.h"

namespace StoryboardEngine
{
    class CameraComponent;
}

namespace StoryboardEngine::Utils
{
    // This ray collector will only store the closest hit
    class RayCollector : public JPH::RayCastBodyCollector
    {
    public:
        bool hasHit = false;
        ResultType hitData;

        virtual void AddHit(const ResultType& inResult) override;
    };

    class RayCollectorAll : public JPH::RayCastBodyCollector
	{
    public:
        std::vector<ResultType> hits;
        virtual void AddHit(const ResultType& inResult) override;
	};

    // Convert a screen-space point to a ray in world-space
    JPH::RayCast ScreenPointToRay(const SceneReference<CameraComponent>& camera, const Vector2& screenPoint, float rayLength = 1000.f);
}
