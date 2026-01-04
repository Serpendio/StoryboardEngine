#include "pch.h"
#include "MainMenu.h"
#include "imgui.h"
#include "Utils/ApplicationUtils.h"
#include "Utils/SceneManager.h"

void MainMenu::OnConstructed()
{
	this->SetRenderPriority(100);
}

// ToDo: These todos should be on main
// ToDo: Nicer UI API
// ToDo: Buttons shouldn't be able to work in edit mode
// ToDo: Can accidentally rename a second object when selecting that while renaming the first
void MainMenu::OnDraw(ID3D11DeviceContext* deviceContext)
{
	ImGui::Begin("Menu");

	if (ImGui::Button("Load Game"))
	{
		StoryboardEngine::SceneManager::LoadScene("GameScene");
	}

	if (ImGui::Button("Quit"))
	{
		StoryboardEngine::ApplicationUtils::Quit();
	}

	ImGui::End();
}
