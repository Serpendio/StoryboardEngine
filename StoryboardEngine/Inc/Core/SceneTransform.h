#pragma once
#include "Core/SceneComponent.h"

namespace StoryboardEngine
{
	class SceneObject;

	class SceneTransform : public SceneComponent
	{
	public:
		virtual void OnDrawInspector() override;
		virtual void OnDrawDebugInspector() override;

		Matrix GetMatrix() const;
		Vector3 GetUp() const;
		Vector3 GetForward() const;
		Vector3 GetRight() const;
		Vector3 GetLocalRotation() const;
		Vector3 GetGlobalRotation() const; // ToDo: Implement global rotation
		Vector3 GetLocalPosition() const;
		Vector3 GetGlobalPosition() const; // This'll probably change once we implement global rotation/scale
		Vector3 GetLocalScale() const;
		Vector3 GetGlobalScale() const; // ToDo: Implement global scale
		void SetRotation(const Vector3& rotation);
		void SetPosition(const Vector3& position);
		void SetScale(const Vector3& scale);
		void LookAt(const Vector3& position, const Vector3& upVec);
		void FaceDirection(const Vector3& direction, const Vector3& upVec);
		bool GetChangedThisFrame() const;
		void SetChangedThisFrame();
		void ClearChangedThisFrame();
		virtual void OnFinishedLoading() override;

		ADD_SERIALIZATION(SceneComponent, SceneTransform,
			JSON_VARIABLE(position), JSON_VARIABLE(rotation), JSON_VARIABLE(scale))
	private:
		void SyncPhysicsBodyToJolt();
		void SyncPhysicsBodyFromJolt();

		friend class Scene;
	private:
		Vector3 position = Vector3(0, 0, 0);
		Vector3 rotation = Vector3(0, 0, 0); // rotation is in degrees, euler angles
		Vector3 scale = Vector3(1, 1, 1);
		Vector3 upVec = Vector3(0, 1, 0);
		Vector3 forwardVec = Vector3(0, 0, 1);
		Vector3 rightVec = Vector3(1, 0, 0);

		bool changedThisFrame = true;
		static bool suppressPhysicsSync;
	};
}