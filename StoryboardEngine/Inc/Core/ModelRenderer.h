#pragma once
#include "Core/DrawableComponent.h"

namespace StoryboardEngine
{
	class ModelRenderer : public DrawableComponent
	{
		size_t modelResourceID = 0;
		size_t textureResourceID = 0;
	public:
		void OnDraw(ID3D11DeviceContext* deviceContext) override;
		void OnDrawInspector() override;
	};
}
