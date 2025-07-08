#include <vector>
#include <iostream>

#include "imgui.h"

#include "Lumina/Core/Layer.h"
#include "Lumina/Utils/Timer.h"
#include "Lumina/Utils/UUID.h"

namespace Lumina
{
    class Example : public Layer
    {
    public:
        virtual void OnAttach() override
        {
            m_UniqueID = UUID::Generate(); 
        }

        virtual void OnDetach() override
        {

        }

        virtual void OnUpdate(float timestep) override
        {
            float elapsedTime = m_FrameTimer.Elapsed();
            m_FPS = 1.0f / elapsedTime;
            m_FrameTimer.Reset();
        }

        virtual void OnUIRender() override
        {
            ImGui::Begin("Example Window");
            ImGui::Text("Hello World!");
            ImGui::Text("Unique ID: %d", m_UniqueID);
            ImGui::End();

            ImGui::Begin("FPS Counter");
            ImGui::Text("FPS: %.1f", m_FPS);
            ImGui::End();
        }

    private:
        Timer m_FrameTimer;
        float m_FPS = 0.0f;

        uint64_t m_UniqueID = 0; 
    };
}