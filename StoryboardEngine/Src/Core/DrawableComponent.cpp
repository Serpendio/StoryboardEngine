#include "pch.h"
#include "Core/DrawableComponent.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "imgui.h"

void StoryboardEngine::DrawableComponent::OnAwake()
{
	SceneComponent::OnAwake();

	GetSceneObject()->GetScene()->RegisterDrawable(GetSelfReference());
}

void StoryboardEngine::DrawableComponent::OnAwakeEditor()
{
	SceneComponent::OnAwakeEditor();

	GetSceneObject()->GetScene()->RegisterDrawable(GetSelfReference());
}

void StoryboardEngine::DrawableComponent::OnDraw(ID3D11DeviceContext* deviceContext)
{
}

/*void StoryboardEngine::DrawableComponent::OnDrawGUI()
{
}*/

void StoryboardEngine::DrawableComponent::OnDrawInspector()
{
	SceneComponent::OnDrawInspector();

	ImGui::DragInt("Render Priority", &renderPriority);
}

void StoryboardEngine::DrawableComponent::OnDestroy()
{
	GetSceneObject()->GetScene()->DeregisterDrawable(GetSelfReference());
}

int StoryboardEngine::DrawableComponent::GetRenderPriority() const
{
	return renderPriority;
}

void StoryboardEngine::DrawableComponent::SetRenderPriority(int priority)
{
	renderPriority = priority;
}

