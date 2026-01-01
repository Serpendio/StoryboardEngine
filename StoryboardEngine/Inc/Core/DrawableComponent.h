#pragma once
#include "Core/SceneComponent.h"

struct ID3D11DeviceContext;

namespace StoryboardEngine
{
    class DrawableComponent : public SceneComponent
    {
		// ToDo Cancel this todo, GUI is back to being handled in OnDraw// ToDo: Perhaps should have two different components for 3D and GUI drawing

        int renderPriority = 0;
    public:
        virtual void OnAwake() override;
        virtual void OnAwakeEditor() override;
        virtual void OnDraw(ID3D11DeviceContext* deviceContext) = 0;
		//virtual void OnDrawGUI();
        virtual void OnDrawInspector() override;
        virtual void OnDestroy();

        // Higher priority drawn after lower priority
        int GetRenderPriority() const;
        // Higher priority drawn after lower priority
        void SetRenderPriority(int priority);

		ADD_SERIALIZATION(SceneComponent, StoryboardEngine::DrawableComponent, JSON_VARIABLE(renderPriority))
		
        // Although default would be nice, would have to adjust the macro quite a bit to allow defaults for abstract
        // No longer abstract, but with default has less checks so will leave commented for now // Back to abstract
        //ADD_SERIALIZATION_WITH_DEFAULT(SceneComponent, StoryboardEngine::DrawableComponent, JSON_VARIABLE_WITH_DEFAULT(renderPriority))
    };
}
