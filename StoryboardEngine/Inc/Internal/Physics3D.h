#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <functional>
#include "Core/ContactListener.h"
#include "Jolt/Core/JobSystemThreadPool.h"

// Note: Credit for this class originally goes to the jolt hello world file, which was then adapted for Overscope Inc's BlueShift: Command, and finally for Storyboard Engine.

namespace StoryboardEngine
{
	class Physics3D
	{
		static Physics3D* instance;

	public:
		static Physics3D* GetInstance() { return instance; }
		static void Init();
		static void Destroy();
		static void Update(float deltaTime, const std::function<void()>& onFixedUpdate);
		static void FixedUpdate(const std::function<void()>& onFixedUpdate);
		static JPH::PhysicsSystem& GetPhysicsSystem() { return instance->physics_system; }
		static void GetInterpolatedTransform(JPH::BodyID bodyID, JPH::RVec3& outPosition, JPH::Quat& outRotation);

		void ResetTimeAccumulator();

		static float cFixedTimestep;
		static float cFixedTimeScale;
		static int cCollisionSteps;

		// ToDo: Layers should be configurable
		struct PhysicsLayers
		{
			static constexpr JPH::ObjectLayer STATIC{ 0 };
			static constexpr JPH::ObjectLayer DYNAMIC{ 1 };
			static constexpr JPH::ObjectLayer NUM_LAYERS{ 2 };
		};

		class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
		{
		public:
			bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
		};

		struct BroadPhaseLayers
		{
			static constexpr JPH::BroadPhaseLayer STATIC{ 0 };
			static constexpr JPH::BroadPhaseLayer MOVING{ 1 };
			static constexpr JPH::uint NUM_LAYERS{ 2 };
		};

		/// Class that determines if an object layer can collide with a broadphase layer
		class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
		{
		public:
			bool ShouldCollide(JPH::ObjectLayer objectLayer, JPH::BroadPhaseLayer broadLayer) const override;
		};
		ObjectVsBroadPhaseLayerFilterImpl objectVsBroadphaseLayerFilter;
		ObjectLayerPairFilterImpl objectVsObjectLayerFilter;
		ContactListener contact_listener;

	private:
		class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
		{
			JPH::BroadPhaseLayer mObjectToBroadPhase[PhysicsLayers::NUM_LAYERS];

		public:
			BPLayerInterfaceImpl();

			JPH::uint GetNumBroadPhaseLayers() const override
			{
				return BroadPhaseLayers::NUM_LAYERS;
			}

			JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
			virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
			{
				switch ((BroadPhaseLayer::Type)inLayer)
				{
				case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
				case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
				default:													JPH_ASSERT(false); return "INVALID";
				}
			}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED
		};

		// Body states at the time of last update
		std::unordered_map<JPH::BodyID, std::pair<JPH::RVec3, JPH::Quat>> previousStates;

		JPH::PhysicsSystem physics_system;
		BPLayerInterfaceImpl broad_phase_layer_interface;
		std::unique_ptr<JPH::TempAllocatorImpl> temp_allocator;
		// JPH::JobSystemSingleThreaded job_system;
		JPH::JobSystemThreadPool job_system;
		float timeAccumulator = 0.0f;
	};
}
