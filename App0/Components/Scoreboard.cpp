#include "pch.h"
#include "Scoreboard.h"
#include <imgui.h>

void Scoreboard::OnDraw(ID3D11DeviceContext* deviceContext)
{
	ImGui::Begin("Scoreboard");
	ImGui::Text("Score: %d", score);
	ImGui::End();
}
