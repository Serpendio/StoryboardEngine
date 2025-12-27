#pragma once
#include <directxmath.h>
#include "Core/SceneComponent.h"

namespace StoryboardEngine
{
	class CameraComponent : public SceneComponent
	{
	public:
		void OnUpdate() override;
		void Render();
		const Matrix& GetViewMatrix() const;

	private:
		Matrix m_viewMatrix;
	};
}
