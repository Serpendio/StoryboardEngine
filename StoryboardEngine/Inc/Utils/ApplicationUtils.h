#pragma once

namespace StoryboardEngine
{
	class ApplicationUtils
	{
		static bool deserializingScene;
#ifdef _EDITOR
		static bool isPlaying;
#endif

		friend class Scene;
		friend class EditorLayer;
	public:
		static constexpr bool InEditor()
		{
#ifdef _EDITOR
			return true;
#else
			return false;
#endif
		}
		static bool IsDeserializingScene();
		static bool IsPlaying();
	};
}