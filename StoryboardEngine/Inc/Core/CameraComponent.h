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
		static const Matrix& GetProjectionMatrix();

	private:
		Matrix m_viewMatrix;
		static Matrix s_projectionMatrix;

		friend class ApplicationCore;
	};
}
