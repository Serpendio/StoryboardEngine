#include "pch.h"
#include "nameof.hpp"
#include "Internal/ApplicationCore.h"
// #pragma comment(linker, "/ENTRY:mainCRTStartup")

//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR pScmdline, _In_ int iCmdshow)
int main(int argc, char* argv[])
{
	StoryboardEngine::ApplicationCore* app;
	bool result;

	// Create the system object.
	app = new StoryboardEngine::ApplicationCore();

	// Initialize and run the system object.
	result = app->Initialize();
	if (result)
	{
		app->Run();
	}

	// Shutdown and release the system object.
	app->Shutdown();
	delete app;
	app = nullptr;

	return 0;
}

