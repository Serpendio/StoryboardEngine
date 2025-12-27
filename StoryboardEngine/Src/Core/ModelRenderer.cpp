#include "pch.h"
#include "Core/ModelRenderer.h"
#include "Core/ResourceManager.h"
#include "Internal/ModelData.h"
#include "Internal/ColourShader.h"
#include "Core/SceneTransform.h"
#include "imgui.h"

void StoryboardEngine::ModelRenderer::OnDraw(ID3D11DeviceContext* deviceContext)
{
	ModelData* modelData = ResourceManager::GetModelResource(modelResourceID);

	if (modelData)
	{
		Matrix matrix = GetTransform()->GetMatrix();
		ColourShader::SetWorldMatrix(matrix);

		modelData->Render(deviceContext);
	}
}

void StoryboardEngine::ModelRenderer::OnDrawInspector()
{
	std::string modelName = ResourceManager::GetModelName(modelResourceID);
	if (ImGui::BeginCombo("Model", modelName.c_str()))
	{
		for (auto& possibleName : ResourceManager::GetAllModelNames())
		{
			const bool is_selected = (possibleName == modelName);
			if (ImGui::Selectable(possibleName.c_str(), is_selected))
			{
				modelResourceID = ResourceManager::GetModelID(possibleName);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	std::string textureName = ResourceManager::GetTextureName(textureResourceID);
	if (ImGui::BeginCombo("Texture", textureName.c_str()))
	{
		for (auto& possibleName : ResourceManager::GetAllTextureNames())
		{
			const bool is_selected = (possibleName == textureName);
			if (ImGui::Selectable(possibleName.c_str(), is_selected))
			{
				textureResourceID = ResourceManager::GetTextureID(possibleName);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}
