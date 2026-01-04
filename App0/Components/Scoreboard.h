#pragma once
#include "pch.h"
#include "Core/DrawableComponent.h"

using namespace StoryboardEngine;

class Scoreboard : public DrawableComponent
{
	int score = 0;
public:
	void AddScore(int points) { score += points; }
	
	virtual void OnDraw(ID3D11DeviceContext* deviceContext) override;
};

