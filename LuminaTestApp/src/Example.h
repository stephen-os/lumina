#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "imgui.h"

#include "Lumina/Core/Layer.h"
#include "Lumina/Core/Ref.h"
#include "Lumina/Core/Log.h"
#include "Lumina/Utils/Timer.h"
#include "Lumina/Graphics/Renderer.h"
#include "Lumina/Graphics/Texture.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"
#include "Lumina/Graphics/Cameras/PerspectiveCamera.h"

#include "Lumina/Utils/FileReader.h"
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Dev Dependencies
#include "Lumina/Core/Input.h"
#include "Lumina/Core/KeyCode.h"

class Example : public Lumina::Layer
{
public:
    virtual void OnAttach() override
    {
        uint32_t whiteTextureData = 0xffffffff;
        m_Texture = Lumina::Texture::Create(1, 1);
        m_Texture->SetData(&whiteTextureData, sizeof(uint32_t)); 
    }

    virtual void OnDetach() override {}

    virtual void OnUpdate(float ts) override
    {

    }

    virtual void OnUIRender() override
    {

        ImGui::Begin("Test");
        ImVec2 size = ImGui::GetContentRegionAvail(); 


        ImGui::Image((void*)m_Texture->GetID(), size);
        ImGui::End(); 

    }
private:
    Lumina::Ref<Lumina::Texture> m_Texture = nullptr; 

    Lumina::Timer m_FrameTimer;
    float m_FPS = 0.0f;
};
