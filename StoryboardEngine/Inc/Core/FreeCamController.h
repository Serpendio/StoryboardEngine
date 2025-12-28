#pragma once
#include "Core/SceneComponent.h"

namespace StoryboardEngine
{
	class FreeCamController : public SceneComponent
	{
	private:
		float m_moveSpeed = 10.f;
		float m_rotateSpeed = 45.f;

	public:
		void OnUpdate() override;
		void OnDrawInspector() override;

		ADD_SERIALIZATION(SceneComponent, FreeCamController, JSON_VARIABLE(m_moveSpeed), JSON_VARIABLE(m_rotateSpeed))
	};
}
