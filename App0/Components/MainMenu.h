#pragma once
#include "pch.h"
#include "Core/DrawableComponent.h"

class MainMenu : public StoryboardEngine::DrawableComponent
{
public:
	void OnConstructed() override;
	void OnDraw(ID3D11DeviceContext* deviceContext) override;
};

