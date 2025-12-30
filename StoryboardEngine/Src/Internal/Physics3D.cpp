#include "pch.h"
#include "Internal/Physics3D.h"
#include <cstdarg>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

// Note: Credit for this class originally goes to the jolt hello world file, which was then adapted for Overscope Inc's BlueShift: Command, and finally for Storyboard Engine.

using namespace JPH::literals;

StoryboardEngine::Physics3D* StoryboardEngine::Physics3D::instance = nullptr;

float StoryboardEngine::Physics3D::cFixedTimestep = 1.0f / 30.0f;
float StoryboardEngine::Physics3D::cFixedTimeScale = 1.0f;
int StoryboardEngine::Physics3D::cCollisionSteps = 1;

// using these from the demo file for now
static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	std::cout << buffer << std::endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
	// Print to the TTY
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;

	// Breakpoint
	return true;
};

#endif

void StoryboardEngine::Physics3D::Init()
{
	instance = new Physics3D();
	JPH::RegisterDefaultAllocator();

	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();
	instance->temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);
	instance->job_system.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
	constexpr JPH::uint cMaxBodies = 65536;
	constexpr JPH::uint cNumBodyMutexes = 0;
	constexpr JPH::uint cMaxBodyPairs = 65536;
	constexpr JPH::uint cMaxContactConstraints = 10240;
	instance->physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, instance->broad_phase_layer_interface, instance->objectVsBroadphaseLayerFilter, instance->objectVsObjectLayerFilter);
	instance->physics_system.SetContactListener(&instance->contact_listener);
}

void StoryboardEngine::Physics3D::Destroy()
{
	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
	delete instance;
}

void StoryboardEngine::Physics3D::Update(float deltaTime, const std::function<void()>& onFixedUpdate)
{
	instance->timeAccumulator += deltaTime * cFixedTimeScale;

	while (instance->timeAccumulator >= cFixedTimestep)
	{
		instance->previousStates.clear();
		JPH::BodyIDVector bodyIDs;
		instance->physics_system.GetBodies(bodyIDs);
		for (const auto& bodyID : bodyIDs)
		{
			JPH::RVec3 position;
			JPH::Quat rotation;
			instance->physics_system.GetBodyInterface().GetPositionAndRotation(bodyID, position, rotation);
			instance->previousStates[bodyID] = std::make_pair(position, rotation);
		}

		instance->physics_system.Update(cFixedTimestep, cCollisionSteps, instance->temp_allocator.get(), &instance->job_system);
		instance->timeAccumulator -= cFixedTimestep;
		onFixedUpdate();
	}
}

void StoryboardEngine::Physics3D::FixedUpdate(const std::function<void()>& onFixedUpdate)
{
	instance->previousStates.clear();

	onFixedUpdate();
	instance->physics_system.Update(cFixedTimestep, cCollisionSteps, instance->temp_allocator.get(), &instance->job_system);
}

static JPH::RVec3 Lerp(const JPH::RVec3& a, const JPH::RVec3& b, const float t)
{
	// it has a quaternion lerp but not a vector lerp? what a peculiar library
	return a + (b - a) * t;
}

void StoryboardEngine::Physics3D::GetInterpolatedTransform(const JPH::BodyID bodyID, JPH::RVec3& outPosition, JPH::Quat& outRotation)
{
	const auto body = instance->physics_system.GetBodyLockInterface().TryGetBody(bodyID);
	if (!body) return;

	const float progress = instance->timeAccumulator / cFixedTimestep;

	// Current state
	const JPH::RVec3 currentPosition = body->GetPosition();
	const JPH::Quat currentRotation = body->GetRotation();

	// Interpolate between steps if possible
	if (const auto it = instance->previousStates.find(bodyID); it != instance->previousStates.end())
	{
		const auto [prevPos, prevRot] = it->second;

		outPosition = Lerp(prevPos, currentPosition, progress);
		outRotation = prevRot.SLERP(currentRotation, progress);
	}
	else
	{
		outPosition = currentPosition;
		outRotation = currentRotation;
	}
}

void StoryboardEngine::Physics3D::ResetTimeAccumulator()
{
	instance->timeAccumulator = 0.0f;
}

bool StoryboardEngine::Physics3D::ObjectLayerPairFilterImpl::ShouldCollide(const JPH::ObjectLayer inObject1, const JPH::ObjectLayer inObject2) const
{
	const auto layer1 = std::min(inObject1, inObject2);
	const auto layer2 = std::max(inObject1, inObject2);

	// Since we check the lower against the higher layer, each relationship only has to be defined once
	// (ie, we don't have to check layer1 in layer2, because layer2 is already checked in layer1)
	// to ensure this relationship works, make sure you always check the higher layer inside the case of the lower layer
	// (ie, check layer2 inside layer1, because layer1 is the lower layer)
	// i.... think that's how this works?
	switch (layer1)
	{
	case PhysicsLayers::STATIC:
		return layer2 != PhysicsLayers::STATIC; // Static objects shouldn't collide with eachother
	case PhysicsLayers::DYNAMIC:
		return true; // Dynamic objects collide with everything
	default:
		JPH_ASSERT(false);
		return false;
	}
}

StoryboardEngine::Physics3D::BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
	// Create a mapping table from object to broad phase layer
	mObjectToBroadPhase[PhysicsLayers::STATIC] = BroadPhaseLayers::STATIC;

	mObjectToBroadPhase[PhysicsLayers::DYNAMIC] = BroadPhaseLayers::MOVING;
}

JPH::BroadPhaseLayer StoryboardEngine::Physics3D::BPLayerInterfaceImpl::GetBroadPhaseLayer(const JPH::ObjectLayer inLayer) const
{
	JPH_ASSERT(inLayer < PhysicsLayers::NUM_LAYERS);
	return mObjectToBroadPhase[inLayer];
}

bool StoryboardEngine::Physics3D::ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(const JPH::ObjectLayer objectLayer, const JPH::BroadPhaseLayer broadLayer) const
{
	switch (objectLayer)
	{
	case PhysicsLayers::STATIC:
		return broadLayer == BroadPhaseLayers::MOVING;
	default:
		return true;
	}
}
