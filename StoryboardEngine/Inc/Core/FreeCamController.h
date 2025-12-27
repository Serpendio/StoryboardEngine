#pragma once
#include "Core/SceneComponent.h"

namespace StoryboardEngine
{
	class FreeCamController : public SceneComponent
	{
	public:
		void OnUpdate() override;

	private:
		float m_moveSpeed = 0.05f;
		float m_rotateSpeed = .25f;
	};
}
