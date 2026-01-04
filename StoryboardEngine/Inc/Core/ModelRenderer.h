#pragma once
#include "Core/DrawableComponent.h"

namespace StoryboardEngine
{
	class ModelRenderer : public DrawableComponent
	{
		// ToDo: Better to serialize names, as if the file path changes then the id will differ, but a user can manually specify the same name for a different path
	public:
		size_t modelResourceID = 0;
		size_t textureResourceID = 0;
		Vector4 colourTint = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	public:
		void OnDraw(ID3D11DeviceContext* deviceContext) override;
		void OnDrawInspector() override;

		ADD_SERIALIZATION(DrawableComponent, ModelRenderer, 
			JSON_VARIABLE(modelResourceID), JSON_VARIABLE(textureResourceID), JSON_VARIABLE(colourTint))
	};
}
