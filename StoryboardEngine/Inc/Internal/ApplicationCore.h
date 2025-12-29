#pragma once

// Starting Settings
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct HWND__;
typedef struct HWND__* HWND;

struct SDL_Window;

namespace StoryboardEngine
{
	class D3DRenderer;
	class ResourceManager;
	class ColourShader;

	class ApplicationCore
	{
	public:
		ApplicationCore();
		ApplicationCore(const ApplicationCore&) = delete;
		ApplicationCore& operator=(const ApplicationCore&) = delete;
		~ApplicationCore();

		bool Initialize();
		void Shutdown();
		void Run();

	private:
		void Frame();

	private:
		HWND m_HWND;
		SDL_Window* m_Window;

		ColourShader* m_ColourShader;

		D3DRenderer* m_Direct3D;

		// Purely to stop multiple instances
		static ApplicationCore* s_instance;

		bool m_isFullscreen;
	};
}
