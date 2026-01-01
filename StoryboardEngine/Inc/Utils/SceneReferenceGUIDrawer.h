#pragma once
#include "pch.h"
#include <functional>
#include "imgui.h"
#include "Core/SerializableObject.h"
#include "Utils/SceneReference.h"

namespace StoryboardEngine
{
	class SceneReferenceGUIDrawer
	{
	public:
		template<typename T>
		static bool DrawSceneReferenceGUI(const std::string& label, SceneReference<T>& sceneRef, std::function<bool(const SceneReference<T>&)> filter = nullptr)
		{
			static_assert(std::is_base_of<SerializableObject, T>::value, "SceneReferenceGUIDrawer can only be used with SerializableObject derived types");
			// ToDo: Add drag and drop support or at least object picker

			// Dropdown for all objects of type T
			
			// if scene object we can use sceneRef->name directly, otherwise we'll make the name look like TypeName (ObjectName)

			bool changed = false;

			std::string displayLabel;
			if constexpr (std::is_same<T, SceneObject>::value)
				displayLabel = sceneRef ? sceneRef->name : "None";
			else
				displayLabel = sceneRef ? (std::string(NAMEOF_SHORT_TYPE(T)) + " (" + sceneRef->GetSceneObject()->name + ")") : "None";

			if (ImGui::BeginCombo(label.c_str(), displayLabel.c_str()))
			{
				for (const auto& [uuid, obj] : SerializableObject::idToObject)
				{
					if (auto casted = std::dynamic_pointer_cast<T>(obj))
					{
						if (filter && !filter(SceneReference<T>(casted)))
						{
							continue;
						}

						bool isSelected = (sceneRef == casted);
						if constexpr (std::is_same<T, SceneObject>::value)
						{
							displayLabel = casted->name;
						}
						else
						{
							displayLabel = std::string(NAMEOF_SHORT_TYPE(T)) + " (" + casted->GetSceneObject()->name + ")";
						}

						ImGui::PushID(uuid.ToString().c_str());
						if (ImGui::Selectable(displayLabel.c_str(), isSelected))
						{
							sceneRef = SceneReference<T>(casted);
							changed = true;
						}
						ImGui::PopID();
					}
				}
				ImGui::EndCombo();
			}

			return changed;
		}
	};
}
