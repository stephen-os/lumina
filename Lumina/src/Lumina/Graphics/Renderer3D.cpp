#include "Renderer3D.h"

#include <iostream>
#include <vector>
#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Core/Log.h"
#include "../Core/Assert.h"
#include "../Utils/FileReader.h"

#include "TextureSlot.h"

namespace Lumina
{
    // Constants
    constexpr uint32_t MaxPointLights = 32;

    // Renderer data storage
    struct Renderer3DData
    {
        // Core framebuffer
        Ref<FrameBuffer> RendererFrameBuffer;

        // Shaders
        Ref<ShaderProgram> PBRShader = nullptr;
		Ref<ShaderProgram> PBRIShader = nullptr;
        Ref<ShaderProgram> SkyboxShader = nullptr;

        // Default textures
        Ref<Texture> WhiteTexture = nullptr;
        Ref<Texture> DefaultNormalMap = nullptr;

        // Lighting
        DirectionalLight DirLight;
        std::vector<PointLight> PointLights;

        // Environment
        Ref<Texture> EnvironmentMap = nullptr;
        // Note: No skybox data here anymore - it's in the Skybox class

        // Instance map 
        std::unordered_map<uint64_t, Ref<Instance>> m_ModelInstances;

        // Render settings
        RenderMode CurrentRenderMode = RenderMode::Normal;
        float GlobalPointSize = 1.0f;

        // View Projection
        glm::mat4 ViewMatrix = glm::mat4(1.0f);
        glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);
        glm::vec3 CameraPosition = glm::vec3(0.0f);

        // Framebuffer Resolution
        uint32_t Width = 800;
        uint32_t Height = 600;

