#include "pch.h"
#include "ReferenceLinkTest.h"
#include "Utils/SceneReferenceGUIDrawer.h"

void ReferenceLinkTest::OnDrawInspector()
{
	StoryboardEngine::SceneReferenceGUIDrawer::DrawSceneReferenceGUI<StoryboardEngine::SceneObject>("Referenced Object", referencedObject);
	StoryboardEngine::SceneReferenceGUIDrawer::DrawSceneReferenceGUI<StoryboardEngine::SceneComponent>("Referenced Component", referencedComponent);
	StoryboardEngine::SceneReferenceGUIDrawer::DrawSceneReferenceGUI<StoryboardEngine::SceneTransform>("Referenced Transform", referencedTransform);
}

void ReferenceLinkTest::OnDrawDebugInspector()
{
	if (referencedObject)
	{
		ImGui::Text("Referenced Object UUID: %s", referencedObject->GetUUID().ToString().c_str());
	}
	else
	{
		ImGui::Text("Referenced Object UUID: None");
	}

	if (referencedComponent)
	{
		ImGui::Text("Referenced Component Type: %s", referencedComponent->GetSerializedType().c_str());
	}
	else
	{
		ImGui::Text("Referenced Component Type: None");
	}

	if (referencedTransform)
	{
		ImGui::Text("Referenced Transform Position: (%.2f, %.2f, %.2f)", referencedTransform->GetLocalPosition().x, referencedTransform->GetLocalPosition().y, referencedTransform->GetLocalPosition().z);
	}
	else
	{
		ImGui::Text("Referenced Transform Position: None");
	}
}
