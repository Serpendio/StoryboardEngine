#include "pch.h"
#include "MainMenu.h"
#include "imgui.h"
#include "Utils/ApplicationUtils.h"
#include "Utils/SceneManager.h"

void MainMenu::OnConstructed()
{
	this->SetRenderPriority(100);
}

// ToDo: Nicer UI API
// ToDo: Buttons shouldn't be able to work in edit mode
void MainMenu::OnDraw(ID3D11DeviceContext* deviceContext)
{
	//int swidth, sheight;
	//StoryboardEngine::ApplicationUtils::GetScreenSize(swidth, sheight);

	//ImGui::SetNextWindowBgAlpha(0);
	//ImGui::Begin("Menu", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);
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