        // Statistics
        Renderer3D::Statistics Stats;
    };

    static Renderer3DData s_Data;

    void Renderer3D::Init()
    {
        LUMINA_LOG_INFO("Renderer3D: Initializing...");

        // Create framebuffer
        s_Data.RendererFrameBuffer = FrameBuffer::Create();

        // Create default white texture
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture = Texture::Create(1, 1);
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        // Create default normal map (pointing up in tangent space)
        uint32_t normalData = 0xFF8080FF; // RGB(128, 128, 255) = normal pointing up
        s_Data.DefaultNormalMap = Texture::Create(1, 1);
        s_Data.DefaultNormalMap->SetData(&normalData, sizeof(uint32_t));

        // Load PBR shader
        {
            std::string vertexSource = ReadFile("res/shaders/PBR.vert");
            std::string fragmentSource = ReadFile("res/shaders/PBR.frag");
            s_Data.PBRShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Load PBRI shader (for instance rendering)
        {
            std::string vertexSource = ReadFile("res/shaders/PBRI.vert");
            std::string fragmentSource = ReadFile("res/shaders/PBRI.frag");
            s_Data.PBRIShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Load Skybox shader
        {
            std::string vertexSource = ReadFile("res/shaders/Skybox.vert");
            std::string fragmentSource = ReadFile("res/shaders/Skybox.frag");
            s_Data.SkyboxShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Set default lighting
        s_Data.DirLight.Direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
        s_Data.DirLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
        s_Data.DirLight.Intensity = 3.0f;

        LUMINA_LOG_INFO("Renderer3D: Initialization complete");
    }

    void Renderer3D::Shutdown()
    {
        LUMINA_LOG_INFO("Renderer3D: Shutting down...");
        s_Data.PointLights.clear();
        LUMINA_LOG_INFO("Renderer3D: Shutdown complete");
    }

    void Renderer3D::Begin(Camera& camera)
    {
        s_Data.ViewMatrix = camera.GetViewMatrix();
        s_Data.ProjectionMatrix = camera.GetProjectionMatrix();
        s_Data.ViewProjectionMatrix = s_Data.ProjectionMatrix * s_Data.ViewMatrix;
        s_Data.CameraPosition = camera.GetPosition();

        ResetStats();

        s_Data.RendererFrameBuffer->Bind();
        s_Data.RendererFrameBuffer->Resize(s_Data.Width, s_Data.Height);

        RenderCommands::SetViewport(0, 0, s_Data.Width, s_Data.Height);
        RenderCommands::SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        RenderCommands::Clear();
        RenderCommands::EnableDepthTest();
        RenderCommands::EnableProgramPointSize();
    }

    void Renderer3D::Begin(const glm::mat4& viewProjection)
    {
        s_Data.ViewProjectionMatrix = viewProjection;

        ResetStats();

        s_Data.RendererFrameBuffer->Bind();
        s_Data.RendererFrameBuffer->Resize(s_Data.Width, s_Data.Height);

        RenderCommands::SetViewport(0, 0, s_Data.Width, s_Data.Height);
        RenderCommands::SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        RenderCommands::Clear();
        RenderCommands::EnableDepthTest();
        RenderCommands::EnableProgramPointSize();
    }

    void Renderer3D::End()
    {
        Flush(); 

        s_Data.RendererFrameBuffer->Unbind();
    }

    void Renderer3D::Draw(const Ref<Model>& model, const ModelAttributes& attributes)
    {
		LUMINA_ASSERT(model, "Renderer3D: Cannot draw null model");

        glm::mat4 modelMatrix = attributes.GetModelMatrix();

        s_Data.PBRShader->Bind();

        s_Data.PBRShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
        s_Data.PBRShader->SetUniformMat4("u_Model", modelMatrix);

        s_Data.PBRShader->SetUniformVec3("u_CameraPos", s_Data.CameraPosition);

        s_Data.PBRShader->SetUniformVec4("u_TintColor", attributes.TintColor);

        s_Data.PBRShader->SetUniformInt("u_RenderMode", static_cast<int>(s_Data.CurrentRenderMode));

        s_Data.PBRShader->SetUniformVec3("u_DirLight.Direction", s_Data.DirLight.Direction);
        s_Data.PBRShader->SetUniformVec3("u_DirLight.Color", s_Data.DirLight.Color);
        s_Data.PBRShader->SetUniformFloat("u_DirLight.Intensity", s_Data.DirLight.Intensity);

        s_Data.PBRShader->SetUniformInt("u_NumPointLights", static_cast<int>(s_Data.PointLights.size()));

        for (size_t i = 0; i < s_Data.PointLights.size() && i < MaxPointLights; ++i)
        {
            std::string base = "u_PointLights[" + std::to_string(i) + "]";
            s_Data.PBRShader->SetUniformVec3(base + ".Position", s_Data.PointLights[i].Position);
            s_Data.PBRShader->SetUniformVec3(base + ".Color", s_Data.PointLights[i].Color);
            s_Data.PBRShader->SetUniformFloat(base + ".Intensity", s_Data.PointLights[i].Intensity);
            s_Data.PBRShader->SetUniformFloat(base + ".Constant", s_Data.PointLights[i].Constant);
            s_Data.PBRShader->SetUniformFloat(base + ".Linear", s_Data.PointLights[i].Linear);
            s_Data.PBRShader->SetUniformFloat(base + ".Quadratic", s_Data.PointLights[i].Quadratic);
        }

        if (s_Data.EnvironmentMap)
        {
            s_Data.EnvironmentMap->Bind(5);
            s_Data.PBRShader->SetUniformInt("u_EnvironmentMap", 5);
            s_Data.PBRShader->SetUniformInt("u_HasEnvironmentMap", 1);
        }
        else
        {
            s_Data.PBRShader->SetUniformInt("u_HasEnvironmentMap", 0);
        }

        if (s_Data.CurrentRenderMode == RenderMode::Points)
        {
            float pointSize = (attributes.PointSize > 0.0f) ? attributes.PointSize : s_Data.GlobalPointSize;
            RenderCommands::SetPointSize(pointSize);
            s_Data.PBRShader->SetUniformFloat("u_PointSize", pointSize);
        }

        const auto& meshes = model->GetMeshes();
        for (const auto& mesh : meshes)
        {
            Draw(mesh, attributes);
        }

        s_Data.PBRShader->Unbind();

        s_Data.Stats.ModelCount++;
        s_Data.Stats.DrawCalls++;
    }

    void Renderer3D::Draw(const Ref<Mesh>& mesh, const ModelAttributes& attributes)
    {
		LUMINA_ASSERT(mesh, "Renderer3D: Cannot draw null mesh");
		LUMINA_ASSERT(mesh->IsSetup(), "Renderer3D: Cannot draw empty mesh");
		LUMINA_ASSERT(s_Data.PBRShader, "Renderer3D: PBR shader is not initialized");

        auto material = mesh->GetMaterial();
		Material::BindMaterial(s_Data.PBRShader, material, s_Data.WhiteTexture, s_Data.DefaultNormalMap);

        auto vao = mesh->GetVAO();
        if (!vao)
        {
            LUMINA_LOG_WARN("Mesh has no VAO setup");
            return;
        }

        vao->Bind();

        bool hasIndices = mesh->HasIndices();
        size_t indexCount = mesh->GetIndexCount();
        size_t vertexCount = mesh->GetVertexCount();

        switch (s_Data.CurrentRenderMode)
        {
        case RenderMode::Normal:
        {
            RenderCommands::SetPolygonMode(PolygonMode::Fill);
            if (hasIndices)
            {
                RenderCommands::DrawTrianglesIndexed(vao);
                s_Data.Stats.TriangleCount += indexCount / 3;
            }
            else
            {
                RenderCommands::DrawTriangles(vao, vertexCount);
                s_Data.Stats.TriangleCount += vertexCount / 3;
            }
            break;
        }
        case RenderMode::Wireframe:
        {
            RenderCommands::SetPolygonMode(PolygonMode::Line);
            if (hasIndices)
            {
                RenderCommands::DrawTrianglesIndexed(vao);
                s_Data.Stats.TriangleCount += indexCount / 3;
            }
            else
            {
                RenderCommands::DrawTriangles(vao, vertexCount);
                s_Data.Stats.TriangleCount += vertexCount / 3;
            }
            break;
        }
        case RenderMode::Points:
        {
            RenderCommands::SetPolygonMode(PolygonMode::Fill);
            if (hasIndices)
            {
                RenderCommands::DrawPointsIndexed(vao);
            }
            else
            {
                RenderCommands::DrawPoints(vao, vertexCount);
            }
            break;
        }
        }

        vao->Unbind();

        s_Data.Stats.MeshCount++;
        s_Data.Stats.VertexCount += vertexCount;
        s_Data.Stats.TexturesUsed += 5;
    }

    void Renderer3D::Draw(const Ref<Skybox>& skybox, const SkyboxAttributes& attributes)
    {
		LUMINA_ASSERT(skybox, "Renderer3D: Cannot draw null skybox");
		LUMINA_ASSERT(skybox->IsValid(), "Renderer3D: Cannot draw invalid skybox");
		LUMINA_ASSERT(s_Data.SkyboxShader, "Renderer3D: Skybox shader is not initialized");

        RenderCommands::SetDepthMask(false);
        RenderCommands::DisableFaceCulling();
        RenderCommands::SetDepthFunc(DepthFunc::LessEqual);

        s_Data.SkyboxShader->Bind();
        
        glm::mat4 skyboxView = glm::mat4(glm::mat3(s_Data.ViewMatrix));
        glm::mat4 skyboxViewProjection = s_Data.ProjectionMatrix * skyboxView;

        s_Data.SkyboxShader->SetUniformMat4("u_ViewProjection", skyboxViewProjection);

		Skybox::BindAttributes(s_Data.SkyboxShader, attributes);

        auto texture = skybox->GetTexture();
        if (texture)
        {
            texture->Bind(TextureSlots::SKYBOX);
        }

        auto vao = skybox->GetVAO();
        if (vao)
        {
            RenderCommands::DrawTriangles(vao, 36);
        }

        s_Data.SkyboxShader->Unbind();

        RenderCommands::EnableFaceCulling();
        RenderCommands::SetDepthMask(true);
		RenderCommands::SetDepthFunc(DepthFunc::Less);

        s_Data.Stats.DrawCalls++;
        s_Data.Stats.TriangleCount += 12;
    }

    void Renderer3D::Submit(const Ref<Model>& model, const ModelAttributes& attributes)
    {
        LUMINA_ASSERT(model, "Renderer3D: Cannot submit null model");
        LUMINA_ASSERT(!model->IsEmpty(), "Renderer3D: Cannot submit empty model");

        uint64_t modelUUID = model->GetUUID();

        auto& instance = s_Data.m_ModelInstances[modelUUID];
        if (!instance)
        {
            instance = Instance::Create(model);
        }

        instance->AddInstance(attributes);
        if (instance->IsFull())
        {
            Flush();
        }
    }

    void Renderer3D::Flush()
    {
        if (s_Data.m_ModelInstances.empty())
            return;

        s_Data.PBRIShader->Bind();

        s_Data.PBRIShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
        s_Data.PBRIShader->SetUniformVec3("u_CameraPos", s_Data.CameraPosition);
        s_Data.PBRIShader->SetUniformInt("u_RenderMode", static_cast<int>(s_Data.CurrentRenderMode));

        s_Data.PBRIShader->SetUniformVec4("u_TintColor", glm::vec4(1.0f));

        s_Data.PBRIShader->SetUniformVec3("u_DirLight.Direction", s_Data.DirLight.Direction);
        s_Data.PBRIShader->SetUniformVec3("u_DirLight.Color", s_Data.DirLight.Color);
        s_Data.PBRIShader->SetUniformFloat("u_DirLight.Intensity", s_Data.DirLight.Intensity);
        
        s_Data.PBRIShader->SetUniformInt("u_NumPointLights", static_cast<int>(s_Data.PointLights.size()));
        for (size_t i = 0; i < s_Data.PointLights.size() && i < MaxPointLights; ++i)
        {
            std::string base = "u_PointLights[" + std::to_string(i) + "]";
            s_Data.PBRIShader->SetUniformVec3(base + ".Position", s_Data.PointLights[i].Position);
            s_Data.PBRIShader->SetUniformVec3(base + ".Color", s_Data.PointLights[i].Color);
            s_Data.PBRIShader->SetUniformFloat(base + ".Intensity", s_Data.PointLights[i].Intensity);
            s_Data.PBRIShader->SetUniformFloat(base + ".Constant", s_Data.PointLights[i].Constant);
            s_Data.PBRIShader->SetUniformFloat(base + ".Linear", s_Data.PointLights[i].Linear);
            s_Data.PBRIShader->SetUniformFloat(base + ".Quadratic", s_Data.PointLights[i].Quadratic);
        }

        if (s_Data.EnvironmentMap)
        {
            s_Data.EnvironmentMap->Bind(TextureSlots::SKYBOX);
            s_Data.PBRIShader->SetUniformInt("u_EnvironmentMap", 5);
            s_Data.PBRIShader->SetUniformInt("u_HasEnvironmentMap", 1);
        }
        else
        {
            s_Data.PBRIShader->SetUniformInt("u_HasEnvironmentMap", 0);
        }

        if (s_Data.CurrentRenderMode == RenderMode::Points)
        {
            s_Data.PBRIShader->SetUniformFloat("u_PointSize", s_Data.GlobalPointSize);
        }

        for (auto& [uuid, instance] : s_Data.m_ModelInstances)
        {
            if (!instance->IsEmpty())
            {
                auto model = instance->GetModel();
                if (model && !model->GetMeshes().empty())
                {
                    auto mesh = model->GetMeshes()[0];
                    auto material = mesh->GetMaterial();

                    Material::BindMaterial(s_Data.PBRIShader, material, s_Data.WhiteTexture, s_Data.DefaultNormalMap);
                }

                instance->Upload();
                instance->Render();
                instance->Clear();

                s_Data.Stats.DrawCalls++;
                s_Data.Stats.ModelCount += instance->GetInstanceCount();
            }
        }

        s_Data.PBRIShader->Unbind();
    }

    void Renderer3D::SetResolution(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            LUMINA_LOG_WARN("Invalid resolution: {0}x{1}", width, height);
            return;
        }

        s_Data.Width = width;
        s_Data.Height = height;
    }

    glm::vec2 Renderer3D::GetResolution()
    {
        return { s_Data.Width, s_Data.Height };
    }

    void Renderer3D::SetRenderMode(RenderMode mode)
    {
        s_Data.CurrentRenderMode = mode;
    }

    RenderMode Renderer3D::GetRenderMode()
    {
        return s_Data.CurrentRenderMode;
    }

    void Renderer3D::SetGlobalPointSize(float size)
    {
        s_Data.GlobalPointSize = size;
    }

    float Renderer3D::GetGlobalPointSize()
    {
        return s_Data.GlobalPointSize;
    }

    uint32_t Renderer3D::GetImage()
    {
        return s_Data.RendererFrameBuffer->GetColorAttachment();
    }

    void Renderer3D::SetDirectionalLight(const DirectionalLight& light)
    {
        s_Data.DirLight = light;
    }

    void Renderer3D::AddPointLight(const PointLight& light)
    {
        if (s_Data.PointLights.size() < MaxPointLights)
        {
            s_Data.PointLights.push_back(light);
        }
        else
        {
            LUMINA_LOG_WARN("Maximum number of point lights ({0}) reached!", MaxPointLights);
        }
    }

    void Renderer3D::ClearPointLights()
    {
        s_Data.PointLights.clear();
    }

    void Renderer3D::SetEnvironmentMap(const Ref<Texture>& envMap)
    {
        s_Data.EnvironmentMap = envMap;
    }

    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }

    void Renderer3D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }
}