#pragma once
#include "Core/SceneComponent.h"
class ReferenceLinkTest : public StoryboardEngine::SceneComponent
{
	StoryboardEngine::SceneReference<StoryboardEngine::SceneObject> referencedObject;
	StoryboardEngine::SceneReference<StoryboardEngine::SceneComponent> referencedComponent;
	StoryboardEngine::SceneReference<StoryboardEngine::SceneTransform> referencedTransform;
public:
	void OnDrawInspector() override;
	void OnDrawDebugInspector() override;

	ADD_SERIALIZATION(StoryboardEngine::SceneComponent, ReferenceLinkTest,
		JSON_VARIABLE(referencedObject),
		JSON_VARIABLE(referencedComponent),
		JSON_VARIABLE(referencedTransform))
};

