#include "pch.h"
#include "Utils/SceneReference.h"

std::vector<std::pair<StoryboardEngine::UUID, StoryboardEngine::SceneReferenceBase*>> StoryboardEngine::SceneReferenceBase::objectsToLink;

StoryboardEngine::SceneReferenceBase::~SceneReferenceBase() {}