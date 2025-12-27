#pragma once
#include "Core/SceneComponent.h"

struct ID3D11DeviceContext;

namespace StoryboardEngine
{
    class DrawableComponent : public SceneComponent
    {
        int renderPriority = 0;
    public:
        virtual void OnAwake() override;
        virtual void OnAwakeEditor() override;
        virtual void OnDraw(ID3D11DeviceContext* deviceContext) = 0;
        virtual void OnDrawInspector() override;
        virtual void OnDestroy();

        // Higher priority drawn after lower priority
        int GetRenderPriority() const;
        // Higher priority drawn after lower priority
        void SetRenderPriority(int priority);

		ADD_SERIALIZATION(SceneComponent, StoryboardEngine::DrawableComponent, JSON_VARIABLE(renderPriority))
		
        // Although default would be nice, would have to adjust the macro quite a bit to allow defaults for abstract
        //ADD_SERIALIZATION_WITH_DEFAULT(SceneComponent, StoryboardEngine::DrawableComponent, JSON_VARIABLE_WITH_DEFAULT(renderPriority))
    };
}
